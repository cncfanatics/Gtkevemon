SHELL = /bin/sh
COMPILER = g++

CORES = $(shell grep processor /proc/cpuinfo | wc -l)

SOURCES = maingui.cc guiuserdata.cc guiconfiguration.cc guiaboutdialog.cc \
          guiskill.cc guiskillplanner.cc guixmlsource.cc guievelauncher.cc \
          gtkevemon.cc gtkserver.cc gtkconfwidgets.cc gtkcharpage.cc \
          gtkportrait.cc gtkinfodisplay.cc gtkplannerbase.cc \
	  gtkcolumnsbase.cc gtkhelpers.cc \
	  gtktrainingplan.cc gtkskilldetails.cc imagestore.cc \
          server.cc serverlist.cc helpers.cc conf.cc config.cc \
          xml.cc evetime.cc bgprocess.cc pipedexec.cc mailer.cc \
          http.cc asynchttp.cc eveapi.cc versionchecker.cc\
          apibase.cc apicharlist.cc apicharsheet.cc apiintraining.cc \
          apiskilltree.cc argumentsettings.cc

OBJECTS = $(foreach file,$(SOURCES),$(subst .cc,.o,$(file)))
BINARY = gtkevemon

GCC_FLAGS = -Wall -W -ansi -pedantic -Wcast-align -Wcast-qual -Wpointer-arith -Wundef -Wconversion -g
GTK_FLAGS = $(shell pkg-config --cflags gtkmm-2.4)
GTK_LIBS = $(shell pkg-config --libs gtkmm-2.4)
PTH_LIBS = -lpthread
XML_FLAGS = $(shell pkg-config --cflags libxml-2.0)
XML_LIBS = $(shell pkg-config --libs libxml-2.0)

gtkevemon: ${OBJECTS}
	${COMPILER} -o ${BINARY} ${OBJECTS} ${GTK_LIBS} ${PTH_LIBS} ${XML_LIBS}

multicore:
	$(MAKE) -j${CORES} gtkevemon

depend:
	${COMPILER} -MM ${SOURCES} > Makefile.dep

%.o: %.cc
	${COMPILER} -c -o $@ $< ${GCC_FLAGS} ${GTK_FLAGS} ${XML_FLAGS}

clean: FORCE
	rm -f ${BINARY} ${OBJECTS}

FORCE:

include Makefile.dep
