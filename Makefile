FILES= main.o bot.o commands.o
LFLAGS=
RFLAGS=-lgloox -lpthread -lsqlite3 -g3 #-lboost_regex
COMPILER=g++
nameprog=bot

world: main.o bot.o commands.o
	$(COMPILER) $(LFLAGS) $(FILES) $(RFLAGS) -o $(nameprog)
main.cpp:
	$(COMPILER) main.cpp -c
bot.cpp:
	$(COMPILER) bot.cpp -c
commands.cpp:
	$(COMPILER) commands.cpp -c

clean:
	rm $(FILES) $(nameprog)
