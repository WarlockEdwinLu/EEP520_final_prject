#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stack>
#include <utility>
#include <sstream>
#include "path.h"

typedef std::pair<int, int> Point;
typedef std::vector<std::vector<int>> Maze;
typedef std::vector<Point> Path;

// Function declarations
Maze readMaze(const std::string& filename);
Point findStart(const Maze& maze);
Path findPathInMaze(const Maze& maze, const Point& start);
void save_coord_path();

// Direction vectors for moving up, down, left, and right
const std::vector<Point> directions{{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

bool isValidMove(const Maze& maze, const Point& point) {
    int rows = maze.size();
    int cols = maze[0].size();
    int x = point.first;
    int y = point.second;

    // Check if the point is within bounds and is not a wall (0)
    return (x >= 0 && x < rows && y >= 0 && y < cols && maze[x][y] != 0);
}

// Recursive utility function for DFS
bool dfs(const Maze& maze, Maze& visited, Path& path, const Point& point) {
    if (!isValidMove(maze, point) || visited[point.first][point.second]) {
        return false;
    }

    // Mark the current cell as visited
    visited[point.first][point.second] = true;
    path.push_back(point); // Add the point to the path

    // Check if we've reached the goal (marked with a 3)
    if (maze[point.first][point.second] == 3) {
        return true; // Path is found
    }

    // Explore the neighbors in the maze
    for (const Point& dir : directions) {
        Point nextPoint(point.first + dir.first, point.second + dir.second);
        if (dfs(maze, visited, path, nextPoint)) {
            return true; // Path is found
        }
    }

    // If the path is not found, backtrack
    path.pop_back();
    return false;
}

Maze readMaze(const std::string& filename) {
    Maze maze;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        std::vector<int> row;
        int value;
        std::istringstream iss(line);

        while (iss >> value) {
            row.push_back(value);
        }

        maze.push_back(row);
    }

    return maze;
}

// Finds the starting point in the maze (marked with 2) and returns its coordinates
Point findStart(const Maze& maze) {
    for (int i = 0; i < maze.size(); ++i) {
        for (int j = 0; j < maze[i].size(); ++j) {
            if (maze[i][j] == 2) {
                return Point(i, j);
            }
        }
    }
    return Point(-1, -1); // If start is not found
}

// Function to find the path in the maze using DFS
Path findPathInMaze(const Maze& maze, const Point& start) {
    Maze visited(maze.size(), std::vector<int>(maze[0].size(), false));
    Path path;
    dfs(maze, visited, path, start);
    return path;
}

std::vector<Point> convertPathToCoordinates(const std::vector<Point>& path) {
    std::vector<Point> coordinates;

    int gridSize = 30;
    int xOffset = -300;
    int yOffset = 300;

    for (const Point& gridPos : path) {
        int x = xOffset + gridPos.second * gridSize + gridSize / 2;
        int y = -(yOffset - gridPos.first * gridSize - gridSize / 2);
        coordinates.push_back({x, y});
    }

    return coordinates;
}

void save_coord_path() {
    // Read the maze from the file
    Maze maze = readMaze("maze_grid.txt");

    // Find the starting point in the maze
    Point start = findStart(maze);

    // Find the path from the start to the goal
    std::vector<Point> path = findPathInMaze(maze, start);

    // Convert the path to coordinates
    std::vector<Point> coordinates = convertPathToCoordinates(path);

    // Open a file to store the coordinates
    std::ofstream file("path_coordinates.txt");
    if (file.is_open()) {
        for (const auto& point : coordinates) {
            file << point.first << " " << point.second << std::endl;
        }
        file.close();
    } else {
        std::cerr << "Unable to open file for writing coordinates." << std::endl;
    }
}
