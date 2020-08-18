# Simulator

This repository contains the Bahn Simulator application and uses the Video engine to find what data should be displayed.

## Building the code

Before the code can be built, the repository has to be setup.

First clone the repository using `git clone https://git.thm.de/bahn-simulator/simulator.git`.
Then open a terminal in the simulator folder.
After that execute `./scripts/setup.sh --init` to download the dependencies and setup the project.
Now you can build the code by executing `./scripts/build.sh`.

For a clean build you can run the setup scripts again (without the `--init`) and then call `./scripts/build.sh` again.

## Using packaged libredhand

If you want to the packaged version of libredhand use the `--redhand-package` option for the setup script.

You can either install the latest package which will have all the new features or the stable package which might be a bit older but might contain less bugs.

## Other script options

To see all the options the scripts have just use the `--help` option, which exists for both the build and setup.

## windows

Windows is not actively tested but everything should compile under windows if you use msys2 as terminal.

## development environment

The used IDE is visual studio code or rather [vs codium](https://vscodium.com/).

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
