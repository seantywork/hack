#ifndef ROS2HWIF_WHEEL_H
#define ROS2HWIF_WHEEL_H

#include <string>
#include <chrono>


class Wheel
{
    public:

    std::string name = "";
    std::string node_id = "";
    int enc = 0;
    double cmd = 0;
    double pos = 0;
    double vel = 0;
    double dec = 0;
    double rads_per_count = 0;
    std::chrono::high_resolution_clock::time_point last_read_time;

    Wheel() = default;

    Wheel(const std::string wheel_name, std::string node_id, int counts_per_rev);

    void setup(const std::string wheel_name, std::string node_id, int counts_per_rev);

    double calcEncAngle();
};


#endif 
