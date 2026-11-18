#!/bin/bash
set -e

APP_NAME="p2p-messenger"
BUILD_DIR="../build"
APPDIR="AppDir"

rm -rf "$APPDIR"
mkdir -p "$APPDIR/usr/bin"
mkdir -p "$APPDIR/usr/share/applications"
mkdir -p "$APPDIR/usr/share/icons/hicolor/256x256/apps"

cp "$BUILD_DIR/$APP_NAME" "$APPDIR/usr/bin/"

cat > "$APPDIR/usr/share/applications/$APP_NAME.desktop" <<EOF
[Desktop Entry]
Name=p2p-messenger
Exec=$APP_NAME
Icon=p2p-messenger.png
Type=Application
Categories=Network;InstantMessaging;
EOF

cp "p2p-messenger.png" "$APPDIR/usr/share/icons/hicolor/256x256/apps/p2p-messenger.png"

export QMAKE=$(which qmake6)
if [ ! -f "$QMAKE" ]; then
    echo "Error: qmake6 not found. Install: qt6-base-dev."
    exit 1
fi


./linuxdeploy-x86_64.AppImage \
    --appdir "$APPDIR" \
    --executable "$APPDIR/usr/bin/$APP_NAME" \
    --desktop-file "$APPDIR/usr/share/applications/$APP_NAME.desktop" \
    --icon-file "$APPDIR/usr/share/icons/hicolor/256x256/apps/p2p-messenger.png" \
    --plugin qt \
    --output appimage

echo "AppImage BUILD: TRUE"
