#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if ! command -v clang &> /dev/null
then
    echo "! clang could not be found."
    echo "! Please install clang before running this script."
    echo "! From apt: sudo apt install clang"
    exit
fi

if [ -d "${DIR}/evsets" ] ; then
  echo "! ${DIR}/evsets already exists."
  echo "! Remove it if desiring re-installation."
  exit
fi

git clone https://github.com/cgvwzq/evsets.git evsets
cd evsets
git checkout 49d4b25ddcd32e00349024a7e3d16cf0448b1f8a
make
