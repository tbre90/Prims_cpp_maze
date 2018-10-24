#include <unordered_set>
#ifndef MAZE_HPP
#define MAZE_HPP

#include <memory>
#include <random>
#include <algorithm>
#include <vector>

#include "tile.hpp"

class maze
{
public:
    enum tile_state { BLOCKED = 0, PASSAGE = 1 , EXIT = 2 };
    enum class direction { NORTH, SOUTH, EAST, WEST };

    maze(int w, int h)
        : width_(w), height_(h),
          gen_(rd_()), dis_(0, 3),
          maze_end_(nullptr)
    {
        maze_ = std::make_unique<int[]>(w * h);
    }

    ~maze()
    {
    }

    enum tile_state get_tile(tile::position pos, enum class direction dir)
    {
        if (dir == direction::NORTH)
        {
            return static_cast<enum tile_state>(maze_.get()[(pos.y - 1) * width_ + pos.x]);
        }
        else if(dir == direction::SOUTH)
        {
            return static_cast<enum tile_state>(maze_.get()[(pos.y + 1) * width_ + pos.x]);
        }
        else if (dir == direction::EAST)
        {
            return static_cast<enum tile_state>(maze_.get()[pos.y * width_ + (pos.x + 1)]);
        }
        else
        {
            return static_cast<enum tile_state>(maze_.get()[pos.y * width_ + (pos.x - 1)]);
        }
    }

    const int * const generate_maze()
    {
        seed_maze();
        create_maze();
        return maze_.get();
    }

    void print_maze()
    {
        for (int i = 0; i < height_; i++)
        {
            for (int j = 0; j < width_; ++j)
            {
                std::cout << maze_.get()[(i * width_) + j] << ", ";
            }

            std::cout << std::endl;
        }
    }

private:
#define NORTH(m) (get_north_neighbour(maze_.get(), (m), width_, height_))
#define SOUTH(m) (get_south_neighbour(maze_.get(), (m), width_, height_))
#define EAST(m)  (get_east_neighbour(maze_.get(), (m), width_, height_))
#define WEST(m)  (get_west_neighbour(maze_.get(), (m), width_, height_))

    struct cell_mark
    {
        int *cell;
        direction dir;
    };

    int width_;
    int height_;

    std::random_device rd_;
    std::mt19937 gen_;
    std::uniform_int_distribution<> dis_;

    std::unique_ptr<int[]> maze_;

    int *maze_end_;

    int* get_north_neighbour(int *origin, int *maze, int width, int )
    {
        if (!maze) { goto return_null; }

        // if this fails then we're on first row
        // going north would be out of array bounds
        if (! (maze >= origin && maze < origin + width))
        {
            int *north_neighbour = maze - width;
            return north_neighbour;
        }

        return_null:
        return nullptr;
    }

    int* get_south_neighbour(int *origin, int *maze, int width, int height)
    {
        if (!maze) { goto return_null; }

        // if this fails then we're on the last row
        // going south would be out of array bounds
        if (! ((origin + (width * height - 1) <= maze)))
        {
            int *south_neighbour = maze + width;
            return south_neighbour;
        }

        return_null:
        return nullptr;
    }

    int* get_east_neighbour(int *origin, int *maze, int width, int height)
    {
        if (!maze) { goto return_null; }

        int *row = find_row(origin, maze, width, height);
        if (!(maze == (row + width - 1))) // if fails -> can't go further east
        {
            return maze + 1;
        }

        return_null:
        return nullptr;
    }

    int* get_west_neighbour(int *origin, int *maze, int width, int height)
    {
        if (!maze) { goto return_null; }

        int *row = find_row(origin, maze, width, height);

        if (!(maze == row)) // if fails -> can't go further west
        {
            return maze - 1;
        }

        return_null:
        return nullptr;
    }

    int* find_row(int *origin, int *maze, int width, int )
    {
        int *row = origin;
        while (row + width - 1 < maze)
        { row += width; }

        return row;
    }

    int* pick_random_frontier_cell(std::unordered_set<int*> frontier)
    {
        size_t random = gen_() % frontier.size();
        size_t i = 0;
        auto it = frontier.begin();
        while (i < random)
        {
            it++;
            i++;
        }

        return *it;
    }

