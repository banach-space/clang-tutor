clang-tutor
=========
[![Build Status](https://github.com/banach-space/clang-tutor/workflows/x86-Ubuntu/badge.svg?branch=master)](https://github.com/banach-space/clang-tutor/actions?query=workflow%3Ax86-Ubuntu+branch%3Amaster)
[![Build Status](https://github.com/banach-space/clang-tutor/workflows/x86-Darwin/badge.svg?branch=master)](https://github.com/banach-space/clang-tutor/actions?query=workflow%3Ax86-Darwin+branch%3Amaster)


Example Clang plugins - based on **Clang 10**

**clang-tutor** is a collection of self-contained reference Clang plugins. It's a
tutorial that targets novice and aspiring Clang developers. Key features:
  * **Complete** - includes `CMake` build scripts, LIT tests and CI set-up
  * **Out of source** - builds against a binary Clang/LLVM installation (no
    need to build Clang from sources)
  * **Modern** - based on the latest version of Clang/LLVM (and updated with
    every release)

### Overview
TODO

### Table of Contents
* [HelloWorld](#helloworld)
* [Building & Testing](#building--testing)
* [Overview of the Plugins](#overview-of-the-plugins)
* [References](#References)
* [License](#license)

Status
======
**Work in progress**

Everything builds fine and all tests pass. This project is under active
development. More content to be added soon.

HelloWorld
==========
The **HelloWorld** plugin from
[HelloWorld.cpp](https://github.com/banach-space/clang-tutor/blob/master/HelloWorld/HelloWorld.cpp)
is a self-contained *reference example*. The corresponding
[CMakeLists.txt](https://github.com/banach-space/clang-tutor/blob/master/HelloWorld/CMakeLists.txt)
implements the minimum set-up for an out-of-tree plugin.

For every class declared in the input file, **HelloWord** prints its name,
location of the declaration and the number of its base classes. You can build
it like this:

```bash
export LLVM_DIR=<installation/dir/of/clang/10>
mkdir build
cd build
cmake -DCT_LLVM_INSTALL_DIR=$LLVM_DIR <source/dir/clang/tutor>/HelloWorld/
make
```

and run it with `clang` like this:

```bash
# Run the plugin
$LLVM_DIR/bin/clang -cc1 -load libHelloWorld.dylib -plugin hello-world test/input_for_hello.cpp
# Expected output
(clang-tutor) Hello from: Base1
(clang-tutor)  location: 3:  1
(clang-tutor)  number of bases classes 0
(clang-tutor) Hello from: Base2
(clang-tutor)  location: 6:  1
(clang-tutor)  number of bases classes 0
(clang-tutor) Hello from: Derived1
(clang-tutor)  location: 9:  1
(clang-tutor)  number of bases classes 1
(clang-tutor) Hello from: Derived2
(clang-tutor)  location: 12:  1
(clang-tutor)  number of bases classes 2
```

Building & Testing
===================
You can build **clang-tutor** (and all the provided plugins) as follows:
```bash
cd <build/dir>
cmake -DCT_LLVM_INSTALL_DIR=<installation/dir/of/clang/10> <source/dir/clang-tutor>
make
```

The `CT_LLVM_INSTALL_DIR` variable should be set to the root of either the
installation or build directory of Clang 10. It is used to locate the
corresponding `LLVMConfig.cmake` script that is used to set the include and
library paths.

In order to run the tests, you need to install **llvm-lit** (aka **lit**). It's
not bundled with LLVM 10 packages, but you can install it with **pip**:
```bash
# Install lit - note that this installs lit globally
pip install lit
```
Running the tests is as simple as:
```bash
$ lit <build_dir>/test
```
Voilà! You should see all tests passing.

Overview of The Plugins
=======================
   * [**HelloWorld**](#helloworld) - prints the name, the location and number
     of base classes for each class declared the input module
   * [**LACommenter**](#lacommenter) - add comments for literal arguments in
     functions calls

Once you've [built](#build-instructions) this project, you can experiment with
every plugin separately. All plugins take C and C++ files as input.  All
plugins, except for [**HelloWorld**](#helloworld), are described below.

## LACommenter
The **LACommenter** (Literal Argument Commenter) plugin will comment literal
arguments in function calls. For example, in the following input code:

```c
extern void foo(int some_arg);

void bar() {
  foo(123);
}
```

the invocation of `foo` is commented as follows:

```c
extern void foo(int some_arg);

void bar() {
  foo(/*some_arg=*/123);
}
```

This commenting style follows LLVM's [oficial guidelines](
https://llvm.org/docs/CodingStandards.html#comment-formatting). **LACommenter**
will comment character, integer, floating point, boolean and string literal
arguments.

This plugin is based on a similar example from a
[presentation](https://s3.amazonaws.com/connect.linaro.org/yvr18/presentations/yvr18-223.pdf)
by Peter Smith.

### Run the plugin

```bash
$LLVM_DIR/bin/clang -cc1 -load lib/libLACommenter.dylib -plugin LAC test/LACLong.cpp
```

### Run the plugin through `lacommenter`

```bash
bin/lacommenter test/LACLong.cpp
```

References
==========
Below is a list of clang resources available outside the official online
documentation that I have found very helpful. Where possible, the items are
sorted by date.

* **Examples in Clang**
  * Refactoring tool template:
    [clang-tools-extra/tool-template](https://github.com/llvm/llvm-project/tree/release/10.x/clang-tools-extra/tool-template)
* **Clang Tool Development**
  * _"How to build a C++ processing tool using the Clang libraries"_, Peter Smith, Linaro Connect 2018
  ([video](https://www.youtube.com/watch?reload=9&v=8QvLVEaxzC8), [slides](https://s3.amazonaws.com/connect.linaro.org/yvr18/presentations/yvr18-223.pdf))

License
========
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to http://unlicense.org/
