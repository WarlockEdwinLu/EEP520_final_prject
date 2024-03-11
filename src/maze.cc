#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>
#include <algorithm>

const int WIDTH = 20;
const int HEIGHT = 20;
const int CELL_SIZE = 40; // Each cell is 40x40 units, making the grid span from -400 to 400
std::vector<std::vector<bool>> maze(HEIGHT, std::vector<bool>(WIDTH, true));
std::pair<int, int> start, goal;

bool isValid(int x, int y, std::vector<std::vector<bool>>& visited) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT || visited[y][x]) {
        return false;
    }
    int visitedNeighbors = 0;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 || dy == 0) {
                int nx = x + dx, ny = y + dy;
                if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT) {
                    visitedNeighbors += visited[ny][nx];
                }
            }
        }
    }
    return visitedNeighbors < 2;
}

void generateMaze(int x, int y, std::vector<std::vector<bool>>& visited) {
    std::vector<int> dirs = {0, 1, 2, 3};
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(dirs.begin(), dirs.end(), g); // Using std::shuffle with a proper random number generator

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
            maze[ny][nx] = false; // Mark the new cell as part of the maze
            generateMaze(nx, ny, visited);
        }
    }
}

// Adjustment in the selectRandomPoints function (if needed) to use std::shuffle
void selectRandomPoints() {
    std::vector<std::pair<int, int>> freeCells;
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            if (maze[y][x] == false) { // Cell is part of the path
                freeCells.emplace_back(x, y);
            }
        }
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(freeCells.begin(), freeCells.end(), g); // Shuffle the vector to randomize the start and goal points

    start = freeCells.front(); // Assuming the first element as the start
    goal = freeCells.back(); // Assuming the last element as the goal
}

void saveMazeToJson(const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Failed to open " << filename << " for writing.\n";
        return;
    }

    file << "{\n";
    file << "  \"name\": \"Robot World\",\n";
    file << "  \"ip\": \"0.0.0.0\",\n";
    file << "  \"port\": 8765,\n";
    file << "  \"agents\": [\n";
    file << "    {\n";
    file << "      \"definition\": \"defs/wanderer.json\",\n";
    file << "      \"style\": { \"fill\": \"blue\", \"stroke\": \"black\" },\n";
    file << "      \"position\": { \"x\": " << (start.first * CELL_SIZE - 400 + 20) << ", \"y\": " << (start.second * CELL_SIZE - 400 + 20) << ", \"theta\": 0 }\n";
    file << "    }\n";
    file << "  ],\n";
    file << "  \"statics\": [\n";
    // Output walls and the goal as static objects
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            if (maze[y][x]) { // Cell is a wall
                int x1 = x * CELL_SIZE - 400;
                int y1 = y * CELL_SIZE - 400;
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
    int gx1 = goal.first * CELL_SIZE - 400;
    int gy1 = goal.second * CELL_SIZE - 400;
    file << "        { \"x\": " << gx1 << ", \"y\": " << gy1 << " },\n";
    file << "        { \"x\": " << gx1 + CELL_SIZE << ", \"y\": " << gy1 << " },\n";
    file << "        { \"x\": " << gx1 + CELL_SIZE << ", \"y\": " << gy1 + CELL_SIZE << " },\n";
    file << "        { \"x\": " << gx1 << ", \"y\": " << gy1 + CELL_SIZE << " }\n";
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
    std::cout << "Maze grid written to " << filename << std::endl;
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    std::vector<std::vector<bool>> visited(HEIGHT, std::vector<bool>(WIDTH, false));
    generateMaze(0, 0, visited); // Pass visited array to generateMaze
    selectRandomPoints();
    saveMazeToJson("../config.json"); // Saving one level up from the src directory
    writeMazeToFile("maze_grid.txt");
    return 0;
}