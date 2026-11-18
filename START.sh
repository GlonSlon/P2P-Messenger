#!/bin/bash
set -e

echo "TAG: BUILD"
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
echo "TAG: BUILD - OK"


echo "TAG: BUILD APPIMAGE"
cd ..
cd appimage
chmod +x *.AppImage
./build_appimage.sh
echo "TAG: BUILD APPIMAGE - OK"


echo "TAG: COPY APPIMAGE"
cp p2p-messenger*.AppImage $HOME/ || cp *.AppImage $HOME/
echo "TAG: COPY APPIMAGE - OK"

