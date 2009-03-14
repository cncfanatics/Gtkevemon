INSTALL_DIR_ICON = /usr/share/pixmaps/
INSTALL_DIR_DESK = /usr/share/applications/

install:
	mkdir -p ${INSTALL_DIR_ICON}
	mkdir -p ${INSTALL_DIR_DESK}
	cp gtkevemon.xpm ${INSTALL_DIR_ICON}
	cp gtkevemon.desktop ${INSTALL_DIR_DESK}

uninstall:
	rm -f ${INSTALL_DIR_ICON}gtkevemon.xpm
	rm -f ${INSTALL_DIR_DESK}gtkevemon.desktop
	rmdir --ignore-fail-on-non-empty ${INSTALL_DIR_ICON}
	rmdir --ignore-fail-on-non-empty ${INSTALL_DIR_DESK}
