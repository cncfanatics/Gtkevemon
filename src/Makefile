SHELL = /bin/sh

ifeq (${OS},Windows_NT)
  PLATFORM = win32
else
  PLATFORM = $(shell uname|tr '[:upper:]' '[:lower:]')
endif
include Makefile.$(PLATFORM)

SOURCES = maingui.cc guiuserdata.cc guiconfiguration.cc guiaboutdialog.cc \
          guiskill.cc guiskillplanner.cc guixmlsource.cc guievelauncher.cc \
          guiversionchecker.cc guicharexport.cc \
          gtkevemon.cc gtkserver.cc gtkconfwidgets.cc gtkcharpage.cc \
          gtkportrait.cc gtkinfodisplay.cc gtkplannerbase.cc \
          gtkcolumnsbase.cc gtkhelpers.cc gtkdownloader.cc \
          gtkitemdetails.cc gtkitembrowser.cc gtktrainingplan.cc \
          imagestore.cc \
          server.cc serverlist.cc helpers.cc conf.cc config.cc \
          xml.cc evetime.cc bgprocess.cc pipedexec.cc notifier.cc \
          http.cc asynchttp.cc eveapi.cc versionchecker.cc \
          apibase.cc apicharlist.cc apicharsheet.cc apiintraining.cc \
          apiskilltree.cc apicerttree.cc apiitemtree.cc argumentsettings.cc

OBJECTS = $(foreach file,$(SOURCES),$(subst .cc,.o,$(file)))

gtkevemon: ${OBJECTS}
	${COMPILER} -o ${BINARY} ${OBJECTS} ${LIBS}

multicore:
	$(MAKE) -j${CORES} gtkevemon

depend:
	${COMPILER} -MM ${SOURCES} > Makefile.dep

%.o: %.cc
	${COMPILER} -c -o $@ $< ${CXXFLAGS}

clean: FORCE
	${RM} ${BINARY} ${OBJECTS}

FORCE:

include Makefile.dep
