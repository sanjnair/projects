///
/// Game class drives the game
/// @author - Sanjay Nair
///

#include "game.h"
#include "gameutil.h"
#include "gameexception.h"
#include "gameinputreader.h"
#include "staticgameplayer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

using namespace std;

//-- Static definitions - private to this file ----------------------------------

static const string OUT_FILE_EXTN = "out";
static const string MSG_INVALID_GAME = "INVALID GAME";

typedef std::unordered_map<size_t, int> PlayerMap;
typedef std::unordered_map<size_t, PlayerMap> PlayMap;

///
/// Writes play map to stream
///
static void writePlaymap(
    ostream& outStream,
    const GameBoard &board,
    const PlayMap& playMap)
{
    size_t rows = board.getNumRows();
    size_t cols = board.getNumCols();


    for (size_t col = 0; col < cols; ++col) {
        for (size_t row = 0; row < rows; ++row) {
            StoneColor color = board.getColor(row, col);
            if (color != StoneColor::NoColor) {
                PlayMap::const_iterator colIt = playMap.find(col);
                if (colIt != playMap.end()) {
                    PlayerMap::const_iterator rowIt = colIt->second.find(row);
                    if (rowIt != colIt->second.end()) {
                        outStream << rowIt->second << " " << col << " " << row << endl;
                    }
                }
            }
        }
    }
}

///
/// Returns the output string which is the representation of 
/// the board status
///
static std::string getOutput(
    bool isValid,
    const GameBoard &board,
    const PlayMap& playMap)
{
    if (!isValid) {
        return MSG_INVALID_GAME;
    }
    else {
        ostringstream outStream;
        writePlaymap(outStream, board, playMap);
        return outStream.str();
    }
}


///
/// Write output file 
///
//static std::string writeOutputFile(
//    const std::string& inFilepath,
//    bool isValid,
//    const GameBoard &board,
//    const PlayMap& playMap)
//{
//    string outFilepath = Util::replaceFileExtension(inFilepath, OUT_FILE_EXTN);
//    if (outFilepath == inFilepath) {
//        string msg("Invalid input file extension ");
//        msg += OUT_FILE_EXTN;
//        msg += (". Both input and output files are same");
//        Log::e(msg);
//        outFilepath.clear();
//        return outFilepath;
//    }
//
//    ofstream outStream;
//
//    try {
//        outStream.open(outFilepath, ios::out | ios::trunc);
//        if (!outStream.good()) {
//            string msg("Unable to open out file for writing. ");
//            msg += outFilepath;
//            Log::e(msg);
//        }
//        else {
//            if (!isValid) {
//                outStream << MSG_INVALID_GAME.c_str();
//            }
//            else {
//                writePlaymap(outStream, board, playMap);
//            }
//
//            if (!outStream.good()) {
//                string msg("Unable to write content to output file. ");
//                msg += outFilepath;
//                Log::e(msg);
//            }
//
//        }
//        if (outStream.is_open()) {
//            outStream.close();
//        }
//    }
//    catch (const std::exception &e) {
//        string msg("Unable to write to out file. (exception)");
//        if (e.what()) {
//            msg += e.what();
//        }
//        msg += outFilepath;
//        Log::e(msg);
//    }
//
//    return outFilepath;
//}
//-----------------------------------------------------------------------------

///
/// Destroys game object
///
Game::~Game() {
    deletePlayers();
}

///
/// Runs the Game
///
std::string Game::run(const char *filename) 
{
    string boardStatus = "";
    GameInputReader reader;

    try {
        reader.read(filename);
        GameBoard board(reader.getNumRows(), reader.getNumCols());
        
        /// Create players
        createStaticPlayers();
        if (!_players.empty()) {
            // set board for all players
            for (GamePlayer* player : _players) {
                player->setBoard(&board);
            }

            // map of [col - > [row -> player]]
            PlayMap playMap;

            GameInputReader::CellIterator it(&reader);
            size_t playIndex = 0;
            bool isValid = true;
            GamePlayer* player = NULL;

            for (Cell cell = it.beg(); !it.end(); cell = it.nxt()) {
                if (playIndex == _players.size()) {
                    playIndex = 0;
                }
                
                player = _players[playIndex];
                ++playIndex;

                player->setMove(cell.row, cell.col);

                isValid = player->playMove();
                isValid = isValid && board.isValid();

                // TODO: debug method - comment out
                //cout << endl << "(c=" << cell.col << ", r=" << cell.row << ")";
                //board.printBoard();

                if (!isValid) {
                    break;
                }
                // Update playmap
                playMap[cell.col][cell.row] = player->getId();
            }
            
            boardStatus = getOutput(
                            isValid, 
                            board,
                            playMap);
        }
    }
    catch (GameException &ex) {
        Log::e(ex.message());
    } 
    catch (...) {
        Log::e("Unexpected Error.");
    }
    
    return boardStatus;
}

///
/// Create Static Players
///
void Game::createStaticPlayers() {
    StaticGamePlayer* player1 = NULL;
    StaticGamePlayer* player2 = NULL;
    int id = 0;

    player1 = new StaticGamePlayer(++id, StoneColor::White);
    if (NULL != player1) { 
        _players.push_back(player1);
        player2 = new StaticGamePlayer(++id, StoneColor::Black);
        
        if (NULL != player2) {
            _players.push_back(player2);
        }
        else {
            deletePlayers();
        }
    }
}

///
/// Delete Static Players
///
void Game::deletePlayers() {
    if (!_players.empty()) {
        for (GamePlayer* player : _players) {
            delete player;
        }
    }
    _players.clear();
}





