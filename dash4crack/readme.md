# 针对 dash3 license 破解 dash4

dash前一阵子发布了 dash4，新的版本，我看了一眼更新，嗯，没啥特别大的新功能。。。然而这个作者丧心病狂地要求付费更新。怪不得当年2升3的时候也被喷了。果断不更新，我就看个文档而已，买一个license还不够，还要付费升级，坑。

我下载了试用版的 dash4，其实免费也能使用完整功能，只是偶尔搜索文档需要等待10s，然后右上角有一个purchase，偏好设置里面的购买也提示需要**付费升级**。

注意这里有一个很重要的信息，那就是他不是提示购买，而是提示**付费升级**，也就是他能识别到我本地有一个dash3的license。那能不能简单修改一下二进制文件，让他以为我用的license就是合法的dash4的license呢？

## 开始简单的逆向

先找到dash的二进制文件，mac的app二进制文件一般在 `*.app/Contents/MacOS/` 下面，二进制文件的名字一般和包名相同。

使用 hopper 打开这个二进制文件，进行分析。可以看到，这个二进制文件没有尽兴任何反逆向处理，hopper 成功将所有导出的函数都列出来了。考虑到dash使用license进行注册，我猜测他的函数里面会有以下关键字：

```
license
purchase
paid
verify
check
```

使用license进行查找，果然找到了几个有用的函数：

```
void -[DHInApp setLicensePurchaseDescription](void * self, void * _cmd) 
void * -[DHInApp licenseFile](void * self, void * _cmd)
void * -[DHInApp licenseFolder](void * self, void * _cmd)
void -[DHInApp processLicenseFile:](void * self, void * _cmd, void * arg2)
void -[DHAnalytics trackAppLaunch:isPaid:](void * self, void * _cmd, void * arg2, char arg3)
```

很明显，其中 `licenseFile` 和 `licenseFolder` 就是用来找license的。简单分析了一下这两个函数，发现了license的路径：

`~/Library/Application\ Support/Dash/License/license.dash-license`

OK，我们看看这个文件是什么样子的：

```
➜  License cat license.dash-license
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>Product</key>
	<string>Dash</string>
	<key>Name</key>
	<string>happy_zengda@126.com</string>
	<key>Email</key>
	<string>happy_zengda@126.com</string>
	<key>Licenses</key>
	<string>1</string>
	<key>Timestamp</key>
	<string>1476971829</string>
	<key>Version</key>
	<string>3</string>
	<key>Signature</key>
	<data>dccxgF2vo1+M6IXkERa3Rs7ND2NzItG5erTEhXAX9wHV/9Z5cPg7b1bN2nwIsvOG3cQqb2o6nKEK
PtcZoDjsT1AHvLp1DjNRCYdTjc72la5U9IOUR8RS14ydP11DqDhgbO86xiC/C2cAp+leiw8utexA
m1xuSlAwxRx2XfmTvqc=
</data>
</dict>
</plist>
```

可以看到这个是一个xml文件，几个关键的key，包括product，version，signature。可以看到，这个license是指定了版本3的dash，并且根据时间戳打上了签名。

## 简单动态调试

ok，已经拿到上面的信息了，我们现在需要确定，誰调用并且检查了license。我们用 hopper 给 `licenseFile` 和 `licenseFolder` 两个调用打上断点，使用 hopper 的调试器运行该二进制文件。事实上，hopper只是提供了一个lldb的gui，hopper的local debugger就是调用的lldb，所以如果你没有hopper，用lldb一样可以撸得很开心。

启动调试器之后，我们看到该断点第一次调用的堆栈是这样的（我没有对class做记录，理解下哈，2333）：

```
DHCustomWindw awakeFromNib
        addPurchaseButtonToWindow
            displayString
            		licenseFolder

```

