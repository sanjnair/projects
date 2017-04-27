///
/// Static Gameplayer class
/// @author - Sanjay Nair
///
#include "staticgameplayer.h"
using namespace std;

///
/// Creates GamePlayer
///
StaticGamePlayer::StaticGamePlayer(int playerID, StoneColor color)
    : GamePlayer(playerID, color)
    , _row(0)
    , _col(0)
    , _color(color)
{}

///
/// Destroys GamePlayer
///
StaticGamePlayer::~StaticGamePlayer()
{}

///
/// Sets the move
///
void StaticGamePlayer::setMove(int row, int col)
{
    _row = row;
    _col = col;
}

///
/// Plays the move
///
bool StaticGamePlayer::playMove() 
{
    if (!_pBoard) {
        return false;
    }

    return _pBoard->moveStone(_row, _col, _color);
}
