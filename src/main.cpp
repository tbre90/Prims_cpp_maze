#include <SDL.h>
#include <SDL_image.h>

#include <iostream>
#include <string>

#include "maze.hpp"
#include "player.hpp"

#define ARRAY_SIZE(a) (sizeof((a)) / sizeof((a)[0]))

constexpr int TILE_WIDTH  = 32;
constexpr int TILE_HEIGHT = 32;

constexpr int SCREEN_WIDTH = TILE_WIDTH * 21;
constexpr int SCREEN_HEIGHT = TILE_HEIGHT * 21;

enum image_type
{
    WALKABLE_PATH = 0,
    PLAYER = 1,
    EXIT = 2,

    TOTAL
};

struct tile_collection
{
    SDL_Surface **tiles;
    size_t num_tiles;
};

static bool init(SDL_Window **, SDL_Surface **);
static bool load_bmp(SDL_Surface **surface, const char * const path);
static bool load_png(SDL_Surface **surface, const char * const path);
static bool load_pngs(tile_collection *t, const char * const path[], size_t num_png);
static bool load_text(tile_collection *t, const char * const path[], size_t num_files);
static void unload_tiles(tile_collection *t);
static void close(SDL_Window **window);
static void blit_image(SDL_Surface *dest, SDL_Surface *src, SDL_Rect *coords);
static void blit_text(SDL_Surface *dest, tile_collection *t, int x, int y);

const char * const g_success_text[] =
{
    "./assets/Upper_Y.png",
    "./assets/Upper_O.png",
    "./assets/Upper_U.png",
    nullptr,
    "./assets/Upper_W.png",
    "./assets/Upper_O.png",
    "./assets/Upper_N.png",
    "./assets/_Exclamation.png"
};

const char * const g_game_tiles[] =
{
    "./assets/wooden_wall.png",
    "./assets/goblin.png",
    "./assets/exit.png"
};

int main(int argc, char **argv)
{
    argc = argc; argv = argv;

    SDL_Surface *screen = NULL;
    SDL_Surface *images[TOTAL];

    SDL_Window *window = NULL;

    if (!init(&window, &screen))
    { return -1; }

    if (!load_png(&images[WALKABLE_PATH], "./assets/wooden_wall.png"))
    { return -2; }

    if (!load_png(&images[PLAYER], "./assets/goblin.png"))
    { return -3; }

    if (!load_png(&images[EXIT], "./assets/exit.png"))
    { return -4; }

    tile_collection game_tiles = {0};
    game_tiles.tiles     = new SDL_Surface*[ARRAY_SIZE(g_game_tiles)]();
    if (!load_pngs(&game_tiles, g_game_tiles, ARRAY_SIZE(g_game_tiles)))
    {
        return -5;
    }

    tile_collection success_text = {0};
    success_text.tiles     = new SDL_Surface*[ARRAY_SIZE(g_success_text)]();
    if (!load_text(&success_text, g_success_text, ARRAY_SIZE(g_success_text)))
    { return -6; }

    int maze_width = SCREEN_WIDTH / TILE_WIDTH;
    int maze_height = SCREEN_HEIGHT / TILE_HEIGHT;
    maze maze(maze_width, maze_height);
    const int * const generated_maze = maze.generate_maze();

    SDL_Rect offset = {0};
    offset.w = TILE_WIDTH;
    offset.h = TILE_HEIGHT;
    for (int row = 0; row < maze_height; row++)
    {
        offset.y = row * TILE_HEIGHT;
        for (int column = 0; column < maze_width; column++)
        {
            const int *tile = &generated_maze[(row * maze_width) + column];

            if (*tile == maze::PASSAGE)
            {
                offset.x = column * TILE_WIDTH;
                blit_image(screen, images[WALKABLE_PATH], &offset);
            }
            else if (*tile == maze::EXIT)
            {
                offset.x = column * TILE_WIDTH;
                blit_image(screen, images[WALKABLE_PATH], &offset);
                blit_image(screen, images[EXIT], &offset);
            }
        }
    }

    SDL_Rect defpos = {0};
    defpos.x = 0;
    defpos.y = 0;
    defpos.w = TILE_WIDTH;
    defpos.h = TILE_HEIGHT;
    blit_image(screen, images[PLAYER], &defpos);

    // a new player starting at (0, 0) top left corner
    movable_tile player(0, 0, TILE_WIDTH, TILE_HEIGHT);

    bool running = true;
    bool game_over = false;
    SDL_Event e = {0};

    while (running)
    {
        movable_tile::position current_logical = player.get_logical_position();

        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                running = false;
                break;
            }
            if (!game_over)
            {
                if (e.type == SDL_KEYDOWN)
                {
                    switch (e.key.keysym.sym)
                    {
                        case SDLK_w:
                        case SDLK_UP:
                        {
                            if (current_logical.y - 1 >= 0)
                            {
                                maze::tile_state north_tile = maze.get_tile(current_logical, maze::direction::NORTH);
                                if (north_tile == maze::PASSAGE)
                                { player.move_up(); }
                                else if (north_tile == maze::EXIT)
                                { game_over = true; }
                            }
                        } break;
                        case SDLK_s:
                        case SDLK_DOWN:
                        {
                            if (current_logical.y + 1 < maze_height)
                            {
                                maze::tile_state south_tile = maze.get_tile(current_logical, maze::direction::SOUTH);
                                if (south_tile == maze::PASSAGE)
                                { player.move_down(); }
                                else if (south_tile == maze::EXIT)
                                { game_over = true; }
                            }
                        } break;
                        case SDLK_a:
                        case SDLK_LEFT:
                        {
                            if (current_logical.x - 1 >= 0)
                            {
                                maze::tile_state west_tile = maze.get_tile(current_logical, maze::direction::WEST);
                                if (west_tile == maze::PASSAGE)
                                { player.move_left(); }
                                else if (west_tile == maze::EXIT)
                                { game_over = true; }
                            }
                        } break;
                        case SDLK_d:
                        case SDLK_RIGHT:
                        {
                            if (current_logical.x + 1 < maze_width)
                            {
                                maze::tile_state east_tile = maze.get_tile(current_logical, maze::direction::EAST);
                                if (east_tile == maze::PASSAGE)
                                { player.move_right(); }
                                else if (east_tile == maze::EXIT)
                                { game_over = true; }
                            }
                        } break;
                        default:
                        {
                        } break;
                    }

                    SDL_Rect pcurr = {0};
                    SDL_Rect pprev = {0};

                    pprev.x = current_logical.x * TILE_WIDTH;
                    pprev.y = current_logical.y * TILE_HEIGHT;
                    pprev.w = TILE_WIDTH;
                    pprev.h = TILE_HEIGHT;

                    movable_tile::position next_tile = player.get_tile_position();

                    pcurr.x = next_tile.x;
                    pcurr.y = next_tile.y;
                    pcurr.w = TILE_WIDTH;
                    pcurr.h = TILE_HEIGHT;

                    blit_image(screen, images[WALKABLE_PATH], &pprev);
                    blit_image(screen, images[PLAYER], &pcurr);
                }
            }
            else
            {
                int letter_width  = success_text.tiles[0]->w * static_cast<int>(success_text.num_tiles);
                int letter_height = success_text.tiles[0]->h;
                blit_text(screen, &success_text, SCREEN_WIDTH / 2 - letter_width / 2, (SCREEN_HEIGHT / 2) - (letter_height / 2));
            }

            SDL_UpdateWindowSurface(window);
        }
    }

    unload_tiles(&success_text);
    delete[] success_text.tiles;
    unload_tiles(&game_tiles);
    delete[] game_tiles.tiles;

    close(&window);

    return 0;
}

