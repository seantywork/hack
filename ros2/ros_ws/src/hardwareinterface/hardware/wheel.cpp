#include "ros2hwif/wheel.h"

#include <cmath>


Wheel::Wheel(const std::string wheel_name, std::string wheel_node_id, int counts_per_rev)
{
  setup(wheel_name, wheel_node_id, counts_per_rev);
}


void Wheel::setup(const std::string wheel_name, std::string wheel_node_id, int counts_per_rev)
{
  name = wheel_name;
  node_id = wheel_node_id;
  rads_per_count = (2*M_PI)/counts_per_rev;
}

double Wheel::calcEncAngle()
{
  return enc * rads_per_count;
}
