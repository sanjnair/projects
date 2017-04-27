# Code Challenge: Go-like Board Game
author - SANJAY NAIR

## Unzipping code

## File Organization

###cpp folder

1. game.h/.cc               -- Game engine that drives the game
2. gameboard.h/.cc          -- Encapsulates game board. Logic for accepting valid move, performing capture etc.
3. gameexception.h/.cc      -- Exception class
4. gameinputreader.h/.cc    -- Input file reader. Responsible for parsing and error handling of input.
5. gameplayer.h/.cc         -- Abstract player class. 
6. gameutil.h/.cc           -- Utility classes and functions
7. staticgameplayer.h       -- Concrete player class derived from gameplayer.
8. game_c_shim.cc           -- Part of the package - You may want to replace this file with your copy.

###c folder
9. gametester.c             -- Main function - You may want to replace this file with your copy
- code is implemented using C++ 11 standard on windows platform. Tested on Ubuntu shell.
- All tests passed during local test.

###tests folder
Contains tests recieved as part of the assignment. Also, I've added additional test cases.
This also contains a script named validate, that'll do regression testing (when using my version of gametester.c)

## Building and running code

After unzipping the code, open a shell and go to the cpp folder and run make command
1. cd <location of code>/src/cpp
2. make



## Design

Game class is responsible for getting the moves from file reader, creating players, running the game and reporting the status of the game. 

Game class relies on the board class to enforce the rules of the game. Game class instructs the player to set a move. If the player is a static player, then it blindly follows the move provided by the Game class. If it is a AI player (not implemented), then it could calculate the move based on the board position.

Board class implements the storage for the stones and enforces the rules of the game. This class uses a modified BFS (breadth-first-search) algorithm to determine liberties. This class also performs capture operation if needed.

## Additional tests
I have created number of additional tests to test the program. These tests include:

1. Testing invalid input file
2. Capture scenarios
3. Self capture scenarios
4. Illegal move scenarios

These tests can be found under the tests folder