static bool
init(SDL_Window **window, SDL_Surface **window_surface)
{
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL_Init error: " << SDL_GetError() << std::endl;
        success = false;
    }
    else
    {
        *window = SDL_CreateWindow("Hello World!", 100, 100,  SCREEN_WIDTH, SCREEN_HEIGHT, 0);
        if (!(*window))
        {
            SDL_Quit();
            std::cout << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;
            success = false;
        }
        else
        {
            int image_flags = IMG_INIT_PNG;
            if (!(IMG_Init(image_flags) & image_flags))
            {
                std::cout << "SDL_image could not initialize. SDL_image error: " << IMG_GetError() << std::endl;
                success = false;
            }
            else
            {
                *window_surface = SDL_GetWindowSurface(*window);
            }
        }
    }

    return success;
}

static bool
load_pngs(tile_collection *t, const char * const path[], size_t num_png)
{
    bool success = true;

    for (size_t i = 0; i < num_png; i++)
    {
        success = load_png(&(t->tiles[i]), path[i]);

        if (!success)
        { break; }
        else
        {
            t->num_tiles++;
        }
    }

    return success;
}

static bool
load_png(SDL_Surface **surface, const char * const path)
{
    bool success = true;

    *surface = IMG_Load(path);
    if (!(*surface))
    {
        std::cout << "Could not load image " << path << ". SDL error: " << IMG_GetError() << std::endl;
        success = false;
    }

    return success;
}

static bool
load_text(tile_collection *t, const char * const path[], size_t num_files)
{
    bool success = true;
    for (size_t i = 0; i < num_files && success; i++)
    {
        if (path[i])
        {
            success = load_png(&(t->tiles[i]), path[i]); 
            if (success)
            { t->num_tiles++; }
            else
            { success = false; }
        }
        else // nullptr == space
        {
            t->num_tiles++;
        }
    }

    return success;
}

static void
blit_text(SDL_Surface *dest, tile_collection *t, int x, int y)
{
    SDL_Rect r = {0};
    r.x = x;
    r.y = y;
    for (size_t i = 0; i < t->num_tiles; i++)
    {
        if (t->tiles[i])
        {
            r.w = t->tiles[i]->w;
            r.h = t->tiles[i]->h;

            blit_image(dest, t->tiles[i], &r);
            r.x += t->tiles[i]->w;
        }
        else // space
        {
            r.x += r.w; // use previous letter's width
        }
    }
}

static void
blit_image(SDL_Surface *dest, SDL_Surface *src, SDL_Rect *coords)
{
    SDL_BlitSurface(src, NULL, dest, coords);
}

static void
unload_tiles(tile_collection *t)
{
    if (!t)
    { return; }

    for (size_t i = 0; i < t->num_tiles; i++)
    {
        SDL_FreeSurface(t->tiles[i]);
        t->tiles[i] = nullptr;
    }
}

static void
close(SDL_Window **window)
{
    if (window)
    {
        SDL_DestroyWindow(*window);
        *window = NULL;
    }

    SDL_Quit();
}
