INSTALL_BIN = /usr/local/bin

all:
	$(MAKE) -C src

multicore:
	$(MAKE) -C src multicore

clean:
	$(MAKE) -C src clean

install:
	mkdir -p ${INSTALL_BIN}
	cp src/gtkevemon ${INSTALL_BIN}

uninstall:
	rm -f ${INSTALL_BIN}/gtkevemon
	rmdir --ignore-fail-on-non-empty ${INSTALL_BIN}
