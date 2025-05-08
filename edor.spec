%global srcname edor

Name: edor
Version: 1.71
Release: 0
License: GPLv3
Summary: Console user interface text editor
Url: https://github.com/colin-i/%{srcname}
Source0: %{name}-%{version}.tar.gz

BuildArch: x86_64 aarch64

BuildRequires: autoconf automake make bc gcc-c++ ncurses-devel bash

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
* Thu May 08 2025 Costin Botescu <costin.botescu@gmail.com> 1.71-0
- new package built with tito

