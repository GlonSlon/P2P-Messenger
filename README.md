<div align="center">
  
![P2P Messenger](https://github.com/GlonSlon/P2P-Messenger/blob/main/appimage/p2p-messenger.png)

</div>

# P2P Messenger

P2P Messenger is a decentralized peer-to-peer messenger for Linux distributions. Peer discovery and data transfer are performed directly through a global distributed hash table (DHT) network.

## Features

- **Full decentralization:** No servers, databases, or single point of failure.
- **Private rooms:** A communication channel is created individually for each pair of users based on their usernames.
- **Data encryption:** All payload, including text and files, is protected with AES-256. The encryption key is generated randomly at build time. Each build is unique and cannot connect to clients from earlier or later builds.
- **Self-cleaning:** Messages are stored in the DHT for approximately 1.5 hours and then automatically deleted.
- **File transfer:** File sending is supported with pre-compression via Zlib.

## Technology Stack

- **Language:** C++17
- **GUI:** Qt6 (Widgets)
- **Networking:** OpenDHT (Jami library)
- **Cryptography:** OpenSSL (AES-256-CBC)
- **Compression:** Zlib
- **Build system:** CMake

## Dependency Installation

### Debian / Ubuntu / Kali Linux

```bash
chmod +x DEBIAN_INSTALLING_DEPENDENCIES.sh
./DEBIAN_INSTALLING_DEPENDENCIES.sh
```

### Arch Linux / Manjaro

```bash
chmod +x ARCH_INSTALLING_DEPENDENCIES.sh
./ARCH_INSTALLING_DEPENDENCIES.sh
```

## Build and Packaging

```bash
cd P2P-Messenger
chmod +x START.sh
./START.sh
```

## Usage

1. Run `p2p-messenger-x86_64.AppImage`.
2. Enter the desired **username**.
3. Enter the peer's **username** in the search field and click the search button.
4. Select the found user in the list on the left.
5. The second peer must perform the same actions, since initiation must be mutual.
6. The chat is ready.

**Note:** Both users must use clients built from the same source code to be able to read each other's messages.

## DHTNet Installation Issues

If DHTNet installation fails, build it from source:

```bash
DHTNET_INSTALLER.sh
```
