///
/// Concrete class for Game Player (Static player follows instructions)
/// @author - Sanjay Nair
///

#ifndef _STATIC_GAME_PLAYER_H_
#define _STATIC_GAME_PLAYER_H_

#include "gameplayer.h"

class StaticGamePlayer : public GamePlayer
{
public:
    StaticGamePlayer(int playerID, StoneColor color);
    ~StaticGamePlayer();
    
    void setMove(int row, int col);
    bool playMove();

private:
    int _row;
    int _col;
    StoneColor _color;
};

#endif //_STATIC_GAME_PLAYER_H_
