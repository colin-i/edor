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
On Fedora:
```sh
sudo dnf copr enable colin/project
sudo dnf install edor
```
\
On openSUSE, run the following as __root__:\
For openSUSE Tumbleweed:
```sh
zypper addrepo https://download.opensuse.org/repositories/home:costin/openSUSE_Tumbleweed/home:costin.repo
```
For openSUSE Leap:
```sh
zypper addrepo https://download.opensuse.org/repositories/home:costin/openSUSE_Leap_15.6/home:costin.repo
```
And:
```sh
zypper refresh
zypper install edor
```
\
On Windows, there is this **edor-\*-msys-\*.zst** pkg at [releases](https://github.com/colin-i/edor/releases) which is using [msys2](https://msys2.github.io).
\
\
On other linux distributions, <i>.AppImage</i> file from [releases](https://github.com/colin-i/edor/releases).

## From source
Debian example: Install autoconf automake make libncurses-dev bc g++(or gcc and --disable-cpp at configure); on armv7l(alias armhf) cpu, libunwind-dev. There is a "headless" build file as well.
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
