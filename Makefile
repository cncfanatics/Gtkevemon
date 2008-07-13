INSTALL_BIN = /usr/local/bin

all:
	make -C src

clean:
	make -C src clean

install:
	mkdir -p ${INSTALL_BIN}
	cp src/gtkevemon ${INSTALL_BIN}

uninstall:
	rm -f ${INSTALL_BIN}/gtkevemon
	rmdir --ignore-fail-on-non-empty ${INSTALL_BIN}
