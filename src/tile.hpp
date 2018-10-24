#ifndef TILE_HPP
#define TILE_HPP

class tile
{
public:

    tile(int x = 0, int y = 0, int scale_x = 0, int scale_y = 0)
        : position_x_(x), position_y_(y),
          scale_x_(scale_x), scale_y_(scale_y)
    {}
    ~tile() {}

    struct position
    {
        int x;
        int y;
    };

    position get_tile_position()
    {
        return { position_x_ * scale_x_, position_y_ * scale_y_ };
    }

    position get_logical_position()
    {
        return { position_x_, position_y_ };
    }

protected:
    
    int position_x_;
    int position_y_;

    int scale_x_;
    int scale_y_;
};

#endif
