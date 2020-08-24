#!/bin/bash

find_compiler(){
    # get cross-distro version of POSIX command
    COMMAND=""
    if command -v command 2>/dev/null; then
        COMMAND="command -v"
    elif type type 2>/dev/null; then
        COMMAND="type"
    fi

    for comp in clang-12 clang-11 clang-10 clang-9 gcc-10 gcc-9 C:/Program Files/LLVM/bin/clang.exe; do
        if $COMMAND "$comp" 2>/dev/null; then
            export CC="$comp"
            break;
        fi
    done

    for comp in clang++-12 clang++-11 clang++-10 clang++-9 g++-10 g++-9 C:/Program Files/LLVM/bin/clang++.exe; do
        if $COMMAND "$comp" 2>/dev/null; then
            export CXX="$comp"
            break;
        fi
    done
}

THREADS="$(nproc)"
THREADS="$(($THREADS+1))"

find_compiler

meson setup build
meson compile -C build