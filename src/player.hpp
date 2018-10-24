#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "tile.hpp"

class movable_tile : public tile
{
public:
    movable_tile(int x = 0, int y = 0, int scale_x = 0, int scale_y = 0)
        : tile(x, y, scale_x, scale_y) {}

    void move_up()
    {
        position_y_--;
    }
    void move_down()
    {
        position_y_++;
    }
    void move_left()
    {
        position_x_--;
    }
    void move_right()
    {
        position_x_++;
    }

private:
};

#endif
