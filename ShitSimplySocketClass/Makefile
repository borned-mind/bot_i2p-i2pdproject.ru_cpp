NAMELIBRARY=simplysocket
FLAGS = -fPIC
TESTPROGRAMNAME=testprogram

all: socket.o somefunc.o library SimplyClassCopyHeaders

SimplyClassCopyHeaders:
	cp source/socket.hpp library/SimplyClassSocket.hpp
	cp source/somefunc.hpp library/somefunc.hpp
test:
	g++ source/main.cpp -L library/ -l$(NAMELIBRARY) -o $(TESTPROGRAMNAME)

socket.o:
	g++ -c $(FLAGS) source/socket.cpp -o build/socket.o
somefunc.o:
	g++ -c $(FLAGS) source/somefunc.cpp -o build/somefunc.o
	
library:
	mkdir library
	g++ -shared build/*.o -o library/lib$(NAMELIBRARY).so
clean:
	rm -rf library
	rm -rf $(TESTPROGRAMNAME)
	rm -rf build/*
reagain: clean all test
	
starttest: reagain
	LD_LIBRARY_PATH=./library ./$(TESTPROGRAMNAME)
