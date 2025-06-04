# basic makefile

.LIBPATTERNS ="'lib%.so lib%.a' "

CXX             = g++
RM		= rm -f

LDLIBS		= -lusb -lc
INCLUDEDIRS 	= -I./../include/ -I/opt/homebrew/include/

CXXFLAGS      	= -Wall -fPIC -stdlib=libc++ -std=c++14

LDFLAGS		= -shared

SOURCES =  libxxusb.c
OBJECTS =  $(SOURCES:.c=.o)
LIB     =  libxx_usb.dylib

library: 
	$(CXX) $(CXXFLAGS) $(INCLUDEDIRS) -c $(SOURCES)
	$(CXX)  $(LDFLAGS) -L/opt/homebrew/lib/ $(LDLIBS) $(OBJECTS) -o $(LIB)  
	@chmod 755 $(LIB)
	[ -d ./../lib ] || mkdir ./../lib 	
	cp $(LIB) ./../lib
	@echo "Done"

clean:
	$(RM) $(OBJECTS) $(LIB)

