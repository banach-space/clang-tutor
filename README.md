clang-tutor
=========
[![Build Status](https://github.com/banach-space/clang-tutor/workflows/x86-Ubuntu/badge.svg?branch=master)](https://github.com/banach-space/clang-tutor/actions?query=workflow%3Ax86-Ubuntu+branch%3Amaster)
[![Build Status](https://github.com/banach-space/clang-tutor/workflows/x86-Darwin/badge.svg?branch=master)](https://github.com/banach-space/clang-tutor/actions?query=workflow%3Ax86-Darwin+branch%3Amaster)


Example Clang plugins - based on **Clang 10**

**clang-tutor** is a collection of self-contained reference Clang plugins. It's a
tutorial that targets novice and aspiring Clang developers. Key features:

  * **Modern** - based on the latest version of Clang/LLVM (and updated with
    every release)
  * **Complete** - includes build scripts, LIT tests and CI set-up
  * **Out of tree** - builds against a binary Clang/LLVM installation (no
    need to build Clang/LLVM from sources)

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

**HelloWorld** extracts some interesting information from the input
_translation unit_. It visits all [C++ record
declarations](https://github.com/llvm/llvm-project/blob/release/10.x/clang/include/clang/AST/DeclCXX.h#L253)
(more specifically class, struct and union declarations) and counts them.
Recall that translation unit consists of the input source file and all the
header files that it includes (directly or indirectly).

**HelloWorld** prints the results on a file by file
basis, i.e. separately for every header file that has been included. It visits
_all_ declarations - including the ones in header files included by other
header files. This may lead to some surprising results!

You can build and run **HelloWorld** like this:

```bash
# Build the plugin
export LLVM_DIR=<installation/dir/of/clang/10>
mkdir build
cd build
cmake -DCT_LLVM_INSTALL_DIR=$LLVM_DIR <source/dir/clang/tutor>/HelloWorld/
make
# Run the plugin
$LLVM_DIR/bin/clang -cc1 -load libHelloWorld.dylib -plugin hello-world test/HelloWorld-basic.cpp
```

You should see the following output:

```
# Expected output
(clang-tutor) file: <source/dir/clang/tutor>/test/HelloWorld-basic.cpp
(clang-tutor)  count: 3
```

### How To Analyze STL Headers
In order to see what happens with multiple _indirectly_ included header files,
you can run **HelloWorld** on one of the header files from the [Standard
Template Library](https://en.wikipedia.org/wiki/Standard_Template_Library). For
example, you can use the following C++ file that simply includes `vector.h`:

```cpp
// file.cpp
#include <vector>
```

When running a Clang plugin on a C++ file that includes headers from STL, it is
easier to run it with `clang++` (rather than `clang -cc1`) like this:

```bash
$LLVM_DIR/bin/clang++ -c -Xclang -load -Xclang libHelloWorld.dylib -Xclang -plugin -Xclang hello-world file.cpp
```

This way you can be confident that all the necessary include paths (required to
locate STL headers) are automatically added. For the above input file,
**HelloWorld** will print:

* an overview of _all_ header files included when using `#include <vector>`,
  and
* the number of C++ records declared in each.


Note that there are no explicit declarations in `file.cpp` and only one header
file is included. However, the output on my system consists of 37 header files
(one of which contains 371 declarations). Note that the actual output depends
on your host OS, the C++ standard library implementation and its version. Your
results are likely to be different.

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
   * [**HelloWorld**](#helloworld) - counts the number of class, struct and
     union declarations in the input translation unit
   * [**LACommenter**](#lacommenter) - adds comments to literal arguments
     in functions calls
   * [**CodeStyleChecker**](#codestylechecker) - checks whether names in the
     source file adhere to LLVM's coding style guide (issues a warning
     otherwise)

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

**LACommenter** will decorate the invocation of `foo`  as follows:

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

## CodeStyleChecker
This plugin checks whether class, function and variable names adhere to LLVM's
[style
guide](https://llvm.org/docs/CodingStandards.html#name-types-functions-variables-and-enumerators-properly).
If not, a warning is generated.

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
