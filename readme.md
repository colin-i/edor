# EDitOR

## Install
On Ubuntu(amd64 arm64/armhf)(focal jammy noble) from PPA.
```sh
sudo add-apt-repository ppa:colin-i/ppa
```
Or the *manual installation step* from this link *https://gist.github.com/colin-i/e324e85e0438ed71219673fbcc661da6* \
Install:
```sh
sudo apt-get install edor
```
\
On Arch Linux:
```sh
yay -Sy edor
```
Or <i>.zst</i>(x86_64) or <i>.xz</i>(aarch64) file from [releases](https://github.com/colin-i/edor/releases).
\
\
On Fedora 42(x86_64 aarch64):
```sh
sudo dnf copr enable colin/project
sudo dnf install edor
```
\
On openSUSE, run the following as __root__:\
For openSUSE Tumbleweed(x86_64/i586 aarch64):
```sh
zypper addrepo https://download.opensuse.org/repositories/home:costin/openSUSE_Tumbleweed/home:costin.repo
```
For openSUSE Leap:
```sh
zypper addrepo https://download.opensuse.org/repositories/home:costin/openSUSE_Leap_16.0/home:costin.repo
```
Replace *16.0*(x86_64 aarch64) with *15.6*(x86_64/i586 aarch64) if needed.\
And:
```sh
zypper refresh
zypper install edor
```
\
On Windows, there is this **edor-\*-msys-\*.zst** pkg at [releases](https://github.com/colin-i/edor/releases) which is using [msys2](https://msys2.github.io).
\
\
On linux distributions(x86_64), <i>.AppImage</i> file from [releases](https://github.com/colin-i/edor/releases).

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
