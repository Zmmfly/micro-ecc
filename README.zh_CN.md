micro-ecc
==========
[English](./README.md) | 中文

一个小而快的ECDH和ECDSA实现，适用于8位、32位和64位处理器。

micro-ecc的静态版本（即在编译时选择曲线的版本）可以在 "static "分支中找到。

构建
--------
只要运行xmake[(点击查看详情)](https://xmake.io)，mico-ecc静态库和命令工具就会在`dst`目录下。
```
$ xmake
```

命令行工具
--------
见 [./command/README.zh_CN.md](./command/README.zh_CN.md)

特点
--------

 * 对已知的侧信道攻击有抵抗力。
 * 用C语言编写，可选择GCC内联汇编，用于AVR、ARM和Thumb平台。
 * 支持8、32和64位架构。
 * 代码规模小。
 * 没有动态内存分配。
 * 支持5种标准曲线：secp160r1、secp192r1、secp224r1、secp256r1和secp256k1。
 * BSD 2-clause许可证。

使用说明
-----------
###点的表示方法 ###
压缩的点以标准格式表示，定义在http://www.secg.org/sec1-v2.pdf；未压缩的点以标准格式表示，但没有`0x04`前缀。除了`uECC_decompress()`外，所有函数只接受未压缩的点；使用`uECC_compress()`和`uECC_decompress()`来转换压缩和未压缩的点的表示。

私钥是以标准格式表示的。

### 使用代码 ###

我建议将uECC文件复制到你的项目中（或符号链接）。然后只需`#include "uECC.h"`来使用micro-ecc的功能。

对于Arduino的使用，你可以使用库管理器来下载micro-ecc（**Sketch**=>**Include Library**=>**Manage Libraries**）。然后你可以像其他Arduino库一样使用uECC（uECC应该显示在**Sketch**=>**Import Library**子菜单中）。

每个函数的文档见uECC.h。

### 编译说明 ###

 * 应该用任何支持stdint.h的C/C++编译器来编译（这包括Visual Studio 2013）。
 * 如果你想改变任何uECC编译时选项的默认值（如`uECC_OPTIMIZATION_LEVEL`），你必须在你的Makefile或类似文件中改变它们，以便uECC.c在编译时得到所需的值（即，用`-DuECC_OPTIMIZATION_LEVEL=3`或其他方式编译uECC.c）。
 * 当为Thumb-1平台编译时，你必须使用`-fomit-frame-pointer` GCC选项（当用`-O1`或更高版本编译时，默认启用该选项）。
 * 当为ARM/Thumb-2平台编译时，`uECC_OPTIMIZATION_LEVEL`>=3，你必须使用`-fomit-frame-pointer`GCC选项（当用`-O1`或更高版本编译时，默认启用该选项）。
 * 当为AVR编译时，你必须启用优化功能（用`-O1`或更高版本编译）。
 * 当为Windows编译时，你将需要链接`advapi32.lib`系统库。
