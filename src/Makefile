SHELL = /bin/sh

ifeq (${OS},Windows_NT)
  PLATFORM = win32
else
  PLATFORM = $(shell uname|tr '[:upper:]' '[:lower:]')
endif
include Makefile.$(PLATFORM)

SOURCES = maingui.cc guiuserdata.cc guiconfiguration.cc guiaboutdialog.cc \
          guiskill.cc guiskillplanner.cc guixmlsource.cc guievelauncher.cc \
          guiversionchecker.cc guicharexport.cc guiskillqueue.cc \
          gtkevemon.cc gtkserver.cc gtkconfwidgets.cc gtkcharpage.cc \
          gtkportrait.cc gtkinfodisplay.cc gtkplannerbase.cc gtkhelpers.cc \
          gtkcolumnsbase.cc gtkdownloader.cc gtkitemdetails.cc \
	  gtkitembrowser.cc gtktrainingplan.cc gtkskillqueue.cc \
          imagestore.cc argumentsettings.cc xmltrainingplan.cc \
          server.cc serverlist.cc helpers.cc conf.cc config.cc \
          xml.cc evetime.cc bgprocess.cc pipedexec.cc notifier.cc \
          http.cc asynchttp.cc eveapi.cc versionchecker.cc \
          apibase.cc apicharlist.cc apicharsheet.cc apiintraining.cc \
          apiskilltree.cc apicerttree.cc apiitemtree.cc apiskillqueue.cc

OBJECTS = $(foreach file,$(SOURCES),$(subst .cc,.o,$(file)))

gtkevemon: ${OBJECTS}
	${CXX} -o ${BINARY} ${OBJECTS} ${LDFLAGS}

multicore:
	$(MAKE) -j${CORES} gtkevemon

depend:
	${CXX} -MM ${SOURCES} > Makefile.dep

%.o: %.cc
	${CXX} -c -o $@ $< ${CXXFLAGS}

clean: FORCE
	${RM} ${BINARY} ${OBJECTS}

FORCE:

include Makefile.dep
