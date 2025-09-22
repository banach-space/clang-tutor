clang-tutor
=========
[![Apple Silicon](https://github.com/banach-space/clang-tutor/actions/workflows/apple-silicon.yml/badge.svg)](https://github.com/banach-space/clang-tutor/actions/workflows/apple-silicon.yml)
[![x86-Ubuntu](https://github.com/banach-space/clang-tutor/actions/workflows/x86-ubuntu.yml/badge.svg)](https://github.com/banach-space/clang-tutor/actions/workflows/x86-ubuntu.yml)


Example Clang plugins for C and C++ - based on **Clang 21**

**clang-tutor** is a collection of self-contained reference Clang plugins. It's a
tutorial that targets novice and aspiring Clang developers. Key features:

  * **Modern** - based on the latest version of Clang (and updated with
    every release)
  * **Complete** - includes build scripts, LIT tests and CI set-up
  * **Out of tree** - builds against a binary Clang installation (no
    need to build Clang from sources)

Corrections and feedback always welcome!

### Overview
[Clang](https://clang.llvm.org/) (together with
[LibTooling](https://clang.llvm.org/docs/LibTooling.html)) provides a very
powerful API and infrastructure for analysing and modifying source files from
the C language family. With Clang's plugin
[framework](https://clang.llvm.org/docs/ClangPlugins.html) one can relatively
easily create bespoke tools that aid development and improve productivity. The
aim of **clang-tutor** is to showcase this framework through small,
self-contained and testable examples, implemented using [idiomatic
LLVM](https://llvm.org/docs/CodingStandards.html).

This document explains how to set-up your environment, build and run the
project, and go about debugging. The source files, apart from the code itself,
contain comments that will guide you through the implementation. The
[tests](https://github.com/banach-space/clang-tutor/tree/main/test) highlight what edge
cases are supported, so you may want to skim through them as well.

### Table of Contents
* [HelloWorld](#helloworld)
* [Development Environment](#development-environment)
* [Building & Testing](#building--testing)
* [Overview of the Plugins](#overview-of-the-plugins)
* [References](#References)
* [License](#license)

HelloWorld
==========
The **HelloWorld** plugin from
[HelloWorld.cpp](https://github.com/banach-space/clang-tutor/blob/main/HelloWorld/HelloWorld.cpp)
is a self-contained *reference example*. The corresponding
[CMakeLists.txt](https://github.com/banach-space/clang-tutor/blob/main/HelloWorld/CMakeLists.txt)
implements the minimum set-up for an out-of-tree plugin.

**HelloWorld** extracts some interesting information from the input
_translation unit_. It visits all [C++ record
declarations](https://github.com/llvm/llvm-project/blob/release/21.x/clang/include/clang/AST/DeclCXX.h#L253)
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
export Clang_DIR=<installation/dir/of/clang/21>
export CLANG_TUTOR_DIR=<source/dir/clang/tutor>
mkdir build
cd build
cmake -DCT_Clang_INSTALL_DIR=$Clang_DIR $CLANG_TUTOR_DIR/HelloWorld/
make
# Run the plugin
$Clang_DIR/bin/clang -cc1 -load ./libHelloWorld.{so|dylib} -plugin hello-world $CLANG_TUTOR_DIR/test/HelloWorld-basic.cpp
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
$Clang_DIR/bin/clang++ -c -Xclang -load -Xclang libHelloWorld.dylib -Xclang -plugin -Xclang hello-world file.cpp
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

Development Environment
=======================
## Platform Support And Requirements
**clang-tutor** has been tested on **Ubuntu 20.04** and **Mac OS X 10.14.6**. In
order to build **clang-tutor** you will need:

  * LLVM 21 and Clang 21
  * C++ compiler that supports C++17
  * CMake 3.13.4 or higher

As Clang is a subproject within
[llvm-project](https://github.com/llvm/llvm-project), it depends on LLVM (i.e.
**clang-tutor** requires development packages for both Clang and LLVM).

There are additional requirements for tests (these will be satisfied by
installing LLVM 21):
  * [**lit**](https://llvm.org/docs/CommandGuide/lit.html) (aka **llvm-lit**,
    LLVM tool for executing the tests)
  * [**FileCheck**](https://llvm.org/docs/CommandGuide/FileCheck.html) (LIT
    requirement, it's used to check whether tests generate the expected output)

## Installing Clang 21 On Mac OS X
On Darwin you can install Clang 21 and LLVM 21 with
[Homebrew](https://brew.sh/):

```bash
brew install llvm
```

If you already have an older version of Clang and LLVM installed, you can
upgrade it to Clang 21 and LLVM 21 like this:

```bash
brew upgrade llvm
```

Once the installation (or upgrade) is complete, all the required header files,
libraries and tools will be located in `/usr/local/opt/llvm/`.

## Installing Clang 21 On Ubuntu
On Ubuntu Jammy Jellyfish, you can [install modern
LLVM](https://blog.kowalczyk.info/article/k/how-to-install-latest-clang-6.0-on-ubuntu-16.04-xenial-wsl.html)
from the official [repository](http://apt.llvm.org/):

```bash
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
sudo apt-add-repository "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-21 main"
sudo apt-get update
sudo apt-get install -y llvm-21 llvm-21-dev libllvm21 llvm-21-tools clang-21 libclang-common-21-dev libclang-21-dev libmlir-21 libmlir-21-dev
```
This will install all the required header files, libraries and tools in
`/usr/lib/llvm-21/`.

## Building Clang 21 From Sources
Building from sources can be slow and tricky to debug. It is not necessary, but
might be your preferred way of obtaining LLVM/Clang 21. The following steps
will work on Linux and Mac OS X:

```bash
git clone https://github.com/llvm/llvm-project.git
cd llvm-project
git checkout release/21.x
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DLLVM_TARGETS_TO_BUILD=host -DLLVM_ENABLE_PROJECTS="clang;libcxx;libcxxabi" <llvm-project/root/dir>/llvm/
cmake --build .
```
For more details read the [official
documentation](https://llvm.org/docs/CMake.html).

### Note for macOS users
As per this great
[description](https://quuxplusone.github.io/blog/2019/11/09/llvm-from-scratch/)
by Arthur O’Dwyer , add `-DDEFAULT_SYSROOT="$(xcrun --show-sdk-path)"` to your
CMake invocation when building Clang from sources. Otherwise, `clang`  won't be
able to find e.g. standard C headers (e.g.  `wchar.h`).

Building & Testing
===================
You can build **clang-tutor** (and all the provided plugins) as follows:
```bash
cd <build/dir>
cmake -DCT_Clang_INSTALL_DIR=<installation/dir/of/clang/21> <source/dir/clang-tutor>
make
```

The `CT_Clang_INSTALL_DIR` variable should be set to the root of either the
installation or build directory of Clang 21. It is used to locate the
corresponding `LLVMConfig.cmake` script that is used to set the include and
library paths.

In order to run the tests, you need to install **llvm-lit** (aka **lit**). It's
not bundled with LLVM 21 packages, but you can install it with **pip**:
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
This table contains a summary of the examples available in **clang-tutor**. The
_Framework_ column refers to a plugin framework available in Clang that was
used to implement the corresponding example. This is either
[RecursiveASTVisitor](https://clang.llvm.org/docs/RAVFrontendAction.html),
[ASTMatcher](https://clang.llvm.org/docs/LibASTMatchersTutorial.html) or both.

| Name      | Description     | Framework |
|-----------|-----------------|-----------|
|[**HelloWorld**](#helloworld) | counts the number of class, struct and union declarations in the input translation unit | RecursiveASTVisitor |
|[**LACommenter**](#lacommenter) | adds comments to literal arguments in functions calls | ASTMatcher |
|[**CodeStyleChecker**](#codestylechecker) | issue a warning if the input file does not follow one of [LLVM's coding style guidelines](https://llvm.org/docs/CodingStandards.html#name-types-functions-variables-and-enumerators-properly) | RecursiveASTVisitor |
|[**Obfuscator**](#obfuscator) | obfuscates integer addition and subtraction | ASTMatcher |
|[**UnusedForLoopVar**](#unusedforloopvar) | issue a warning if a for-loop variable is not used | RecursiveASTVisitor + ASTMatcher |
|[**CodeRefactor**](#coderefactor) | rename class/struct method names | ASTMatcher |

Once you've [built](#building--testing) this project, you can experiment with
every plugin separately. All of them accept C and C++ files as input. Below you
will find more detailed descriptions  (except for **HelloWorld**, which is
documented [here](#helloworld)).

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

This plugin is based on a similar example by Peter Smith presented
[here](https://static.linaro.org/connect/yvr18/presentations/yvr18-223.pdf).

### Run the plugin
You can test **LACommenter** on the example presented above. Assuming that it
was saved in `input_file.c`, you can add comments to it as follows:

```bash
$Clang_DIR/bin/clang -cc1 -load <build_dir>/lib/libLACommenter.dylib -plugin LAC input_file.cpp
```

### Run the plugin through `ct-la-commenter`
**lacommenter** is a standalone tool that will run the **LACommenter** plugin,
but without the need of using `clang` and loading the plugin:

```bash
<build_dir>/bin/ct-la-commenter input_file.cpp --
```

If you don't append `--` at the end of tools invocation will get the complain
from Clang tools about missing compilation database as follow:

```
Error while trying to load a compilation database:
Could not auto-detect compilation database for file "input_file.cpp"
No compilation database found in <source/dir/clang-tutor> or any parent directory
fixed-compilation-database: Error while opening fixed database: No such file or directory
json-compilation-database: Error while opening JSON database: No such file or directory
Running without flags.
```
Another workaround to solve the issue is to set the
[CMAKE_EXPORT_COMPILE_COMMANDS flag](https://cmake.org/cmake/help/v3.14/variable/CMAKE_EXPORT_COMPILE_COMMANDS.html)
during the CMake invocation. It will give you the compilation database into your
build directory with the filename as compile_commands.json. More detailed
explanation about it can be found on [Eli Bendersky's blog](https://eli.thegreenplace.net/2014/05/21/compilation-databases-for-clang-based-tools).

## CodeStyleChecker
This plugin demonstrates how to use Clang's
[DiagnosticEngine](https://github.com/llvm/llvm-project/blob/release/21.x/clang/include/clang/Basic/Diagnostic.h#L191)
to generate custom compiler warnings.  Essentially, **CodeStyleChecker** checks
whether names of classes, functions and variables in the input translation unit
adhere to LLVM's [style
guide](https://llvm.org/docs/CodingStandards.html#name-types-functions-variables-and-enumerators-properly).
If not, a warning is printed. For every warning, **CodeStyleChecker** generates
a suggestion that would fix the corresponding issue. This is done with the
[FixItHint](https://github.com/llvm/llvm-project/blob/release/21.x/clang/include/clang/Basic/Diagnostic.h#L70)
API.
[SourceLocation](https://github.com/llvm/llvm-project/blob/release/21.x/clang/include/clang/Basic/SourceLocation.h#L86)
API is used to generate valid source location.

**CodeStyleChecker** is robust enough to cope with complex examples like
`vector.h` from STL, yet the actual implementation is fairly compact. For
example, it can correctly analyze names expanded from macros and knows that it
should ignore [user-defined conversion
operators](https://en.cppreference.com/w/cpp/language/cast_operator).


### Run the plugin
Let's test **CodeStyleChecker** on the following file:

```cpp
// file.cpp
class clangTutor_BadName;
```

The name of the class doesn't follow LLVM's coding guide and
**CodeStyleChecker** indeed captures that:

```bash
$Clang_DIR/bin/clang -cc1 -fcolor-diagnostics -load libCodeStyleChecker.dylib -plugin CSC file.cpp
file.cpp:2:7: warning: Type and variable names should start with upper-case letter
class clangTutor_BadName;
      ^~~~~~~~~~~~~~~~~~~
      ClangTutor_BadName
file.cpp:2:17: warning: `_` in names is not allowed
class clangTutor_BadName;
      ~~~~~~~~~~^~~~~~~~~
      clangTutorBadName
2 warnings generated.
```

There are two warnings generated as two rules have been violated. Alongside
every warning, a suggestion (i.e. a `FixItHint`) that would make the
corresponding warning go away. Note that **CodeStyleChecker** also supplements
the warnings with correct source code information.

`-fcolor-diagnostics` above instructs Clang to generate color output
(unfortunately Markdown doesn't render the colors here).

### Run the plugin and compile the input

In the invocation from the previous section, Clang runs only one action - the plugin itself. This means that no output files are generated. In order to run a plugin action _and_ e.g. a compilation action, you need implement `getActionType` method (from [Clang Plugins](https://clang.llvm.org/docs/ClangPlugins.html#using-the-clang-command-line)):
> If the plugin class implements the `getActionType` method then the plugin is run automatically. 
```c
// Automatically run the plugin after the main AST action
PluginASTAction::ActionType getActionType() override {
  return AddAfterMainAction;
}
```

The **CodeStyleChecker** plugin does implement `getActionType` and hence can be run automatically and used during the normal compilation 
process to detect errors in the code, and get the output file, for example:
```bash
$Clang_DIR/bin/clang -fplugin=libCodeStyleChecker.dylib -o file.o -c file.cpp
file.cpp:2:7: warning: Type and variable names should start with upper-case letter
class clangTutor_BadName;
      ^~~~~~~~~~~~~~~~~~~
      ClangTutor_BadName
file.cpp:2:17: warning: `_` in names is not allowed
class clangTutor_BadName;
      ~~~~~~~~~~^~~~~~~~~
      clangTutorBadName
2 warnings generated.
```

### Run the plugin through `ct-code-style-checker`
**ct-code-style-checker** is a standalone tool that will run the **CodeStyleChecker** plugin,
but without the need of using `clang` and loading the plugin:

```bash
<build_dir>/bin/ct-code-style-checker input_file.cpp --
```


## Obfuscator
The **Obfuscator** plugin will rewrite integer addition and subtraction
according to the following formulae:

```
a + b == (a ^ b) + 2 * (a & b)
a - b == (a + ~b) + 1
```

The above transformations are often used in code obfuscation. You may also know
them from [Hacker's
Delight](https://www.amazon.co.uk/Hackers-Delight-Henry-S-Warren/dp/0201914654).

The plugin runs twice over the input file. First it scans for integer
additions. If any are found, the input file is updated and printed to stdout.
If there are no integer additions, there is no output. Similar logic is
implemented for integer subtraction.

Similar code transformations are possible at the [LLVM
IR](https://llvm.org/docs/LangRef.html) level. In particular, see
[MBAsub](https://github.com/banach-space/llvm-tutor#mbasub) and
[MBAAdd](https://github.com/banach-space/llvm-tutor#mbaadd) in
[**llvm-tutor**](https://github.com/banach-space/llvm-tutor).

### Run the plugin
Lets use the following file as our input:

```c
int foo(int a, int b) {
  return a + b;
}
```

You can run the plugin like this:

```bash
$Clang_DIR/bin/clang -cc1 -load <build_dir>/lib/libObfuscator.dylib -plugin Obfuscator input.cpp
```

You should see the following output on your screen.

```c
int foo(int a, int b) {
  return (a ^ b) + 2 * (a & b);
}
```

## UnusedForLoopVar
This plugin detects unused for-loop variables (more specifically, the variables
defined inside the
[traditional](https://en.cppreference.com/w/cpp/language/for) and
[range-based](https://en.cppreference.com/w/cpp/language/range-for) `for`
loop statements) and issues a warning when one is found. For example, in
function `foo` the loop variable `j` is not used:

```c
int foo(int var_a) {
  for (int j = 0; j < 10; j++)
    var_a++;

  return var_a;
}
```

**UnusedForLoopVar** will warn you about it. Clearly the for loop in this case
can be replaced with `var_a += 10;`, so **UnusedForLoopVar** does a great job
in drawing developer's attention to it. It can also detect unused loop
variables in range for loops, for example:

```c++
#include <vector>

int bar(std::vector<int> var_a) {
  int var_b = 10;
  for (auto some_integer: var_a)
    var_b++;

  return var_b;
}

```

In this case, `some_integer` is not used and **UnusedForLoopVar** will
highlight it. The loop could be replaced with a much simpler expression: `var_b
+= var_a.size();`.

Obviously unused loop variables _may_ indicate an issue or a potential
optimisation (e.g. unroll the loop) or a simplification (e.g. replace the loop
with one arithmetic operation). However, that does not have to be the case and
sometimes we have good reasons not to use the loop variable.
If the name of a loop variable matches the `[U|u][N|n][U|u][S|s][E|e][D|d]`
then it will be ignored by"**UnusedForLoopVar**. For example, the following
modified version of the above example will not be reported:

```c
int foo(int var_a) {
  for (int unused = 0; unused < 10; unused++)
    var_a++;

  return var_a;
}
```

**UnusedForLoopVar** mixes both the
[ASTMatcher](https://clang.llvm.org/docs/LibASTMatchersTutorial.html) and
[RecursiveASTVisitor](https://clang.llvm.org/docs/RAVFrontendAction.html)
frameworks. It is an example of how to leverage both of them to solve a
slightly more complex problem. The generated warnings are labelled so that you
can see which framework was used to capture a particular case of an unused
for-loop variable. For example, for the first example above you will get the
following warning:

```bash
warning: (Recursive AST Visitor) regular for-loop variable not used
```

The second example leads to the following warning:

```bash
warning: (AST Matcher) range for-loop variable not used
```

Reading the [source
code](https://github.com/banach-space/clang-tutor/blob/main/lib/UnusedForLoopVar.cpp)
should help you understand why different frameworks are needed in different
cases. I have also added a few test files that you can use as reference
examples (e.g.
[UnusedForLoopVar_regular_loop.cpp](https://github.com/banach-space/clang-tutor/blob/main/test/UnusedForLoopVar_regular_loop.cpp)).

### Run the plugin
```bash
$Clang_DIR/bin/clang -cc1 -fcolor-diagnostics -load <build_dir>/lib/libUnusedForLoopVar.dylib -plugin UFLV input.cpp
```

## CodeRefactor
This plugin will rename a specified member method in a class (or a struct) and
in all classes derived from it. It will also update all call sites in which the
method is used so that the code remains semantically correct.

The following example contains all cases supported by **CodeRefactor**.

```cpp
// file.cpp
struct Base {
    virtual void foo() {};
};

struct Derived: public Base {
    void foo() override {};
};

void StaticDispatch() {
  Base B;
  Derived D;

  B.foo();
  D.foo();
}

void DynamicDispatch() {
  Base *B = new Base();
  Derived *D = new Derived();

  B->foo();
  D->foo();
}
```

We will use **CodeRefactor** to rename `Base::foo` as `Base::bar`. Note that
this consists of two steps:

* update the declaration and the definition of `foo` in the base class (i.e.
  `Base`) as well as all in the derived classes (i.e. `Derived`)
* update all call sites the use static dispatch (e.g. `B1.foo()`) and dynamic
  dispatch (e.g. `B2->foo()`).

**CodeRefactor** will do all this refactoring for you! See
[below](#run-the-plugin-4) how to run it.

The
[implementation](https://github.com/banach-space/clang-tutor/blob/main/lib/CodeRefactor.cpp)
of **CodeRefactor** is rather straightforward, but it can only operate on one
file at a time.
[**clang-rename**](https://clang.llvm.org/extra/clang-rename.html) is much more
powerful in this respect.

### Run the plugin
**CodeRefactor** requires 3 command line arguments: `-class-name`, `-old-name`,
`-new-name`. Hopefully these are self-explanatory. Passing the arguments to the
plugin is  _a bit_ cumbersome and probably best demonstrated with an example:

```bash
$Clang_DIR/bin/clang -cc1 -load <build_dir>/lib/libCodeRefactor.dylib -plugin CodeRefactor -plugin-arg-CodeRefactor -class-name -plugin-arg-CodeRefactor Base  -plugin-arg-CodeRefactor -old-name -plugin-arg-CodeRefactor foo  -plugin-arg-CodeRefactor -new-name -plugin-arg-CodeRefactor bar file.cpp
```

It is much easier when you the plugin through a stand-alone tool like
`ct-code-refactor`!

### Run the plugin through `ct-code-refactor`
`ct-code-refactor` is a standalone tool that is basically a wrapper for
**CodeRefactor**. You can use it to refactor your input file as follows:

```bash
<build_dir>/bin/ct-code-refactor --class-name=Base --new-name=bar --old-name=foo file.cpp  --
```

`ct-code-refactor` uses LLVM's [CommandLine
2.0](https://llvm.org/docs/CommandLine.html) library for parsing command line
arguments. It is very well documented, relatively easy to integrate and the end
result is a very intuitive interface.

References
==========
Below is a list of clang resources available outside the official online
documentation that I have found very helpful.

* **Resources inside Clang**
  * Refactoring tool template:
    [clang-tools-extra/tool-template](https://github.com/llvm/llvm-project/tree/release/14.x/clang-tools-extra/tool-template)
  * AST Matcher [Reference](https://clang.llvm.org/docs/LibASTMatchersReference.html)
* **Clang Tool Development**
  * _"How to build a C++ processing tool using the Clang libraries"_, Peter Smith, Linaro Connect 2018
  ([video](https://www.youtube.com/watch?reload=9&v=8QvLVEaxzC8), [slides](https://s3.amazonaws.com/connect.linaro.org/yvr18/presentations/yvr18-223.pdf))
* **Diagnostics**
  * _"Emitting Diagnostics in Clang"_, Peter Goldsborough ([blog
  post](http://www.goldsborough.me/c++/clang/llvm/tools/2017/02/24/00-00-06-emitting_diagnostics_and_fixithints_in_clang_tools/))
* **Projects That Use Clang Plugins**
  * Mozilla: official documentation on [static analysis in Firefox](https://firefox-source-docs.mozilla.org/code-quality/static-analysis.html#build-time-static-analysis), custom [ASTMatchers](https://searchfox.org/mozilla-central/source/build/clang-plugin/CustomMatchers.h)
  * Chromium: official documentation on [using clang plugins](https://chromium.googlesource.com/chromium/src.git/+/master/docs/clang.md#using-plugins), in-tree [source code](https://chromium.googlesource.com/chromium/src/+/master/tools/clang/plugins/)
  * LibreOffice: official documentation on [developing Clang plugins](https://wiki.documentfoundation.org/Development/Clang_plugins), in-tree [source code](https://github.com/LibreOffice/core/tree/master/compilerplugins/clang)
* **clang-query**
  * _"Exploring Clang Tooling Part 2: Examining the Clang AST with clang-query"_, Stephen Kelly, [blog post](https://devblogs.microsoft.com/cppblog/exploring-clang-tooling-part-2-examining-the-clang-ast-with-clang-query/)
  * _"The Future of AST-Matcher based Refactoring"_, Stephen Kelly, [video 1](https://www.youtube.com/watch?v=yqi8U8Q0h2g&t=1202s), [video 2](https://www.youtube.com/watch?v=38tYYrnfNrs), [blog post](https://steveire.wordpress.com/2019/04/30/the-future-of-ast-matching-refactoring-tools-eurollvm-and-accu/)
  * Compiler Explorer [view](https://godbolt.org/z/clang-query) with clang-query

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
