#!/bin/bash
set -e

sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    pkg-config \
    git \
    libncurses5-dev \
    libreadline-dev \
    nettle-dev \
    libgnutls28-dev \
    libargon2-dev \
    libmsgpack-dev \
    libssl-dev \
    libfmt-dev \
    libjsoncpp-dev \
    libhttp-parser-dev \
    libasio-dev \
    libyaml-cpp-dev \
    libunistring-dev \
    libcppunit-dev

git clone https://github.com/savoirfairelinux/dhtnet.git
cd dhtnet
git submodule update --init --recursive

mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLE=ON
make -j$(nproc)
sudo make install
sudo ldconfig