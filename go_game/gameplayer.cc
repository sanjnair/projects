///
/// Game Exception class
/// @author - Sanjay Nair
///
#include "gameplayer.h"
using namespace std;

///
/// Creates GamePlayer
///
GamePlayer::GamePlayer(int playerID, StoneColor color)
    : _id(playerID)
    , _color(color)
    , _pBoard(NULL)
{}

///
/// Destroys GamePlayer
///
GamePlayer::~GamePlayer()
{}

///
/// Returns player ID
///
int GamePlayer::getId() const {
    return _id;
}

///
/// Returns player's color
///
StoneColor GamePlayer::getPlayerColor() const
{
    return _color;
}

///
/// Sets the Game board pointer
///
void GamePlayer::setBoard(GameBoard* pBoard) 
{
    _pBoard = pBoard;
}

///
/// Returns the Game board pointer
///
GameBoard* GamePlayer::getBoard() const
{
    return _pBoard;
}
