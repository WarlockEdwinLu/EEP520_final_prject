#ifndef MAZE_H
#define MAZE_H

#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>
#include <algorithm>

const int WIDTH = 20;
const int HEIGHT = 20;

void generateMaze(int x, int y, std::vector<std::vector<bool>>& visited);
void selectDistantGoal();
void saveMazeToJson(const std::string& filename);
void writeMazeToFile(const std::string& filename);

#endif // MAZE_H