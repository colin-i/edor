
Name: edor
Version: 1.75
Release: 0
License: GPLv3
Summary: Console user interface text editor
Url: https://github.com/colin-i/%{name}
Source0: %{name}-%{version}.tar.gz

BuildArch: x86_64 aarch64

BuildRequires: autoconf automake make bc gcc-c++ ncurses-devel

Requires: ncurses

%description
Console user interface text editor using ncurses.

#-- PREP, BUILD & INSTALL -----------------------------------------------------#
%prep
%autosetup

%build
autoreconf -i
%configure
make

%install
%make_install

#-- FILES ---------------------------------------------------------------------#
%files
%attr(0755, root, root) "%{_bindir}/%{name}"

#-- CHANGELOG -----------------------------------------------------------------#
%changelog
* Sun May 25 2025 Costin Botescu <costin.botescu@gmail.com> 1.75-0
- 

* Sun May 25 2025 Costin Botescu <costin.botescu@gmail.com> 1.74-0
- KEY_BACKSPACE (costin.botescu@gmail.com)
- win readme (costin.botescu@gmail.com)
- msys instead of 0 at subrelease (costin.botescu@gmail.com)
- windows msys build (costin.botescu@gmail.com)
- warnings (costin.botescu@gmail.com)
- erroring (costin.botescu@gmail.com)
- memmem (costin.botescu@gmail.com)
- memmem from configure (costin.botescu@gmail.com)
- is_dir  open write  realpath memmem (costin.botescu@gmail.com)
- poll nc_getmouse mkdir(char*) (costin.botescu@gmail.com)
- cross compile considerations (costin.botescu@gmail.com)
- jpg (costin.botescu@gmail.com)
- "sync" (costin.b.84@gmail.com)
- "sync" (costin.b.84@gmail.com)
- "sync" (costin.b.84@gmail.com)
- tumbleweed and leap (costin.botescu@gmail.com)
- "tests" (costin.b.84@gmail.com)
- remote tags (costin.botescu@gmail.com)
- update (costin.botescu@gmail.com)

* Fri May 23 2025 Costin Botescu <costin.botescu@gmail.com> 1.73-0
- alt+q following ctrl+q (costin.botescu@gmail.com)
- suse pub (costin.botescu@gmail.com)
- "sync" (costin.b.84@gmail.com)
- fix pkgarm (costin.botescu@gmail.com)

* Sun May 18 2025 Costin Botescu <costin.b.84@gmail.com> 1.72-0
- alt+q and keys_help (costin.b.84@gmail.com)
- reason at main.sh (costin.botescu@gmail.com)
- Alt+q & keys_help (costin.botescu@gmail.com)
- factory (costin.botescu@gmail.com)
- openSUSE (costin.botescu@gmail.com)
- f1 last row (costin.botescu@gmail.com)
- only ncurses on suse, and test std=c23 (costin.botescu@gmail.com)
- "sync" (costin.b.84@gmail.com)
- "sync" (costin.b.84@gmail.com)
- options (costin.botescu@gmail.com)
- "sync" (costin.b.84@gmail.com)
- "sync" (costin.b.84@gmail.com)
- "sync" (costin.b.84@gmail.com)

* Thu May 08 2025 Costin Botescu <costin.botescu@gmail.com> 1.71-0
- new package built with tito

