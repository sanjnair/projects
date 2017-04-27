#include "game.h"

extern "C" int game_run(const char *filename, char *out_results, int resultsLen)
{
  Game game;
  std::string strResults = game.run(filename);
  strResults.copy(out_results, resultsLen);
  out_results[resultsLen-1] = '\0';
  return 0;
}
