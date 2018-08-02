ifeq (${CXX},)
CXX=g++
endif
LINK=${CXX}

TOPDIR=`pwd`/../..
COMMONPATH=`pwd`/..
EASYWSCPATH=${TOPDIR}/samples/easywsclient
LIBPATH=${TOPDIR}/libs
#-static-libstdc++ 
LDFLAGS= -static-libgcc -std=c++11 -Wl,-rpath,$(LIBPATH)
CXXFLAGS  = -std=c++0x -fPIC -g -fno-omit-frame-pointer \
			-DNDEBUG=1 -Wconversion -O3 -Wall -W -fvisibility=hidden
					
LIB	   = -pthread -lpthread -L$(LIBPATH) -lagora-rtc-sdk -lrt -ljson
INCPATH =-I. -I${TOPDIR}/include -I${COMMONPATH} -I${EASYWSCPATH}

SRC =$(wildcard ../easywsclient/*.cpp *.cpp)
OBJ=$(addsuffix .o, $(basename $(SRC)))
TARGET=wayang_linux


.PHONY: all clean
all: $(TARGET)

$(TARGET): $(OBJ)
	$(LINK) $(LDFLAGS) $(INCPATH) $^ -o "$@" $(LIB)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

clean:
	rm -f $(TARGET)
	rm -f ${OBJ}
