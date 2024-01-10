#ifndef ROS2HWIF_SYSTEM_H_
#define ROS2WHIF_SYSTEM_H_

#include <memory>
#include <string>
#include <vector>

#include "hardware_interface/base_interface.hpp"
#include "hardware_interface/handle.hpp"
#include "hardware_interface/hardware_info.hpp"
#include "hardware_interface/system_interface.hpp"
#include "hardware_interface/types/hardware_interface_return_values.hpp"
#include "hardware_interface/types/hardware_interface_status_values.hpp"
#include "rclcpp/macros.hpp"
#include "ros2hwif/visibility_control.h"
#include "ros2hwif/wheel.h"
#include "ros2hwif/canopen_comm.h"

namespace ros2hwif
{

class RosSystemHardware
: public hardware_interface::BaseInterface<hardware_interface::SystemInterface>
{

struct Config
{
  std::string front_left_wheel_name = "";
  std::string front_right_wheel_name = "";
  std::string rear_left_wheel_name = "";
  std::string rear_right_wheel_name = "";

  std::string front_left_wheel_node_id = "";
  std::string front_right_wheel_node_id = "";  
  std::string rear_left_wheel_node_id = "";
  std::string rear_right_wheel_node_id = "";

  float enc_counts_per_rev = 0;
};

public:
  RCLCPP_SHARED_PTR_DEFINITIONS(AgvSystemHardware)

  AGV_BASE_PUBLIC
  hardware_interface::return_type configure(const hardware_interface::HardwareInfo & info) override;

  AGV_BASE_PUBLIC
  std::vector<hardware_interface::StateInterface> export_state_interfaces() override;

  AGV_BASE_PUBLIC
  std::vector<hardware_interface::CommandInterface> export_command_interfaces() override;

  AGV_BASE_PUBLIC
  hardware_interface::return_type start() override;

  AGV_BASE_PUBLIC
  hardware_interface::return_type stop() override;

  AGV_BASE_PUBLIC
  hardware_interface::return_type read() override;

  AGV_BASE_PUBLIC
  hardware_interface::return_type write() override;

private:
  std::vector<Wheel> wheels_;
  Wheel wheel_f_l_;
  Wheel wheel_f_r_;
  Wheel wheel_r_l_;
  Wheel wheel_r_r_;

  Config cfg_;
  canopen_comm::CanopenComm canopen_comm_;
};

}  

#endif  
