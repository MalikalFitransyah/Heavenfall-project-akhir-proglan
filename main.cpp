#include <cstdlib>
#include <ctime>
#include "Game.hpp"
 
int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    Game game;
    game.run();
    return 0;
}