很明显，`DHCustomWindw` 就是主窗口，然后 `addPurchaseButtonToWindow` 就是给主窗口右上角加上 `purchase` 按钮的调用。

 简单分析 addPurchaseButton 之后，发现在调用 `displayString` 返回 `@"com.barebones.textwrangler"` 的时候，就会被认为是注册成功。嗯，我也不懂作者的脑洞怎么想的，用这个来做注册成功的标记。。。。
 
 我进一步分析了其他几个调用了license进行注册检查的函数，我发现都是这样做的。很神奇的思维。。。。
 
 那么现在的问题，就是，如何让 `displayString` 返回 `@"com.barebones.textwrangler"`
 
 ## 对 displayString 进行分析
 
 Hopper 带有将汇编代码反编译为伪代码的功能，我将 `displayString` 函数翻译为伪代码。我这里简单罗列一下伪代码的逻辑：
 
 ```
 调用loadingString，如果出错，返回错误
 检查是不是存在一个叫做invalidTest的文件，如果存在，进行简单测试
 检查是不是有license文件，如果不存在，返回注册失败
 检查license文件的内容是否合法（签名是不是合法）
 检查license文件里面的version字段是不是小于4，
 	是，认为有合法的旧license
 	否，认为是合法license，设置为已购买，并且返回`@"com.barebones.textwrangler"`
 ```
 
 我又进行了一次简单的动态调试，发现在有dash3的license的时候，程序会愉快运行到检查license version的那一行，然后因为version是3，下降到旧license的逻辑。
 
 ## 修改displayString函数
 
 为了修改二进制文件，我们简单看看该函数检查version的相关汇编代码：
 
 ```
 		1.	00000001000dee4c         mov        rdi, r13                                    ; argument "instance" for method _objc_msgSend, CODE XREF=-[DHInApp displayString]+794
	2.	00000001000dee4f         mov        rsi, rbx                                    ; argument "selector" for method _objc_msgSend
	3.	00000001000dee52         call       r15                                         ; _objc_msgSend
	4.	00000001000dee55         mov        rdi, r13                                    ; argument "instance" for method _objc_msgSend
	5.	00000001000dee58         mov        rsi, r12                                    ; argument "selector" for method _objc_msgSend
	6.	00000001000dee5b         call       r15                                         ; _objc_msgSend
	7.	00000001000dee5e         cmp        rax, 0x4
	8.	00000001000dee62         jl         loc_1000deebe
	9.	
	10.	00000001000dee64         mov        rsi, qword [0x10030f5a0]                    ; @selector(setActivatedByLicense:), argument "selector" for method _objc_msgSend
	11.	00000001000dee6b         mov        edx, 0x1
	12.	00000001000dee70         mov        rdi, r13                                    ; argument "instance" for method _objc_msgSend
	13.	00000001000dee73         call       r15                                         ; _objc_msgSend
	14.	00000001000dee76         mov        rsi, qword [0x10030f530]                    ; @selector(setLoadingString:), argument "selector" for method _objc_msgSend
	15.	00000001000dee7d         lea        rdx, qword [cfstring_com_barebones_textwrangler] ; @"com.barebones.textwrangler"
	16.	00000001000dee84         mov        rdi, r13                                    ; argument "instance" for method _objc_msgSend
	17.	00000001000dee87         call       r15                                         ; _objc_msgSend
	18.	00000001000dee8a         mov        rdi, qword [objc_cls_ref_NSThread]          ; argument "instance" for method _objc_msgSend
	19.	00000001000dee91         mov        rsi, qword [0x100308d68]                    ; @selector(isMainThread), argument "selector" for method _objc_msgSend
	20.	00000001000dee98         call       r15                                         ; _objc_msgSend
	21.	00000001000dee9b         test       al, al

 ```
 
 在第七行，我们看到，他将`rax`和`0x4`进行比较，如果是little，就跳到`loc_1000deebe`的地址，否则不跳转，继续执行 `setActivatedByLicense` 等的内容，即激活成功。
 
 那我们需要做的，可以是以下几个选择：
 
 ```
 1,在cmp之前将rax设置为4，即将call r15替换成mov rax,0x4
 2，将jl 改成jg，也就是让他不跳转
 3，将jmp替换成nop，无操作
 4，cmp rax, 0x4变成cmp rax, 0x3
 ```
 
 由于`mov rax,0x4` 的二进制代码长度比call调用长，所以第一个办法会覆盖后面的有效代码。
 另外几种方法都是可以的。我选择第4种方法，这样就将dash4的license检查变成了只要是dash3的lincese就可以通过检查了。
 
 好了，保存二进制文件，由于mac的app会有一个开发者的签名文件，保证二进制不被修改。修改了二进制之后，该签名不再有用，需要移除签名。hopper在保存二进制文件的时候，会帮助我们自动移除这个签名。
 
 重新打开app，发现purchase的按钮不再出现了。进入偏好里面，purchase里面也不再有购买按钮，而是actived。
 
 Done。


