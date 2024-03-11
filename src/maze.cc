#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>
#include <algorithm>
#include "maze.h"

// Function declarations
bool isValid(int x, int y, std::vector<std::vector<bool>>& visited);
void generateMaze(int x, int y, std::vector<std::vector<bool>>& visited);
void selectDistantGoal();
void saveMazeToJson(const std::string& filename);
void writeMazeToFile(const std::string& filename);

const int CELL_SIZE = 30; // Each cell is 30x30 units, making the grid span from -300 to 300
std::vector<std::vector<bool>> maze(HEIGHT, std::vector<bool>(WIDTH, true));
std::pair<int, int> start, goal;

bool isValid(int x, int y, std::vector<std::vector<bool>>& visited) {
    // Check for bounds and if cell is already visited
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT || visited[y][x]) {
        return false;
    }
    int visitedNeighbors = 0;
    for (int dx = -1; dx <= 1; dx += 2) { // Only check left and right neighbors
        int nx = x + dx;
        if (nx >= 0 && nx < WIDTH && visited[y][nx]) {
            visitedNeighbors++;
        }
    }
    for (int dy = -1; dy <= 1; dy += 2) { // Only check top and bottom neighbors
        int ny = y + dy;
        if (ny >= 0 && ny < HEIGHT && visited[ny][x]) {
            visitedNeighbors++;
        }
    }
    return visitedNeighbors == 1;
}

void generateMaze(int x, int y, std::vector<std::vector<bool>>& visited) {
    std::vector<int> dirs = {0, 1, 2, 3};
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(dirs.begin(), dirs.end(), g);

    visited[y][x] = true;

    for (int& dir : dirs) {
        int nx = x, ny = y;
        switch (dir) {
            case 0: ny -= 1; break; // Up
            case 1: nx += 1; break; // Right
            case 2: ny += 1; break; // Down
            case 3: nx -= 1; break; // Left
        }
        if (isValid(nx, ny, visited)) {
            // Carve a path to the new cell
            maze[y][x] = false;
            maze[ny][nx] = false;
            generateMaze(nx, ny, visited);
        }
    }
}

void selectDistantGoal() {
    std::vector<std::pair<int, int>> freeCells;
    // Find all cells that are part of the path
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            if (!maze[y][x]) {
                freeCells.emplace_back(x, y);
            }
        }
    }

    // Shuffle to randomize the starting cell
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(freeCells.begin(), freeCells.end(), g);
    start = freeCells.front();

    // No need to shuffle again, just pick a cell far enough from the start
    int maxDistanceIndex = 0;
    double maxDistance = 0;
    for (size_t i = 0; i < freeCells.size(); ++i) {
        double distance = std::sqrt(std::pow(freeCells[i].first - start.first, 2) + 
                                    std::pow(freeCells[i].second - start.second, 2));
        if (distance > maxDistance) {
            maxDistance = distance;
            maxDistanceIndex = i;
        }
    }
    goal = freeCells[maxDistanceIndex];
}

