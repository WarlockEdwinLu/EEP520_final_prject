#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>
#include <algorithm>
#include "maze.h"
#include "path.h"

int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    std::vector<std::vector<bool>> visited(HEIGHT, std::vector<bool>(WIDTH, false));
    generateMaze(0, 0, visited); // Pass visited array to generateMaze
    selectDistantGoal(); // Select start and goal points after maze generation
    writeMazeToFile("maze_grid.txt"); // Write the maze to a text file
    saveMazeToJson("../config.json"); // Saving maze configuration to 'config.json' one level up from the current directory
    save_coord_path();
    return 0;
}