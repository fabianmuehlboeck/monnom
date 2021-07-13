#!/bin/bash
sudo apt-get install -y build-essential git libgc-dev libpugixml-dev
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 12
rm llvm.sh

wget https://packages.microsoft.com/config/ubuntu/20.04/packages-microsoft-prod.deb -O packages-microsoft-prod.deb
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

sudo raco pkg install grift
