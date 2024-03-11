#ifndef __WANDERER_AGENT__H
#define __WANDERER_AGENT__H

#include <iostream>
#include <fstream>
#include <sstream>
#include <deque>
#include "enviro.h"

using namespace enviro;

class WandererController : public Process, public AgentInterface {
    public:
    WandererController() : Process(), AgentInterface(), rotating(false) {}

    void init() {
        load_path("src/path_coordinates.txt");
        move_to_next();
    }
    
    void start() {}
    
    void update() {
        if (!path.empty() && close_to(path.front().first, path.front().second, 5)) {
            std::cout << "Current coordinate: (" << path.front().first << ", " << path.front().second << ")\n"; // Print statement
            path.pop_front();
        }
        move_to_next();
    }
    
    void stop() {}

    private:
    std::deque<std::pair<double, double>> path;
    bool rotating;

    void load_path(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            double x, y;
            if (iss >> x >> y) {
                path.push_back({x, y});
            }
        }
    }

    void move_to_next() {
        if (!path.empty()) {
            auto target = path.front();
            double dx = target.first - position().x;
            double dy = target.second - position().y;
            double target_angle = atan2(dy, dx);
            double current_angle = angle();
            double angle_difference = angle_diff_rad(current_angle, target_angle);

            // Debugging output
            // std::cout << "Current position: (" << position().x << ", " << position().y << ")\n";
            // std::cout << "Target position: (" << target.first << ", " << target.second << ")\n";
            // std::cout << "Current angle: " << current_angle << ", Target angle: " << target_angle << "\n";
            // std::cout << "Angle difference: " << angle_difference << ", Rotating: " << rotating << "\n";

            if (!rotating) {
                if (std::abs(angle_difference) > 0.1) { // if the robot is not facing the target direction
                    track_velocity(0, angle_difference * 10); // rotate to face the target
                    rotating = true;
                } else {
                    rotating = false;
                move_toward(target.first, target.second, 10, 30); // move towards the target
                }
            } else {
                if (std::abs(angle_difference) < 0.1) {
                    // If the angle is close enough to the target angle, stop rotating and move forward
                    rotating = false;
                }
                // Continue rotation until we are facing close enough to the target angle
                track_velocity(0, angle_difference * 10); // adjust the multiplier as necessary
            }
        }
    }



    bool close_to(double x, double y, double tolerance) {
        auto [px, py] = position();
        return (std::hypot(px - x, py - y) < tolerance);
    }

    double angle_diff_rad(double a1, double a2) {
        double diff = fmod(a2 - a1, 2 * M_PI);
        if (diff < -M_PI)
            diff += 2 * M_PI;
        if (diff > M_PI)
            diff -= 2 * M_PI;
        return diff;
    }
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
