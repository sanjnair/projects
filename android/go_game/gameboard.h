///
/// Game Board class
/// @author - Sanjay Nair
///

#ifndef _GAME_BOARD_H_
#define _GAME_BOARD_H_

#include <vector>
#include <string>
#include "gameutil.h"

class GameBoard
{
public:
    explicit GameBoard(size_t rows, size_t cols);

    size_t getNumRows() const;
    size_t getNumCols() const;
    StoneColor getColor(int row, int col) const;

    bool moveStone(int row, int col, StoneColor color);
    bool isValid() const;

    void printBoard() const;

private:
    bool isValidCell(int row, int col) const;
    bool hasConnectedLiberty(int row, int col, bool captureCells);
    void performCapture();

private:
    bool _isValidBoard;
    size_t _rows;
    size_t _cols;
    StoneColor _lastColor;
    std::vector<std::vector<StoneColor> > _board;
    std::vector<std::vector<bool> > _visited;
    std::vector<Cell> _visitedCells;
};

#endif //_GAME_BOARD_H_
