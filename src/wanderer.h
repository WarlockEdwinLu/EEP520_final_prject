#ifndef __WANDERER_AGENT__H
#define __WANDERER_AGENT__H

#include "enviro.h"
#include <fstream>
#include <sstream>
#include <deque>

using namespace enviro;

class FollowPath : public State, public AgentInterface {
public:
    void entry(const Event& e) override {
        path.clear();
        load_path("path_coordinates.txt");
        if (!path.empty()) {
            move_to_next();
        }
    }

    void during() override {
        if (!path.empty() && close_to(path.front().first, path.front().second, 5)) {
            path.pop_front();
            move_to_next();
        }
    }

    void exit(const Event& e) override {}

private:
    std::deque<std::pair<double, double>> path;

    void load_path(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            double x, y;
            if (iss >> x >> y) {
                path.emplace_back(x, y);
            }
        }
    }

    void move_to_next() {
        if (!path.empty()) {
            move_toward(path.front().first, path.front().second, 10, 200);
        }
    }

    bool close_to(double x, double y, double tolerance) {
        auto [px, py] = position();
        return (std::hypot(px - x, py - y) < tolerance);
    }
};

class WandererController : public StateMachine, public AgentInterface {
public:
    WandererController() : StateMachine() {
        set_initial(follow_path);
    }
    FollowPath follow_path;
};

class Wanderer : public Agent {
public:
    Wanderer(json spec, World& world) : Agent(spec, world) {
        add_process(controller);
    }
private:
    WandererController controller;
};

DECLARE_INTERFACE(Wanderer)

#endif /* __WANDERER_AGENT__H */
