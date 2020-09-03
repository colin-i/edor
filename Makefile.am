SUBDIRS = s

setinfofilecommand:
	if [ ! -e "`cat ./bash_home`/.@PROJ@info" ]; then cp "./.@PROJ@info" "`cat ./bash_home`/" --preserve=ownership; fi

install: install-am setinfofilecommand

reminfofilecommand:
	if [ -e "`cat ./bash_home`/.@PROJ@info" ]; then rm "`cat ./bash_home`/.@PROJ@info"; fi

uninstall: uninstall-am reminfofilecommand
