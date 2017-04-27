///
/// Reads game input file and stores information
/// @author - Sanjay Nair
///
#ifndef _GAME_INPUT_READER_H_
#define _GAME_INPUT_READER_H_

#include <string>
#include <vector>
#include "gameutil.h"

class GameInputReader {
public:
    GameInputReader();
    void read(const char *filename);
    size_t getNumRows() const;
    size_t getNumCols() const;
    std::string getFilePath() const;

public:
    class CellIterator
    {
    public:
        CellIterator(const GameInputReader* pReader);
        Cell beg();
        Cell nxt();
        bool end() const;

    private:
        const GameInputReader* _pReader;
        std::vector<Cell>::const_iterator _it;
    };

private:
    size_t _numRows;
    size_t _numCols;
    std::string _filePath;
    std::vector<Cell> _points;
};
#endif //_GAME_INPUT_READER_H_
