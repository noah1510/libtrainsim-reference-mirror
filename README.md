# Simulator

This repository contains the Bahn Simulator application and uses the Video engine to find what data should be displayed.

## Building the code

Clone the repository using `git clone https://git.thm.de/bahn-simulator/simulator.git`and open that folder in vs code/codium.
If you set up the development environment, you can simply go to the run tab and run `lldb debug` or `gdb debug`.

## windows

Windows is not actively tested but everything should compile under windows if you use msys2 as terminal.

## dependencies

The following dependencies are needed to compile the code.
On Windows these can be installed with [choco](https://chocolatey.org/) and on Linux you can use your package manager.

* [meson](https://mesonbuild.com/) (version > 0.54.0)
* [opencv](https://opencv.org/releases/) (version > 2.4.0)
* some C++ compiler (Gcc or llvm/clang) (needs support for `<filesystem>` C++17 header; GCC > 9.2.0, Clang > 10.0.0)

## development environment

The used IDE is visual studio code or rather [vs codium](https://vscodium.com/).
In addition to that the dependencies have to be installed.

The following plugins are used for the configurations to work properly:

Documentaion generation and linting

* bbenoist.doxygen
* cschlosser.doxdocgen
* davidanson.vscode-markdownlint

Code formatting and linting:

* notskm.clang-tidy
* llvm-vs-code-extensions.vscode-clangd

Execution and setup:

* vadimcn.vscode-lldb
* webfreak.debug
* asabil.meson

At the moment the configuration is only tested on Linux (kubuntu 20.10 amd64) however it is written in a way that should work for all platforms (except OSX where I don't know how to configure it, should be possible to make work).
