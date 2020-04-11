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

Status
======
**Work in progress**

Everything builds fine and all tests pass. This project is under active
development. More content to be added soon.

Building & Testing
===================
You can build **clang-tutor** (and all the provided plugins) as follows:
```bash
cd <build/dir>
cmake -DCT_LLVM_INSTALL_DIR=<installation/dir/of/llvm/10> <source/dir/clang-tutor>
make
```

The `CT_LLVM_INSTALL_DIR` variable should be set to the root of either the
installation or build directory of LLVM 10. It is used to locate the
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
