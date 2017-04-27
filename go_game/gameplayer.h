///
/// Abstract class for Game Player
/// @author - Sanjay Nair
///

#ifndef _GAME_PLAYER_H_
#define _GAME_PLAYER_H_

#include "gameboard.h"

class GamePlayer
{
public:
    GamePlayer(int playerID, StoneColor color);
    virtual ~GamePlayer();
    virtual int getId() const;
    virtual StoneColor getPlayerColor() const;
    
    virtual void setBoard(GameBoard* pBoard);
    virtual GameBoard* getBoard() const;

    virtual void setMove(int row, int col) = 0;
    virtual bool playMove() = 0;

protected:
    int _id;
    StoneColor _color;
    GameBoard* _pBoard;
};

#endif //_GAME_PLAYER_H_
