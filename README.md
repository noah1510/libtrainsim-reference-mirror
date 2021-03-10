# Simulator

This repository contains the Bahn Simulator application based on [libtrainsim](https://git.thm.de/bahn-simulator/libtrainsim).
If you want the data you need to download the subproject.

If you simply want to test this out you can download the simulator from the snap store (linux only).
Once downloaded you can open the simulator from your application menu.

If the program crashes immediatly you have to options to get it working.
First off you can edit the desktop launch file and try manually setting the working directory to `/snap/libtrainsim/current/usr/local/share/libtrainsim/`.
The other solution is to open a terminal and type `cd /snap/libtrainsim/current/usr/local/share/libtrainsim/`.
Now you can just enter `libtrainsim` into the terminal and the simulator should open.

[![Get it from the Snap Store](https://snapcraft.io/static/images/badges/en/snap-store-black.svg)](https://snapcraft.io/libtrainsim)

## Building the code

Clone the repository using `git clone https://git.thm.de/bahn-simulator/simulator.git`.
You can simply build the code using meson with any build system that supports meson.
Below are the instructions for kdevelop and codium.

## windows

Windows is not actively tested but everything should compile under windows.
If there are problems feel free to open a new issue just make sure to provide as much details as possible.
If it is a problem with libtrainsim instead of the simulator the issue will be moved there.

## dependencies

The following dependencies are needed to compile the code.
On Windows these can be installed with [choco](https://chocolatey.org/) and on Linux you can use your package manager.
You don't need opencv if you have ffmpeg. Opencv provides an additional video backend which has bad performance (if both are installed ffmpeg is used by default).

* [meson](https://mesonbuild.com/) (version > 0.54.0)
* [ffmpeg](https://www.ffmpeg.org/download.html) you need the development headers for the libraries
* some C++ compiler (Gcc or llvm/clang) (needs support for `<filesystem>` C++17 header; GCC > 9.2.0, Clang > 10.0.0)
* [opencv](https://opencv.org/releases/) (version > 2.4.0) (optinal instead of ffmpeg)

## development environment

### [kdevelop](https://www.kdevelop.org/)

Personally I use kdevelop to write the simulator and libtrainsim.
It should come preinstalled with the meson plugin.

#### compiling and running the code

After you have cloned the repository, open kdevelop and select `Project` > `Open / Import Project` and select the meson.build file.
Click next and make sure the project manager is `meson project manager`.
Click on build to test if you have all of the dependencies.

The last step is to set up a launch command, so that you only have to click on the play button to build and launch the simulator.
Select `Run` > `Configure Launches...`.
In the new window select your project and click on `+ Add` > `Compiled Binary`.
Select the project target (there should only be one option) and select the project folder as working directory (by default the build directory is the working dir but we don't want that).
The last step is to add the build of the binary as a dependency in the launch configuration.
Select `Build` as Action and click on the folder symbol.
Select the project and click `Ok`.
You can now close the configuration window.

Now you can build and launch the simulator by clicking on execute or debug.

### [vs codium](https://vscodium.com/)

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

#### compiling and running the code

Just open the folder using codium after you cloned the repository.
If you set up the development environment, you can simply go to the run tab and run `lldb debug` or `gdb debug`.

At the moment the configuration is only tested on Linux (kubuntu 20.10 amd64) however it is written in a way that should work for all platforms (except OSX where I don't know how to configure it, should be possible to make work).
