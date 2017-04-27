///
/// Reads game input file and stores information
/// @author - Sanjay Nair
///

#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <string.h>
#include "gameinputreader.h"
#include "gameexception.h"

using namespace std;

//-----------------------------------------------------------------------------
static const size_t MAX_FILE_LEN    = 256;
static const size_t MAX_FILE_SIZE   = 1048576; // 1MB
static const size_t MAX_ROWS        = 50;
static const size_t MAX_COLS        = 50;
static const size_t MAX_MOVES       = 50000;
static char SEP_CHAR                = ',';
//-----------------------------------------------------------------------------

///
/// Constructs the game input reader
///
GameInputReader::GameInputReader() 
    : _numRows(0)
    , _numCols(0) {
}

///
/// Returns the number of rows
///
size_t GameInputReader::getNumRows() const
{
    return _numRows;
}

///
/// Returns the number of cols
///
size_t GameInputReader::getNumCols() const
{
    return _numCols;
}

///
/// Get loaded file Path
///
std::string GameInputReader::getFilePath() const
{
    return _filePath;
}

///
/// static function to check if the filename is valid
/// @throw GameException
///
static void validateFile(const char *filename) 
{
    if (NULL == filename) {
        string msg("Invalid filename(NULL)");
        throw GameException(msg);
    }

    /// Validate the filename length
    size_t len = strlen(filename);
    if ((len == 0) || len >= MAX_FILE_LEN) {
        string msg("Invalid file name length. Must be greater than 0 and less than ");
        msg += to_string(MAX_FILE_LEN);
        throw GameException(msg, __FILE__, __LINE__);
    }

    struct stat buf;
    int status = stat(filename, &buf);
    if (status != 0) {
        string msg("Unable to read file ");
        msg += filename;
        msg += ". status = ";
        msg += to_string(status);
        throw GameException(msg, __FILE__, __LINE__);
    }

    if ((buf.st_size == 0) || (buf.st_size > static_cast<int>(MAX_FILE_SIZE))) {
        string msg("Invalid file size for file ");
        msg += filename;
        msg += ". Must be > 0 and <= to ";
        msg += to_string(MAX_FILE_SIZE);
        throw GameException(msg, __FILE__, __LINE__);
    }
}

///
/// Get pair of unsigned integers specified by the separator
///
static RESULT getInPairHelper(
    const string& str, 
    pair<size_t, size_t>& pair)
{
    if (str.length() == 0) {
        return RES_ERR;
    }

    size_t sepIndex = str.find(SEP_CHAR);
    if ((sepIndex == string::npos) || 
        (sepIndex == 0) || 
        (sepIndex == str.length() - 1)) {

        return RES_ERR;
    }
    string left = str.substr(0, sepIndex);
    string right = str.substr(sepIndex + 1);
    
    RESULT result = Util::toSize_t(left, pair.first);
    if (result == RES_OK) {
        result = Util::toSize_t(right, pair.second);
    }

    return result;
}

///
/// Get pair of unsigned integers specified by the separator
///
static void getInPair(
    const string& str,
    pair<size_t, size_t>& pair,
    size_t lineNum) {

    RESULT result = getInPairHelper(str, pair);
    if (result != RES_OK) {
        string msg("unable to parse line # ");
        msg += to_string(lineNum);
        msg += " '" + str + "'";
        throw GameException(msg, __FILE__, __LINE__);
    }
}

///
/// Validates the header
/// @throw GameException
///
static void validateRowsAndCols(
    const pair<size_t, size_t>& inPair,
    size_t lineNum,
    const char *filename)
{
    size_t cols = inPair.first;
    size_t rows = inPair.second;

    if (cols == 0 || rows == 0 || cols > MAX_COLS || rows > MAX_ROWS) {
        string msg("Invalid rows/cols for file name ");
        msg += filename;
        msg += " line number ";
        msg += to_string(lineNum);
        msg += ". (";
        msg += " cols = " + to_string(cols);
        msg += " rows = " + to_string(rows);
        msg += ". )";
        msg += ". Expected cols > 0 and <= " + to_string(MAX_COLS);
        msg += ". Expected rows > 0 and <= " + to_string(MAX_ROWS);

        throw GameException(msg, __FILE__, __LINE__);
    }
}

