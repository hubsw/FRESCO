# A simple makefile generator by KiSoft, 2008. mailto: kisoft@rambler.ru
# version: 0.3.12.1

# Project Variables start
CPP=g++
CC=gcc
LD=g++
LIB=ar
WINDRES=
# Project Variables end


# Target: Release

OBJS_RELEASE=../main.o

Release: ../FRESCO

../FRESCO: $(OBJS_RELEASE)
	@echo Building console executable ../FRESCO
	@g++ -lz -lboost_system -lboost_filesystem -lboost_iostreams -o ../FRESCO $(OBJS_RELEASE)  -s  

../main.o: ../source/main.cpp
	@echo Compiling: ../source/main.cpp
	@g++ -Wall -Wunused-variable -fexceptions -std=c++11 -O3 -c ../source/main.cpp -o ../main.o

../source/main.cpp: ../include/* ../include/Sequence/* ../include/Index/*.h ../include/Compressor/* ../include/Serializer/* ../include/Config/ConfigFileParser.h

.PHONY: clean

clean: 
	@echo Delete $(OBJS_RELEASE) ../FRESCO
	-@rm -f $(OBJS_RELEASE) ../FRESCO



