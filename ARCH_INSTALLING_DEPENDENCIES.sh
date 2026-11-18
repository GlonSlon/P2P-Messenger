#!/bin/bash
set -e

sudo pacman -Syu --noconfirm
sudo pacman -S --needed --noconfirm base-devel cmake pkgconf \
    git patchelf fuse2 zlib \
    openssl jsoncpp qt6-base opendht

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