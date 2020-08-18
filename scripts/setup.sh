#!/bin/bash

THREADS="3"
CI="0"
BUILDREDHAND="1"
REDHANDSTABLE="0"
USEPACKAGE="0"
BUILDVIDEO="1"
INSTALLDEPS="0"
REDHAND_BUILD_OPTIONS=""
REDHAND_SETUP_OPTIONS=""

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
    "--redhand-build-options")
      shift
      if [ $1 ]
      then
        REDHAND_BUILD_OPTIONS+="$1"
      fi
      shift
      i+=1
      ;;
    "--redhand-setup-options")
      shift
      if [ $1 ]
      then
        REDHAND_SETUP_OPTIONS+="$1"
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
    "--redhand-package")
      BUILDREDHAND="0"
      shift
      ;;
    "--redhand-stable")
      REDHANDSTABLE="1"
      BUILDREDHAND="0"
      USEPACKAGE="1"
      shift
      ;;
    "--redhand-latest")
      REDHANDSTABLE="0"
      BUILDREDHAND="0"
      USEPACKAGE="1"
      shift
      ;;
    "--help")
      echo "Usage: scripts/build.sh [options]"
      echo "Options:"
      echo "    --help                              Display this information"
      echo "    --ci                                Run in CI mode"
      echo "    --init                              Install all of the dependencies"
      echo "    --redhand-package                   (Ubuntu 20.04 or newer only) Build redhand from package instead of using the source."
      echo "    --redhand-stable                    (Ubuntu 20.04 or newer only) Use the redhand-dev package from the stable repository."
      echo "    --redhand-latest                    (Ubuntu 20.04 or newer only) Use the redhand-dev package from the latest repository. (default)"
      echo "    -j [threadnumber]                   Build the project with the specified number of threads."
      echo "    --redhand-build-options [options]   Build the redhand with the specified options."
      echo "    --redhand-setup-options [options]   Setup the redhand with the specified options."
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

find_redhand_package(){
    if [ "$(lsb_release -s -i)" == "Ubuntu" ]
    then
        VERSION="$(lsb_release -s -c)"
        supported="0"
        case $VERSION in
            "focal")
                supported="1"
                ;;
            "groovy")
                supported="1"
                ;;
            *)
                ;;
        esac
            
        if [ "$supported" == "1" ]
        then
            if [ -n "$(sudo apt-get install libredhand-dev 2>&1 >/dev/null | grep -c "Unable to locate package libredhand-dev")" ]
            then
                if [ "$USEPACKAGE" == "1" ]
                then
                    if [ "$REDHANDSTABLE" == "1" ]
                    then
                        sudo add-apt-repository --yes ppa:noasakurajin/libredhand
                    else
                        sudo add-apt-repository --yes ppa:noasakurajin/libredhand-latest
                    fi
                    
                    sudo apt-get update
                    sudo apt-get install --yes libredhand-dev
                else
                    echo "libredhand package could not be installed do you want to add the repository? (y/n)(default n):"
                    ADDREPO="n"
                    read ADDREPO
                    
                    if [ "$ADDREPO" == "y" ]
                    then
                        echo "Do you want to use the stable instead of the latest packages (y for stable, anything for latest)(default n):"
                        STABLE="n"
                        read STABLE
                        
                        if [ "$STABLE" == "y" ]
                        then
                            echo "adding stable repository"
                            sudo add-apt-repository --yes ppa:noasakurajin/libredhand
                        else
                            echo "adding latest repository"
                            sudo add-apt-repository --yes ppa:noasakurajin/libredhand-latest
                        fi
                        
                        sudo apt-get update
                        sudo apt-get install --yes libredhand-dev
                        
                        return
                    elif [ "$ADDREPO" == "n" ]
                    then
                        echo "not adding repository, building redhand from source"
                        BUILDREDHAND="1"
                        return
                    else
                        echo "invalid input, building redhand from source"
                        BUILDREDHAND="1"
                        return
                    fi
                fi
                
            else
                echo "libredhand was sucessfully installed"
                return
            fi
            
        fi
        
    fi
    
    echo "This linux distro or version of this distro does not support redhand packages."
    echo "Building redhand from source"
    BUILDREDHAND="1"
}

if [ "$OSTYPE" == "linux-gnu" ]
then
    # Linux
    echo "script running on linux"

    if [ "$BUILDREDHAND" == "0" ]
    then
        find_redhand_package
    fi
    
    REDHANDLIB="libredhand.so"

elif [ "$OSTYPE" == "darwin"* ]
then
    # Mac OSX
    echo "script running on mac osx"

    BUILDREDHAND="1"
    REDHANDLIB="libredhand.so"

        
elif [[ "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]
then
    # Lightweight shell and GNU utilities compiled for Windows (part of MinGW)
    # or 
    # POSIX compatibility layer and Linux environment emulation for Windows
    echo "script running on windows"

    BUILDREDHAND="1"
    REDHANDLIB="libredhand.dll"

elif [  "$OSTYPE" == "msys" ]
then
    echo "script running on windows"
    
    BUILDREDHAND="1"
    REDHANDLIB="libredhand.dll"
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

if [ "BUILDVIDEO" == "1" ]
then
    echo "cloning video engine"
    git submodule update --init --recursive subprojects/video-engine
    if [ $? -eq 0 ]
    then
        echo "Successfully initiated video engine"
    else
        echo "Could not initiate video engine" >&2
        exit 1
    fi
fi

if [ "$BUILDREDHAND" == "1" ]
then
    echo "cloning the redhand repository"
    git submodule update --init subprojects/redhand
    if [ $? -eq 0 ]
    then
        echo "Successfully initiated redhand"
    else
        echo "Could not initiate redhand" >&2
        exit 1
    fi
    
    
    cd "subprojects/redhand"
    
    if [ "$INSTALLDEPS" == "1" ]
    then
        echo "installing redhand dependencies"
        bash ./scripts/dependencies.sh
    fi
    
    cd ../..
    
fi

exit 0
