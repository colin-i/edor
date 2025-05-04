# EDitOR

## Install
On Ubuntu from PPA.\
Architecture: amd64 arm64 armhf
```sh
sudo add-apt-repository ppa:colin-i/ppa
```
Or the *manual installation step* from this link *https://gist.github.com/colin-i/e324e85e0438ed71219673fbcc661da6* \
Install:
```sh
sudo apt-get install edor
```
\
On Arch Linux, <i>.zst</i> or <i>.xz</i> file from [releases](https://github.com/colin-i/edor/releases), depending on architecture. Or:
```sh
yay -Sy edor
```
\
On Fedora, <i>.rpm</i> file from [releases](https://github.com/colin-i/edor/releases).
```sh
dnf install edor-*.*.rpm
```
\
On linux distributions, <i>.AppImage</i> file from [releases](https://github.com/colin-i/edor/releases).

## From source
Debian example: Use autoconf automake make libncurses-dev bc ; on armv7l(alias armhf) cpu, libunwind-dev. There is a "headless" build file as well.
```sh
autoreconf -i
./configure
make
sudo make install
```
Use *\-\-prefix=your_path* at *./configure* if needed (example: at Termux in Android).\
*\-\-disable\-cpp* to set c rules.\
Uninstall command is *make uninstall*.

## Donations
The *donations* section is here
*https://gist.github.com/colin-i/e324e85e0438ed71219673fbcc661da6*
