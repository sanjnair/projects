CC=gcc
CPPFLAGS=-Wall -ggdb -std=c++11
Q=@

test: game
	$(Q)echo Running game tests
	$(Q)./game

game_OBJECTS=gametester.o game.o game_c_shim.o gameboard.o gameexception.o gameinputreader.o gameplayer.o gameutil.o staticgameplayer.o
game: $(game_OBJECTS)
	$(CXX) -o $@ $^

gametester.o: ../c/gametester.c
	$(CC) -c -o $@ $(CFLAGS) $^

clean:
	rm -f $(game_OBJECTS) game game.exe
