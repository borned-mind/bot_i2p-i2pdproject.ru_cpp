FILES= main.o bot.o commands.o installed.c servers.db SimplyClassSocket.hpp somefunc.hpp
LFLAGS= -L./
RFLAGS=-lgloox -lpthread -lsqlite3 -lsimplysocket -O2 #-lboost_regex
COMPILER=g++
nameprog=bot

LIBSIMPLYSOCKETPATH=./ShitSimplySocketClass
LIBNAME=libsimplysocket.so

libsimplysocket:
	cd $(LIBSIMPLYSOCKETPATH) && make -f Makefile
	mv $(LIBSIMPLYSOCKETPATH)/library/*.* .

world: libsimplysocket main.o bot.o commands.o
	$(COMPILER) $(LFLAGS) $(FILES) $(RFLAGS) -o $(nameprog)
main.cpp:
	$(COMPILER) main.cpp -c
bot.cpp:
	$(COMPILER) bot.cpp -c
commands.cpp:
	$(COMPILER) commands.cpp -c

clean:
	rm $(FILES) $(nameprog) $(LIBNAME)
