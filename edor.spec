
Name: edor
Version: 1.88
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
* Thu Feb 19 2026 costin <costin.botescu@gmail.com> 1.88-0
- guides (costin.botescu@gmail.com)
- one less comparison at mouse (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- Press instead of Click (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)

* Sun Jan 25 2026 costin <costin.botescu@gmail.com> 1.87-0
- split format with what was left: change,help (costin.botescu@gmail.com)
- write .split file (costin.botescu@gmail.com)
- .split at overall (costin.botescu@gmail.com)
- will make room for a new key that will work with actionswf as3
  (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- and at help (costin.botescu@gmail.com)
- ignore blanks (costin.botescu@gmail.com)
- first part implementation of ctrl alt j (costin.botescu@gmail.com)

* Sat Jan 17 2026 costin <costin.botescu@gmail.com> 1.86-0
- 

* Wed Jan 07 2026 costin <costin.botescu@gmail.com> 1.85-0
- fix a warning (costin.botescu@gmail.com)
- fix for new makepkg (costin.botescu@gmail.com)
- that variable is no longer passed to makepkg (costin.botescu@gmail.com)

* Fri Dec 26 2025 costin <costin.botescu@gmail.com> 1.84-0
- fix com_nr confusion (costin.botescu@gmail.com)

* Mon Nov 03 2025 Costin Botescu <costin.botescu@gmail.com> 1.83-0
- "sync" (costin.b.84@gmail.com)
- display nr of rows. timeout option (costin.b.84@gmail.com)
- not depending on for(;;) (costin.botescu@gmail.com)
- default timeout (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- save timeout option (costin.botescu@gmail.com)
- extra wnoutrefresh. goto fix. (costin.botescu@gmail.com)
- number of rows at start (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)

* Tue Oct 21 2025 Costin Botescu <costin.botescu@gmail.com> 1.81-0
- "sync" (costin.b.84@gmail.com)
- cast (costin.botescu@gmail.com)
- calculate replace number counter (costin.b.84@gmail.com)
- "up" (costin.botescu@gmail.com)
- calculate replace number counter once (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- fix finds counter when replacing a text and the old text is included one or
  multiple times in the new text (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)

* Wed Sep 24 2025 Costin Botescu <costin.botescu@gmail.com> 1.79-0
- indent option to not follow (only positive, negative is not elegant)
  (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- readme (costin.botescu@gmail.com)
- jq name (costin.botescu@gmail.com)

* Fri Jul 11 2025 Costin Botescu <costin.botescu@gmail.com> 1.78-0
- aftercall_aux  tabs at aftercall  and  init_aftercall
  (costin.botescu@gmail.com)
- win auto (costin.botescu@gmail.com)
- appimage (costin.botescu@gmail.com)
- name (costin.botescu@gmail.com)
- escapes (costin.botescu@gmail.com)
- new appimage AppRun (costin.botescu@gmail.com)
- new appimage runtime (costin.botescu@gmail.com)
- test.yml (costin.botescu@gmail.com)

* Thu Jul 03 2025 Costin Botescu <costin.botescu@gmail.com> 1.77-0
- ENTRY_DEBUG not required anymore (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- --file argument (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- some win tests, and since stat, also at arm7 (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- same as in configure (costin.botescu@gmail.com)
- "sync" (costin.b.84@gmail.com)

* Sun May 25 2025 Costin Botescu <costin.botescu@gmail.com> 1.76-0
- memmem on older systems (costin.botescu@gmail.com)

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