    std::vector<cell_mark> get_neighbour_passages(int *frontier_cell)
    {
        std::vector<cell_mark> neighbours = std::vector<cell_mark>();

        int *north = NORTH(NORTH(frontier_cell));
        int *south = SOUTH(SOUTH(frontier_cell));
        int *east  = EAST(EAST(frontier_cell));
        int *west  = WEST(WEST(frontier_cell));

        if (north && *north == PASSAGE)
        {
            neighbours.push_back( { north, direction::SOUTH } );
        }
        if (south && *south == PASSAGE)
        {
            neighbours.push_back( { south, direction::NORTH } );
        }
        if (east && *east == PASSAGE)
        {
            neighbours.push_back( { east, direction::WEST } );
        }
        if (west && *west == PASSAGE)
        {
            neighbours.push_back( { west, direction::EAST } );
        }

        return neighbours;
    }

    void mark_passage(cell_mark cell)
    {
        if (cell.dir == direction::NORTH)
        {
            *(NORTH(cell.cell)) = PASSAGE;
        }
        else if (cell.dir == direction::SOUTH)
        {
            *(SOUTH(cell.cell)) = PASSAGE;
        }
        else if (cell.dir == direction::EAST)
        {
            *(EAST(cell.cell)) = PASSAGE;
        }
        else
        {
            *(WEST(cell.cell)) = PASSAGE;
        }
    }

    cell_mark get_random_neighbour_passage(std::vector<cell_mark> &neighbours)
    {
        size_t num = gen_();
        size_t index = num % neighbours.size();
        return neighbours[index];
    }

    bool is_blocked(int *tile)
    {
        return *tile == BLOCKED;
    }

    void create_maze()
    {
        // top left tile starts as a passage
        int *maze = maze_.get();

        int *frontier_cell = &maze[0];
        *frontier_cell = PASSAGE;

        // load up first frontier cells
        int *north_neighbour = NORTH(NORTH(frontier_cell));
        int *south_neighbour = SOUTH(SOUTH(frontier_cell));
        int *east_neighbour  = EAST(EAST(frontier_cell));
        int *west_neighbour  = WEST(WEST(frontier_cell));

        std::unordered_set<int*> frontier = std::unordered_set<int*>();

        if (north_neighbour && is_blocked(north_neighbour)) { frontier.insert(north_neighbour); }
        if (south_neighbour && is_blocked(south_neighbour)) { frontier.insert(south_neighbour); }
        if (east_neighbour  && is_blocked(east_neighbour))  { frontier.insert(east_neighbour); }
        if (west_neighbour  && is_blocked(west_neighbour))  { frontier.insert(west_neighbour); }

        while (frontier.size())
        {

            frontier_cell = pick_random_frontier_cell(frontier);
            *frontier_cell = PASSAGE;
            std::vector<cell_mark> neighbours = get_neighbour_passages(frontier_cell);
            cell_mark random_neighbour = get_random_neighbour_passage(neighbours);
            mark_passage(random_neighbour);

            // add new passage's neighbours as new frontier cells
            north_neighbour = NORTH(NORTH(frontier_cell));
            south_neighbour = SOUTH(SOUTH(frontier_cell));
            east_neighbour  = EAST(EAST(frontier_cell));
            west_neighbour  = WEST(WEST(frontier_cell));

            if (north_neighbour && is_blocked(north_neighbour)) { frontier.insert(north_neighbour); }
            if (south_neighbour && is_blocked(south_neighbour)) { frontier.insert(south_neighbour); }
            if (east_neighbour  && is_blocked(east_neighbour))  { frontier.insert(east_neighbour); }
            if (west_neighbour  && is_blocked(west_neighbour))  { frontier.insert(west_neighbour); }

            frontier.erase(frontier_cell);
        }

        *frontier_cell = EXIT;
    }

    void seed_maze()
    {
        int *t = maze_.get();
        for (int i = 0; i < height_; i++)
        {
            for (int j = 0; j < width_; ++j)
            {
                t[(i * width_) + j] = BLOCKED;
            }
        }
    }
};

#endif
