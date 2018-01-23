ifeq (${CXX},)
CXX=g++
endif
LINK=${CXX}
TOPDIR=`pwd`
LIBPATH=${TOPDIR}/../libs
CPP11 = -std=c++11
TARGET=sig_demo_mul

LD_LIBRARY_PATH=./../libs
export LD_LIBRARY_PATH

.PHONY: all clean

all: $(TARGET)

$(TARGET):
	#$(LINK) demo_multi.cpp -o $@ -I ../include -W -L${LIBPATH} -lpthread -lagorasig -std=c++11
	$(LINK) demo.cpp -o sig_demo -I ../include -W -L${LIBPATH} -lagorasig -std=c++11

clean:
	rm -f $(TARGET)
	

