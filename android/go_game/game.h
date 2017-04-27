#include <string>

#ifndef _GAME_H_
#define _GAME_H_

#include <vector>
class GamePlayer;

class Game {
public:
    ~Game();
    std::string run(const char *fn);

private:
    void createStaticPlayers();
    void deletePlayers();

private:
    std::vector<GamePlayer*> _players;
};

#endif //_GAME_H_
