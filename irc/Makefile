all: src/irc_main.cpp
	g++ -c src/irc_main.cpp -o build/irc_main.o
	g++ -c src/irc_rx.cpp -o build/irc_rx.o
	g++ build/irc_main.o build/irc_rx.o -o irc