void saveMazeToJson(const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Failed to open " << filename << " for writing.\n";
        return;
    }

    file << "{\n";
    file << "  \"name\": \"Maze\",\n";
    file << "  \"ip\": \"0.0.0.0\",\n";
    file << "  \"port\": 8765,\n";
    file << "  \"agents\": [\n";
    file << "    {\n";
    file << "      \"definition\": \"defs/wanderer.json\",\n";
    file << "      \"style\": { \"fill\": \"blue\", \"stroke\": \"black\" },\n";
    file << "      \"position\": { \"x\": " << (start.first * CELL_SIZE - 300 + 15) << ", \"y\": " << (start.second * CELL_SIZE - 300 + 15) << ", \"theta\": 0 }\n";
    file << "    }\n";
    file << "  ],\n";
    file << "  \"statics\": [\n";
    // Output walls and the goal as static objects
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            if (maze[y][x]) { // Cell is a wall
                int x1 = x * CELL_SIZE - 300;
                int y1 = y * CELL_SIZE - 300;
                file << "    {\n";
                file << "      \"style\": { \"fill\": \"gray\", \"stroke\": \"none\" },\n";
                file << "      \"shape\": [\n";
                file << "        { \"x\": " << x1 << ", \"y\": " << y1 << " },\n";
                file << "        { \"x\": " << x1 + CELL_SIZE << ", \"y\": " << y1 << " },\n";
                file << "        { \"x\": " << x1 + CELL_SIZE << ", \"y\": " << y1 + CELL_SIZE << " },\n";
                file << "        { \"x\": " << x1 << ", \"y\": " << y1 + CELL_SIZE << " }\n";
                file << "      ]\n";
                file << "    },\n";
            }
        }
    }
    // Add the goal as a static object with a distinct style
    file << "    {\n";
    file << "      \"style\": { \"fill\": \"gold\", \"stroke\": \"black\" },\n";
    file << "      \"shape\": [\n";
    int gx1 = goal.first * CELL_SIZE - 300;
    int gy1 = goal.second * CELL_SIZE - 300;
    file << "        { \"x\": " << gx1 << ", \"y\": " << gy1 << " },\n";
    file << "        { \"x\": " << gx1 + CELL_SIZE << ", \"y\": " << gy1 << " },\n";
    file << "        { \"x\": " << gx1 + CELL_SIZE << ", \"y\": " << gy1 + CELL_SIZE << " },\n";
    file << "        { \"x\": " << gx1 << ", \"y\": " << gy1 + CELL_SIZE << " }\n";
    file << "      ]\n";
    file << "    },\n";

    // top bound
    file << "    {\n";
    file << "      \"style\": { \"fill\": \"black\", \"stroke\": \"none\" },\n";
    file << "      \"shape\": [\n";
    file << "        { \"x\": " << -300 << ", \"y\": " << -330 << " },\n";
    file << "        { \"x\": " << 300 << ", \"y\": " << -330 << " },\n";
    file << "        { \"x\": " << 300 << ", \"y\": " << -300 << " },\n";
    file << "        { \"x\": " << -300 << ", \"y\": " << -300 << " }\n";
    file << "      ]\n";
    file << "    },\n";

    // bottom bound
    file << "    {\n";
    file << "      \"style\": { \"fill\": \"black\", \"stroke\": \"none\" },\n";
    file << "      \"shape\": [\n";
    file << "        { \"x\": " << -300 << ", \"y\": " << 300 << " },\n";
    file << "        { \"x\": " << 300 << ", \"y\": " << 300 << " },\n";
    file << "        { \"x\": " << 300 << ", \"y\": " << 330 << " },\n";
    file << "        { \"x\": " << -300 << ", \"y\": " << 330 << " }\n";
    file << "      ]\n";
    file << "    },\n";

    // left bound
    file << "    {\n";
    file << "      \"style\": { \"fill\": \"black\", \"stroke\": \"none\" },\n";
    file << "      \"shape\": [\n";
    file << "        { \"x\": " << -330 << ", \"y\": " << -330 << " },\n";
    file << "        { \"x\": " << -300 << ", \"y\": " << -330 << " },\n";
    file << "        { \"x\": " << -300 << ", \"y\": " << 330 << " },\n";
    file << "        { \"x\": " << -330 << ", \"y\": " << 330 << " }\n";
    file << "      ]\n";
    file << "    },\n";

    // right bound
    file << "    {\n";
    file << "      \"style\": { \"fill\": \"black\", \"stroke\": \"none\" },\n";
    file << "      \"shape\": [\n";
    file << "        { \"x\": " << 300 << ", \"y\": " << -330 << " },\n";
    file << "        { \"x\": " << 330 << ", \"y\": " << -330 << " },\n";
    file << "        { \"x\": " << 330 << ", \"y\": " << 330 << " },\n";
    file << "        { \"x\": " << 300 << ", \"y\": " << 330 << " }\n";
    file << "      ]\n";
    file << "    }\n";

    file << "  ]\n";
    file << "}\n";
    file.close();
}

void writeMazeToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Failed to open " << filename << " for writing.\n";
        return;
    }

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            // Mark the cell as path (0) or wall (1)
            if (y == start.second && x == start.first) {
                file << "2 "; // Start marker, if you decide to mark it in the grid
            } else if (y == goal.second && x == goal.first) {
                file << "3 "; // Goal marker, same as above
            } else {
                file << (maze[y][x] ? "0" : "1") << " ";
            }
        }
        file << "\n";
    }

    file.close();
}