///
/// Validates the header
/// @throw GameException
///
static void validateCell(
    const pair<size_t, size_t>& inPair,
    size_t lineNum,
    const char *filename,
    size_t numCols,
    size_t numRows)
{
    size_t col = inPair.first;
    size_t row = inPair.second;

    if (col >= numCols || row >= numRows) {
        string msg("row / col is invalid for file ");
        msg += filename;
        msg += ". (";
        msg += " col = " + to_string(col);
        msg += " row = " + to_string(row);
        msg += ". )";
        msg += ". Expected col >= 0 and < " + to_string(numCols);
        msg += ". Expected row >= 0 and < " + to_string(numRows);

        throw GameException(msg, __FILE__, __LINE__);
    }
}

///
/// Reads input file and loads the content
/// @throw GameException
///
void GameInputReader::read(const char *filename) 
{
    _numRows = 0;
    _numCols = 0;
    _filePath.clear();
    _points.clear();

    // Validates input file path/name
    validateFile(filename);
    
    ifstream fstream(filename, ifstream::in);
    if (!fstream.is_open()) {
        string msg("Unable to read file for processing input ");
        msg += filename;
        throw GameException(msg, __FILE__, __LINE__);
    }

    string line;
    size_t lineNum = 0;
    pair<size_t, size_t> inPair;
    bool headerParsed = false;

    while (getline(fstream, line)) {
        ++lineNum;

        if (fstream.fail() || fstream.bad()) {
            string msg("file stream read error. ");
            msg += filename;
            throw GameException(msg, __FILE__, __LINE__);
        }
        line = Util::trim(line);
        if (line.empty()) {
            continue;
        }

        getInPair(line, inPair, lineNum);

        if (!headerParsed) {
            validateRowsAndCols(inPair, lineNum, filename);
            _numCols = inPair.first;
            _numRows = inPair.second;
            headerParsed = true;
        }
        else {
            validateCell(inPair, lineNum, filename, _numCols, _numRows);
            _points.push_back(Cell(inPair.second, inPair.first));

            if (_points.size() >= MAX_MOVES) {
                string msg("Invalid input file. reached max moves. ");
                msg += to_string(MAX_MOVES);
                throw GameException(msg, __FILE__, __LINE__);
            }
        }
    }

    /// Do a final validation
    if ((_numRows == 0) || (_numCols == 0) || _points.empty()) {
        string msg("Invalid input file. ");
        msg += filename;
        msg += " rows/cols/grid points are empty.";
        msg += " rows=" + to_string(_numRows);
        msg += " cols=" + to_string(_numCols);
        msg += " points=" + to_string(_points.size());
        throw GameException(msg, __FILE__, __LINE__);
    }

    _filePath = filename;
}

//-CellIterator----------------------------------------------------------------
GameInputReader::CellIterator::CellIterator(const GameInputReader* pReader)
    : _pReader(pReader)
{}

///
/// Returns the begining of the Cell from the reader
///
Cell GameInputReader::CellIterator::beg()
{
    Cell cell(-1, -1);
    _it = _pReader->_points.cbegin();

    if (_it == _pReader->_points.cend()) {
        return Cell(-1, -1);
    }
    return *_it;
}

///
/// Returns the next Cell from the reader
///
Cell GameInputReader::CellIterator::nxt()
{
    if (_it != _pReader->_points.cend()) {
        ++_it;
    }

    if (_it == _pReader->_points.cend()) {
        return Cell(-1, -1);
    }
    return *_it;
}

///
/// Returns true, if iterator end has reached
///
bool GameInputReader::CellIterator::end() const {
    return (_it == _pReader->_points.cend());
}
