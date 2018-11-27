#include "maze.h"
#include "util.h"

Maze_t Maze;

Maze_t::Maze_t() : surface{NULL}, coinsCollected{0} {
    //
}

void Maze_t::setColors() {
    colors[Tile::EMPTY] = Surface::colorFor(30, 10, 10);
    colors[Tile::WALL] = Surface::colorFor(100, 70, 20);
    colors[Tile::COIN] = Surface::colorFor(200, 200, 30);
    colors[Tile::PLAYER] = Surface::colorFor(50, 100, 250);
}

void Maze_t::generate() {
    for (size_t y = 0; y < MAP_SIZE; y++) {
        for (size_t x = 0; x < MAP_SIZE; x++) {
            tiles[y][x] = (x == 0 || y == 0 || x == MAP_SIZE - 1 || y == MAP_SIZE - 1 ? WALL : EMPTY);
        }
    }
    
    for(size_t i = 0; i < RANDOM_WALLS; i++) {
        nthEmptyTile(rand() % countEmptyTiles()) = WALL;
    }

    drop(COIN);
    drop(COIN);
    drop(PLAYER);

    level++;
}

bool Maze_t::hasCoin() {
    for (size_t y = 0; y < MAP_SIZE; y++) {
        for (size_t x = 0; x < MAP_SIZE; x++) {
            if (tiles[y][x] == COIN) {
                return true;
            }
        }
    }
    return false;
}

size_t Maze_t::countEmptyTiles() {
    size_t count = 0;
    for (size_t y = 0; y < MAP_SIZE; y++) {
        for (size_t x = 0; x < MAP_SIZE; x++) {
            if (tiles[y][x] == EMPTY) {
                count++;
            }
        }
    }
    return count;
}

Maze_t::Tile& Maze_t::nthEmptyTile(size_t index) {
    size_t count = -1;
    for (size_t y = 0; y < MAP_SIZE; y++) {
        for (size_t x = 0; x < MAP_SIZE; x++) {
            if (tiles[y][x] == EMPTY) {
                count++;

                if (count == index) {
                    return tiles[y][x];
                }
            }
        }
    }

    throw std::runtime_error("Maze::nthEmptyTile out of range!");
}

void Maze_t::drop(Tile tile) {
    if (countEmptyTiles() == 0) {
        return;
    }

    nthEmptyTile(rand() % countEmptyTiles()) = tile;
}

void Maze_t::findPlayer() {
    for (size_t y = 0; y < MAP_SIZE; y++) {
        for (size_t x = 0; x < MAP_SIZE; x++) {
            if (tiles[y][x] == PLAYER) {
                playerX = x;
                playerY = y;
            }
        }
    }
}

void Maze_t::moveLeft() {
    moveX = -1;
    moveY = 0;
    move();
}

void Maze_t::moveRight() {
    moveX = 1;
    moveY = 0;
    move();
}

void Maze_t::moveUp() {
    moveX = 0;
    moveY = -1;
    move();
}

void Maze_t::moveDown() {
    moveX = 0;
    moveY = 1;
    move();
}

void Maze_t::move() {
    findPlayer();

    if (tiles[playerY + moveY][playerX + moveX] == WALL) {
        return;
    }

    tiles[playerY][playerX] = EMPTY;
    if (tiles[playerY + moveY][playerX + moveX] == COIN) {
        coinsCollected++;
    }
    tiles[playerY + moveY][playerX + moveX] = PLAYER;

    moveX = moveY = 0;
    if (!hasCoin()) {
        generate();
    }
}

SDL_Surface* Maze_t::render() {

    setColors();

    SDL_FreeSurface(surface);
    surface = Surface::create(MAP_SIZE * TILE_SIZE, MAP_SIZE * TILE_SIZE);

    SDL_Rect tileRect;
    tileRect.w = TILE_SIZE - 2;
    tileRect.h = TILE_SIZE - 2;
    for (size_t y = 0; y < MAP_SIZE; y++) {
        for (size_t x = 0; x < MAP_SIZE; x++) {
            tileRect.x = x * TILE_SIZE + 1;
            tileRect.y = y * TILE_SIZE + 1;

            SDL_FillRect(surface, &tileRect, colors[tiles[y][x]]);
        }
    }

    return surface;
}