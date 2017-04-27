///
/// Game board class
/// @author - Sanjay Nair
///

#include "gameboard.h"
#include "gameexception.h"
#include <queue>
#include <iostream>
#include <iomanip>

using namespace std;


///
/// define navigation pattern for stones
///
static const vector<pair<int, int>> CELL_NAV_PATTERN = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };

///
/// Creates game board for the given rows and cols
///
GameBoard::GameBoard(
    size_t rows, 
    size_t cols) 
    : _isValidBoard(true)
    , _rows(rows)
    , _cols(cols)
    , _lastColor(StoneColor::NoColor)
    , _board(rows, vector<StoneColor>(cols, StoneColor::NoColor))
    , _visited(rows, vector<bool>(cols, false))
{}

///
/// Returns number of rows in the board
///
size_t GameBoard::getNumRows() const
{
    return _board.size();
}

///
/// Returns true if the board is valid
///
bool GameBoard::isValid() const
{
    return _isValidBoard;
}

///
/// Returns number of cols in the board
///
size_t GameBoard::getNumCols() const
{
    size_t cols = 0;
    if (_board.size() > 0) {
        cols = _board.at(0).size();
    }

    return cols;
}

///
/// Returns color of the cell.
/// If invalid cell, throw game exception
///
StoneColor GameBoard::getColor(
    int row, 
    int col) const
{
    if (!isValidCell(row, col)) {
        string msg("Invalid row/col index specified.");
        msg += " row = " + to_string(row);
        msg += " col = " + to_string(col);
        throw GameException(msg, __FILE__, __LINE__);
    }
    
    return _board[row][col];
}

///
/// Returns true if the given row and col indices are valid
///
bool GameBoard::isValidCell(
    int row, 
    int col) const
{
    return ((row >= 0) && 
            (row < static_cast<int>(_rows)) && 
            (col >= 0) && 
            (col < static_cast<int>(_cols)));
}

///
/// Moves the stone to the specified cell.
/// Returns true if the move is valid,
/// otherwise, returns false
///
bool GameBoard::moveStone(
    int row, 
    int col, 
    StoneColor color)
{
    /// Do early check
    // validate cell
    if (!_isValidBoard                              ||
        !isValidCell(row, col)                      ||
        (StoneColor::NoColor != _board[row][col])   ||
        (StoneColor::NoColor == color)              ||
        (_lastColor == color)) {

        return false;
    }

    _board[row][col] = color;

    /// TODO: Optimization - no need to find liberty again for already connected neighbors
    
    // Flood fill algorithm (using BFS) on neighbors for connectivity
    for (const pair<int, int>& pattern : CELL_NAV_PATTERN) {
        int newRow = row + pattern.first;
        int newCol = col + pattern.second;

        if (isValidCell(newRow, newCol) &&
            (_board[newRow][newCol] != StoneColor::NoColor) &&
            (_board[newRow][newCol] != color)) {
            
            // If no connected liberty, perform capture operation
            if (!hasConnectedLiberty(newRow, newCol, true)) {
                performCapture();
            }
        }
    }

    // check if we have induced a self capture with this move
    if (!hasConnectedLiberty(row, col, false)) {
        _board[row][col] = StoneColor::NoColor;
        _isValidBoard = false;
    }
    else {
        _lastColor = color;
    }
    
    return _isValidBoard;
}

///
/// Performs capture based on the contents of _visitedCells
///
void GameBoard::performCapture() {
    for (const Cell& cell : _visitedCells) {
        _board[cell.row][cell.col] = StoneColor::NoColor;
    }
}

///
/// Returns true if the specified cell has connected liberty.
/// 
/// For each stone S :
/// a.A *liberty* of S is an empty grid point horizontally or vertically adjacent
/// to S.
/// b.A stone *connected* to S is a stone of like color that is horizontally or
/// vertically adjacent to another stone connected to S.
/// c.S is connected to itself.
/// d.S is *captured* if it is not connected to a stone with a liberty.
///
/// Will use BFS algorithm to get the liberty information
/// If liberty is found is any of the connected cell, this function will return immediately.
///
bool GameBoard::hasConnectedLiberty(
    int row,
    int col,
    bool captureCells)
{
    /// clear the _visited and _visitedCells
    for (size_t i = 0; i < _visited.size(); ++i) {
        fill(_visited[i].begin(), _visited[i].end(), false);
    }
    _visitedCells.clear();
    
    StoneColor color = _board[row][col];
    queue<Cell> searchQueue;
    
    searchQueue.push(Cell(row, col));
    _visited[row][col] = true;
    if (captureCells) {
        _visitedCells.push_back(searchQueue.front());
    }

    while (!searchQueue.empty()) {
        Cell cell = searchQueue.front();
        searchQueue.pop();

        for (const pair<int, int>& pattern : CELL_NAV_PATTERN) {
            int newRow = cell.row + pattern.first;
            int newCol = cell.col + pattern.second;

            // check if already visisted a valid cell
            if (isValidCell(newRow, newCol) && !_visited[newRow][newCol]) {
                _visited[newRow][newCol] = true;
                
                // Liberty found!! - return immediately
                if (_board[newRow][newCol] == StoneColor::NoColor) {
                    return true;
                }
                else if (_board[newRow][newCol] == color) {
                    Cell newCell(static_cast<size_t>(newRow), static_cast<size_t>(newCol));
                    searchQueue.push(newCell);
                    _visited[newRow][newCol] = true;

                    if (captureCells) {
                        _visitedCells.push_back(newCell);
                    }
                }
            }
        }
    }

    return false;
}

//====== DEBUGGING CODE =======================================================
///
/// Debug function to print Board
///
void GameBoard::printBoard() const
{
    static const vector<string> COLOR_STR = { "-", "W", "B" };

    cout << endl << endl;
    if (_board.empty()) {
        cout << "Board EMPTY" << endl;
    }
    else {
        size_t rows = getNumRows();
        size_t cols = getNumCols();

        size_t maxColStr = to_string(cols).length() + 1;

        for (size_t col = 0; col < cols; ++col) {
            if (col == 0) {

            }
            cout << setw(maxColStr) << col;
        }
        cout << endl;


        for (size_t row = 0; row < rows; ++row) {
            cout << row;

            for (size_t col = 0; col < cols; ++col) {
                int colorIdx = static_cast<int>(_board[row][col]);
                cout << setw(maxColStr) << COLOR_STR[colorIdx];
            }
            cout << endl;
        }
    }
}