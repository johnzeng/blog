# CVE-2016-5195 Dirty-cow 简单分析

这个漏洞是去年爆出来的一个linux跨权限写漏洞，基本表现为一个普通用户能直接写root用户持有的文件。这里简单分析下漏洞形成的原因。

本文需要阅读者有以下知识：
- CPU的Physical Address Extension
- Linux下mmap调用基本使用方法
- Linux的Copy On Write

## 一次普通的get_user_pages

先简单介绍几个比较关键的函数：

```
int __get_user_pages(struct task_struct *tsk, struct mm_struct *mm,
		     unsigned long start, int nr_pages, unsigned int gup_flags,
		     struct page **pages, struct vm_area_struct **vmas,
             int *nonblocking)
```
这个函数用于在内核中查找某个用户进程的某个内存地址的页。

```
int handle_mm_fault(struct mm_struct *mm, struct vm_area_struct *vma,
		unsigned long address, unsigned int flags)
```
这个函数负责处理缺页中断。内存中寻页发现哪个页没有的时候，也会调用这个函数。这里的flags表示出错的时候对应的哪些标志位出错，里面其中一位就标志写不可写的内存出错。address表示用户需要访问的虚地址。

```
struct page *follow_page_mask(struct vm_area_struct *vma,
                    unsigned long address, unsigned int flags,
                    unsigned int *page_mask)
```
这个函数用于根据页表找具体地址所在的页。其中address表示用户进程的虚地址，flags表示用户需要的权限，page_mask会返回当前该页的权限。

下面简单描述一下这几个函数走的流程。

在Linux内核的`__get_user_pages`函数里面，有一个循环，里面会多次尝试调用`follow_page_mask`（或者是`follow_page`）来获得想要查找的页，请求的内容包括页的地址和请求的权限。然后在调用失败之后会进入出错处理，并且`handle_mm_fault`，进行缺页处理。

他们三个的调用关系如下：
```
__get_user_pages ->
	while follow_page_mask failed ->
		handle_mm_fault
	done
end
```

对于一个root用户拥有的文件，被mmap为一个private，只读映射的时候，如果有人尝试通过get_user_pages对其进行写，并且该页不在内存里面。会有下面的三次`follow_page_mask`和`handle_mm_fault`调用：

```
1,页不存在于内存，就把页从硬盘里加在起来，或者swap回来。
2,页存在，但是对该用户只读，所以就创建一个页的copy，也就是Copy On Write。
3,此时一切正常，返回0表示ok，所有的缺页问题已经处理了，获得了对应的拷贝页。
```

**注意**这个循环中，由于第二次handle_mm_fault会处理权限错误，在写权限错误的时候，handle_mm_fault会做COW，然后将follow_page_mask请求的flag里面的写请求位移除：

```
	while follow_page_mask failed{
		handle_mm_fault;
		....
		if ((ret & VM_FAULT_WRITE) &&
				    !(vma->vm_flags & VM_WRITE))
					foll_flags &= ~FOLL_WRITE;
	}

```

这里的ret是handle_mm_fault的返回，foll_flags是follow_page_mask的输入。这里将WRITE去掉了，所以下次follow_page_mask不再检查写权限。

## COW的时候内存到底发生了什么事情

实际上，在内核内部，发生COW的时候，内核会做下面的事情：
```
1，通过虚拟地址找页表，找到对应的页在内存的位置。
2，发现该页只能读，不能写。但是这个写势在必行。
3，将旧的页拷贝到一个新的内存地址。
4，内核修改页表，将新的页地址写到页表去。
```

经过上面的逻辑，用户进程的虚地址没有变化，但是cpu通过页表找到的页就是拷贝的页了。

## 一次普通的madvise DONTNEED移除影射。

在内存发现某个页不需要使用的时候，他会做以下事情：
 ```
 1，将旧的page frame mark为not using。
 2，将页表里面映射到这个页的项移除。
 ```

## 同时mavise DONTNEED和get_user_pages导致竞态问题
一般来说，如果正常循环，理想情况执行顺序是这样的：
```
1, madvise DONTNEED
2, get_user_pages三次follow_page_mask循环
3, write
4, loop to 1
```

这个是理想的，但是，由于get_user_pages和madvise没有加锁，所以两个函数在两个ptrhead下面可同时执行，可能存在如下之行顺序：

```
1, madvise DONTNEED
2, get_user_pages
3, 两次follow_page_mask
4, foll_flags &= ~FOLL_WRITE
5, 有一次madvise DONTNEED
6， 又一次调用follow_page_mask
7, write
```

这里的问题就是，由于第五步发生了DONTNEED，所以COW的页被drop了。下一次follow_page_mask不需要写权限，就返回了原始的，本来不能写的page。然后用户直接对其进行了写操作，导致这个页变成了dirty page，在后面内核write back的时候，就造成了越权写。

## 一些关于poc的问题：

dirty_cow提供了完整的poc，但是网上提供的一些关于poc的讲解不是特别清晰。这里只是做一个简单记录，不确定有没人有兴趣。

### ptrace，/proc/self/mem

事实是，这个不清楚跟`ptrace`，`/proc/self/mem`有什么关系，可能最直接的就是`ptrace`的写操作调用了`get_user_pages`，写`mem`设备也会调用这个函数，导致bug出现。

### 直接写mmap的buffer

如map的内存只可读，那么写的时候会出现segment fault。由于用户空间内存错误是直接中断，然后调用mm_fault函数，所以不存在字段被抹去的情况。

### 写入到mmap的原始页之后为啥会被写回到文件

实际上，这里有一个问题，有一个理解错误。mmap的man文档写的是，在`msync`和`munmap`的时候，会把mmap的内容写回到文件，但是实际上是错误的。如果仔细读内核的代码，会发现，只有`msync`调用了`vsf_write`，但是`munmap`并没有调用这个接口。`munmap`根本没有将内存的数据回写，现在的版本的Linux内核使用的思路是，使用一个`write_back`内核线程，去扫描页表里面的脏页，然后将脏页回写的这种策略。所以哪怕你不调用任何接口，mmap也会回写，只是时间的问题而已。


