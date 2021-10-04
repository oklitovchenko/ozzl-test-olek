# Multithread TCP server & client Test task realization

Work has been done according to task from: https://docs.google.com/document/d/1vc11Zw2sAOCEmwi2haaNoZ4C9UCkJTycTh7hgBnUUZM/edit.
Additionally added some improvements.

## Usage advice

To compile and run integration test:

```
sudo apt-get update
sudo apt-get install -y gcc build-essential qt5-default
git clone git@github.com:oklitovchenko/ozzl-test-olek.git
cd ozzl-test-olek
make && make test
make clean
```

Project contains Debian package specification. 
You can build .deb separate packages for server and client
from one source in standard way.

To install and use .deb binaries:

for server:
```
sudo dpkg -i ozzl-test-olek-server_0.1.0_amd64.deb
```

for client:
```
sudo dpkg -i ozzl-test-olek-client_0.1.0_amd64.deb
```


Server installs as a daemon and can be managed by systemd:
```
sudo systemctl sart ozzl-test-olek-server
sudo systemctl resart ozzl-test-olek-server
sudo systemctl stop ozzl-test-olek-server
```


Client supports command line arguments:
```
shell$ bin/ozzl-test-olek-client -h
Usage: bin/ozzl-test-olek-client [options]

Options:
  -h, --help                                 Displays this help.
  -c, --config-file <config file>            Client configuration <file>.
  -r, --request-value <request value>        Client request <value>.
  -v, --protocol-version <protocol version>  Client protocol <version> for TCP
                                             interaction.
  -o, --output-file <output file>            Client output <file>.
```

## Basic protocol explanation
```
All protocols musts support (for amd64):
 *
 *  0) first two bytes:
 *     0xAA 0xFF
 *
 *  1) for Request:
 *     0xAA 0xFF 0x00 0x00  0x00 0x00 0x00 0x00  0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
 *    [ protocol  version ][     reserved      ][        request (double value)         ]
 *
 *  2) for Response:
 *     0xAA 0xFF 0x00 0x00  0x00 0x00 0x00 0x00  0x00 0x00 0x00 0x00  0x00 ...............
 *    [ protocol  version ][       error       ][     body size     ][ body as byte stream ->
 *                  ^              ^                                    ^
 *                  |              |                                    |
 *                  |              |                             (LOG INFO) if (Error code) != NULL
 *                  |              |
 *                 NULL if (Error code) == ERR_PROTO_VER_MISMATCH
```