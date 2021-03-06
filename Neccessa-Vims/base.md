# Vi 使用基本原则

在开始其他的内容前，先整理一些vi日常使用的基本原则。

## vi是可以在工程中使用的

不用怀疑，我已经用vi在办公室码了一年多的代码了。在此之前，我也在学校用vi码了三四年的代码。他绝对适合工程中大量代码的情况，而且在某些极为复杂的工程里面，比某些IDE更加适合用来写代码。

## vi可以定制很多东西，但是不要丢了基本的好东西

键盘上的任何按键，任何按键的组合，序列，都能在vi里被影射为你想要的功能。这个非常酷。但是请记住，不要做过了，一些基本的设置，请保留，并且学会使用这些东西，仅仅是使用好这些技能，就能让你使用vi的效率大幅提高。简单一点，就下面这个图片提到的这些快捷键，有多少人真正能掌握完整呢？

![vim快捷键](http://michael.peopleofhonoronly.com/vim/vim_cheat_sheet_for_programmers_print.png)

事实是，仅仅掌握这个图上面的快捷键，就可以很愉快使用vim完成很多工作了。他们说多不多，时不时看看，哪个快捷键以前没注意的，偶尔拿来试试，使用中主动去用，就记完了，而且里面还有一定的规律，没有那么难得。

请记住用vi的一个原则，那就是上面这些映射的功能，都是常用功能，学会使用它们，然后在它们的基础上，另外安排其他新的功能。

## 正则表达是码农的好朋友

熟悉正则表达，结合各种命令行工具，所向披靡。vim内大量的命令都会涉及正则表达，查找，替换，脚本，等等功能，都是正则在背后支持。

## 很多高级的东西并不是你真正需要的

很多人会尝试把vim配置成一个完整的IDE，开好几个侧边栏，显示一堆信息，以让别人觉得自己的vim不比IDE差。

有必要吗？？？

事实是，我尝试过网上的一些vim ide化的插件，也照某些博客将vim武装称一个IDE，然而并没什么用。想明白以下几个点：

- IDE里面有很多功能，你不一定会用，也不一定需要用。
- 某些看起来很重要的功能，其实没那么重要，比如说根据上下文进行代码补全（这个后面讲）
- vim最重要的特性就是轻巧，灵活，这是灵魂，请不要丢掉它。
- 某些看起来浪费时间的做法，有时候反而是最有效的做法，比如不使用语法分析，而是使用grep在代码里面查找某些关键字。（这个也会在后面讲）
- 永远只专注于一个窗口，那些侧边栏除了让你的代码看起来更加拥挤之外，并没什么用。

## 不要拿vi和IDE比

他们就不是一个层次的东西，vi顶多是个编辑器，他只是IDE里面的一个元件。非要纠结于vi能不能比IDE好用，那就跟比较不同集合的两个元素是一样的。有人尝试着把IDE的功能都通过vi的扩展脚本实现，然后就发先vi跟IDE一样启动一次要老半天。这是得不偿失的。

vi作为一个命令行下的编辑器，他的使用应该更加接近命令行软件的原则，一个工具就做好一件事。在vi这里，就是编辑。至于其它的编译，部署，测试，文件管理，静态代码分析，vi肯定可以做，但是不能做到最好，所以我一般就是开两个命令行tab，一个开vi，另一个纯粹用来跑各种命令，编译，调试等。

请记住：

**command line tool set 才是最强的IDE**

vi相对IDE的一个优势，是他在简单配置了语法染色之后，就可以用来写任何一种语言的代码。而IDE，遇到某些比较新的语言，不一定有成熟的IDE支持。

## 不要拿vi和emacs、sublime、atom等等等等对比

同样是编辑器，大家都有各自的特点。萝卜青菜，各有所爱，拒绝撕逼。我选择vi的原因是，在任何linux发行版下面，他都能直接跑起来，比较通用。但是不能不承认，vi有很多不完美的地方，他的界面就是个字符界面，缺少很多字体渲染机制，一个markdown的即时渲染就做不了，各种gui封装版本也各有差异，我最后还是喜欢命令行版本。

如果决定vi不适合自己，快捷键太多，不想学，fine，用其他编辑器便是了，黑猫白猫，能抓到老鼠的就是好猫。不要纠结。

## 偶尔看看别人怎么玩的

其实vi的技巧在网上有很多

- [vim wiki](http://vim.wikia.com/wiki/Vim_Tips_Wiki)
- [vim home](http://www.vim.org/)
- [易水的博客](http://easwy.com/blog/archives/advanced-vim-skills-catalog/)
- [learn vim script the hard way](http://learnvimscriptthehardway.stevelosh.com/)

这些都是很好的vim技巧获得渠道。当然我不保证这些网站啥时候会过期就是了。

最后还要安利下我自己的vim配置中心：

[vim home](https://github.com/johnzeng/vimhome)

现在已经实现了一条命令安装完整工作环境的功能哦～欢迎使用。





