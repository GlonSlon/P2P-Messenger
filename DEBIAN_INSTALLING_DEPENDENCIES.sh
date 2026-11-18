#!/bin/bash
set -e

sudo apt update
sudo apt install -y build-essential cmake pkg-config git \
    patchelf libfuse2 zlib1g-dev libssl-dev libjsoncpp-dev \
    qt6-base-dev libopendht-dev

if [ -d "appimage" ]; then
    cd appimage
fi

if [ ! -f "linuxdeploy-x86_64.AppImage" ]; then
    wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
fi

if [ ! -f "linuxdeploy-plugin-qt-x86_64.AppImage" ]; then
    wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
fi

chmod +x *.AppImage

echo "DEPENDENCIES: OK"