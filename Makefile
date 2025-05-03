# basic makefile
HERE=$(shell pwd)

CXX             = g++
RM		= rm -rf

INCLUDEDIRS 	= -I$(XXUSBDIR)/include -I./include -I/opt/homebrew/include/
LIBDIRS	        = -L$(XXUSBDIR)/lib
LIBS		    = -lxx_usb -lm -lusb

CXXFLAGS      	= -g -O2 `root-config --cflags` -stdlib=libc++ 

LDFLAGS         = -O2 `root-config --glibs` -L$(XXUSBDIR)/lib -L/opt/homebrew/lib/ -lusb

SOURCES         = MyDaq.cpp   GenericModule.cpp    CC-DAQ.cpp MyMainFrame.cpp
INCLUDES        = MyDaq.h     GenericModule.h      Module.h   Common.h MyMainFrame.h
OBJ             = runMyDaq

SOURCEDIR       = src/
INCLUDEDIR      = include/

CSOURCES        = $(addprefix $(SOURCEDIR), $(SOURCES))
CINCLUDES       = $(addprefix $(INCLUDEDIR),$(INCLUDES))

RUNDIR          = run/

all: $(CSOURCES) $(CINCLUDES)
	$(CXX) $(CXXFLAGS) $(INCLUDEDIRS) $(CSOURCES)  \
	$(LIBDIRS) $(LIBS) $(LDFLAGS) -o $(OBJ)
	[ -d $(RUNDIR) ] || mkdir $(RUNDIR)
	mv $(OBJ)  $(RUNDIR)
	[ -f $(RUNDIR)env.sh ] || echo "export XXUSBDIR="$(XXUSBDIR) >> $(RUNDIR)env.sh
	@echo "Done"

clean:
	$(RM) $(RUNDIR)$(OBJ)
