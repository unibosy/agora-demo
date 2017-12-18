ifeq (${CXX},)
CXX=g++
endif
LINK=${CXX}
TOPDIR=`pwd`
LIBPATH=${TOPDIR}/../libs
CPP11 = -std=c++11
TARGET=sig_demo

LD_LIBRARY_PATH=./../libs
export LD_LIBRARY_PATH

.PHONY: all clean

all: $(TARGET)

$(TARGET):
	$(LINK) demo.cpp -o $@ -I ../include -W -L${LIBPATH} -lagorasig -std=c++11

clean:
	rm -f $(TARGET)
	

