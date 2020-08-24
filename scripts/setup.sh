#!/bin/bash

THREADS="3"
CI="0"
USEPACKAGE="0"
INSTALLDEPS="0"
#parse parameter
pars=$#
for ((i=1;i<=pars;i+=1))
do
  case "$1" in
    "-j")
      shift
      if [ $1 ]
      then
        THREADS="$(($1+1))"
      fi
      shift
      i+=1
      ;;
    "--ci")
      CI="1"
      shift
      ;;
    "--init")
      INSTALLDEPS="1"
      shift
      ;;
    "--help")
      echo "Usage: scripts/build.sh [options]"
      echo "Options:"
      echo "    --help                              Display this information"
      echo "    --ci                                Run in CI mode"
      echo "    --init                              Install all of the dependencies"
      echo "    -j [threadnumber]                   Build the project with the specified number of threads."
      echo ""
      echo "view the source on https://git.thm.de/bahn-simulator/simulator"
      exit 1
      ;;
    *)
      echo "Invalid option try --help for information on how to use the script"
      exit 1
      ;;
  esac
done

if [ "$OSTYPE" == "linux-gnu" ]
then
    # Linux
    echo "script running on linux"

elif [ "$OSTYPE" == "darwin"* ]
then
    # Mac OSX
    echo "script running on mac osx"
        
elif [[ "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]
then
    # Lightweight shell and GNU utilities compiled for Windows (part of MinGW)
    # or 
    # POSIX compatibility layer and Linux environment emulation for Windows
    echo "script running on windows"

elif [  "$OSTYPE" == "msys" ]
then
    echo "script running on windows"

else
    # Unknown os
    echo "running on something else."
    echo "Not a supported OS: $OSTYPE" >&2
    exit 1
fi

if [ "$CI" == "0" ]
then
    echo "cloning the data"
    git submodule update --init data
    if [ $? -eq 0 ]
    then
        echo "Successfully initiated data"
    else
        echo "Could not initiate data" >&2
        exit 1
    fi
fi

rm -rf build
meson setup build
if [ $? -eq 0 ]
then
    echo "Successfully initiated build directory"
else
    echo "Could not initiate build directory" >&2
    exit 1
fi

exit 0
