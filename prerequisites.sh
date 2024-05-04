#!/bin/bash
SOURCE="$( realpath "${BASH_SOURCE[0]}" )"
DIRNAME="$( dirname "$SOURCE" )"
sudo apt-get update
sudo apt-get install -y build-essential git libgc-dev libpugixml-dev
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 12
rm llvm.sh

wget https://packages.microsoft.com/config/ubuntu/22.04/packages-microsoft-prod.deb -O packages-microsoft-prod.deb
sudo dpkg -i packages-microsoft-prod.deb
rm packages-microsoft-prod.deb

sudo apt-get update; \
  sudo apt-get install -y apt-transport-https && \
  sudo apt-get update && \
  sudo apt-get install -y dotnet-sdk-3.1

wget https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB -O - | sudo apt-key add -
sudo add-apt-repository -y "deb https://apt.repos.intel.com/oneapi all main"
sudo apt-get update

sudo apt-get install -y intel-oneapi-vtune intel-oneapi-tbb-devel-2021.3.0

sudo add-apt-repository -y ppa:plt/racket
sudo apt-get update

sudo apt-get install -y racket

raco pkg install grift

sudo apt-get install -y mono-runtime

sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang++-12 60 #alternatively, ensure some modern version of clang is installed

cd "$DIRNAME/experiments/racket"
raco link benchmark-util

sudo apt-get install -y python3 python3-pip
pip3 install -U plotly
pip3 install -U pandas
pip3 install -U kaleido

sudo apt-get install default-jdk

sudo apt-get install pypy3

sudo apt-get install nodejs
