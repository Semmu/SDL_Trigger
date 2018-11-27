#ifndef MAZE_H
#define MAZE_H

#include <SDL2/SDL.h>
#include <array>

struct Maze_t {
    SDL_Surface* surface;
    size_t level;
    size_t coinsCollected;

    size_t playerX, playerY;
    int moveX, moveY;

    static const int MAP_SIZE = 10;
    static const int TILE_SIZE = 30;
    static const int RANDOM_WALLS = 10;


    enum Tile {
        EMPTY,
        WALL,
        COIN,
        PLAYER,
        COUNT
    };
    Tile tiles[MAP_SIZE][MAP_SIZE];
    Uint32 colors[Tile::COUNT];

    Maze_t();

    void setColors();
    void generate();

    bool hasCoin();
    size_t countEmptyTiles();
    Tile& nthEmptyTile(size_t index);

    void drop(Tile tile);

    void findPlayer();

    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();

    void move();

    SDL_Surface* render();

};

extern Maze_t Maze;

#endif /* MAZE_H */
