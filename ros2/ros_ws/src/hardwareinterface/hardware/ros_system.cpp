#include "ros2hwif/ros_system.h"
#include "ros2hwif/canopen_comm.h"
#include "roscancomm/cancomm.h"

#include <chrono>
#include <cmath>
#include <limits>
#include <memory>
#include <vector>

#include "hardware_interface/types/hardware_interface_type_values.hpp"
#include "rclcpp/rclcpp.hpp"

namespace ros2hwif
{


hardware_interface::return_type RosSystemHardware::configure(
  const hardware_interface::HardwareInfo & info)
{
  if (configure_default(info) != hardware_interface::return_type::OK)
  {
    return hardware_interface::return_type::ERROR;
  }

  cfg_.front_left_wheel_name = info_.hardware_parameters["front_left_wheel_name"];
  cfg_.front_right_wheel_name = info_.hardware_parameters["front_right_wheel_name"];
  cfg_.rear_left_wheel_name = info_.hardware_parameters["rear_left_wheel_name"];
  cfg_.rear_right_wheel_name = info_.hardware_parameters["rear_right_wheel_name"];

  cfg_.front_left_wheel_node_id = info_.hardware_parameters["front_left_wheel_node_id"];
  cfg_.front_right_wheel_node_id = info_.hardware_parameters["front_right_wheel_node_id"];
  cfg_.rear_left_wheel_node_id = info_.hardware_parameters["rear_left_wheel_node_id"];
  cfg_.rear_right_wheel_node_id = info_.hardware_parameters["rear_right_wheel_node_id"];

  cfg_.enc_counts_per_rev = std::stoi(info_.hardware_parameters["enc_counts_per_rev"]);

  std::string can_dev_name = info_.hardware_parameters["can_dev_name"];
  std::string can_node_id = info_.hardware_parameters["can_node_id"];

  wheel_f_l_.setup(cfg_.front_left_wheel_name, cfg_.front_left_wheel_node_id, cfg_.enc_counts_per_rev);
  wheel_f_r_.setup(cfg_.front_right_wheel_name, cfg_.front_right_wheel_node_id, cfg_.enc_counts_per_rev);
  wheel_r_l_.setup(cfg_.rear_left_wheel_name, cfg_.rear_left_wheel_node_id, cfg_.enc_counts_per_rev);
  wheel_r_r_.setup(cfg_.rear_right_wheel_name, cfg_.rear_right_wheel_node_id, cfg_.enc_counts_per_rev);

  wheels_.push_back(wheel_f_l_);
  wheels_.push_back(wheel_f_r_);
  wheels_.push_back(wheel_r_l_);
  wheels_.push_back(wheel_r_r_);

  // TODO: uncomment below codes after attaching ros to the real robot
  /*
  for (Wheel &wheel : wheels_) {
    canopen_comm_.setMotorDisable(wheel.node_id);
    canopen_comm_.clearEncoderValue(wheel.node_id);
    canopen_comm_.setMotorEnable(wheel.node_id);
  }
  */



  char candevnm[MAX_CAN_DEV_NAME] = {0};

  strncpy(candevnm, can_dev_name.c_str(), MAX_CAN_DEV_NAME);

  uint8_t cannid = (uint8_t)stoi(can_node_id);

  int ret = InitCanComm(candevnm, cannid);

  if(ret < 0){
    RCLCPP_INFO(rclcpp::get_logger("RosSystemHardware"), "InitCanComm error:" + std::to_string(ret));
  }else{
    RCLCPP_INFO(rclcpp::get_logger("RosSystemHardware"), "InitCanComm success");
  }

  
  ret = InitCmdGateway();

  if(ret < 0){
    RCLCPP_INFO(rclcpp::get_logger("RosSystemHardware"), "InitCmdGateway error:" + std::to_string(ret));
  }else{
    RCLCPP_INFO(rclcpp::get_logger("RosSystemHardware"), "InitCmdGateway success");
  }

 


  status_ = hardware_interface::status::CONFIGURED;
  RCLCPP_INFO(rclcpp::get_logger("RosSystemHardware"), "System Successfully configured!");

  return hardware_interface::return_type::OK;
}

std::vector<hardware_interface::StateInterface> RosSystemHardware::export_state_interfaces()
{
  std::vector<hardware_interface::StateInterface> state_interfaces;

  for(Wheel &wheel : wheels_) {
    state_interfaces.emplace_back(hardware_interface::StateInterface(wheel.name, hardware_interface::HW_IF_POSITION, &wheel.pos));
    state_interfaces.emplace_back(hardware_interface::StateInterface(wheel.name, hardware_interface::HW_IF_VELOCITY, &wheel.vel));
  }

  return state_interfaces;
}

std::vector<hardware_interface::CommandInterface> RosSystemHardware::export_command_interfaces()
{
  std::vector<hardware_interface::CommandInterface> command_interfaces;

  for(Wheel &wheel : wheels_) {
    command_interfaces.emplace_back(hardware_interface::CommandInterface(wheel.name, hardware_interface::HW_IF_VELOCITY, &wheel.cmd));
  }

  return command_interfaces;
}

hardware_interface::return_type RosSystemHardware::start()
{


  for (Wheel &wheel : wheels_) {
  // TODO: uncomment below code after attaching ros to the real robot
    // canopen_comm_.readEncoderValues(control_wheels_); // wheel.pos = 0
    wheel.cmd = 0;
    wheel.vel = 0;
    wheel.last_read_time = std::chrono::high_resolution_clock::now();
  }

  status_ = hardware_interface::status::STARTED;
  RCLCPP_INFO(rclcpp::get_logger("RosSystemHardware"), "System Successfully started!");

  return hardware_interface::return_type::OK;
}

hardware_interface::return_type RosSystemHardware::stop()
{
  status_ = hardware_interface::status::STOPPED;
  RCLCPP_INFO(rclcpp::get_logger("RosSystemHardware"), "System successfully stopped!");

  return hardware_interface::return_type::OK;
}

hardware_interface::return_type RosSystemHardware::read()
{
  RCLCPP_INFO(rclcpp::get_logger("RosSystemHardware"), "Reading...");

  // TODO: uncomment below codes after attaching ros to the real robot
  /*
  canopen_comm_.readEncoderValues(wheels_);

  // At this point, just have read wheel_enc values, 
  // not have calculated and set wheel_pos from encoder values yet
  for (Wheel &wheel : wheels_) {
    double pos_prev = wheel.pos;
    wheel.pos = wheel.calcEncAngle();

    auto time_point_prev = wheel.last_read_time;
    auto time_point_now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_point_now - time_point_prev);

    RCLCPP_INFO(rclcpp::get_logger("RosSystemHardware"), "read duration: %d ms", duration.count());

    // wheel.vel = (wheel.pos - pos_prev) / (static_cast<double>(duration.count()) / 1000);

    wheel.last_read_time = time_point_now;
  }
  */

  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ temp ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  // for canopen communication test. TODO: delete after attaching the ROS to real robot
  int test_value = canopen_comm_.readTestValue("99");
  RCLCPP_INFO(rclcpp::get_logger("RosSystemHardware"), "Test comm result: %d", test_value);

  // The default hardware_interface control loop frequency is 10Hz
  double dt = 0.1;      // Control period

  for (Wheel &wheel : wheels_) {
    wheel.pos = wheel.pos + dt * wheel.cmd;
    wheel.vel = wheel.cmd;

    RCLCPP_INFO(
      rclcpp::get_logger("RosSystemHardware"),
      "Got position state %.5f and velocity state %.5f for '%s'!", 
      wheel.pos, wheel.vel, wheel.name.c_str()
    );
  }

  // vvvvvvvvvvvvvvvvvvvvvvvvvvvvv temp vvvvvvvvvvvvvvvvvvvvvvvvvvvvv

  return hardware_interface::return_type::OK;
}

hardware_interface::return_type ros2hwif::RosSystemHardware::write()
{

  RCLCPP_INFO(rclcpp::get_logger("RosSystemHardware"), "Writing...");

  /**
   * ============================================
   * |        Unit conversion formulas          |
   * ============================================
   * 1 [rad/s] = (30 / pi) [rpm]
   * 1 [rpm] = (pi / 30) [rad/s]
   * y [dec] = x [rpm] * 512 * 4096 / 1875
   * y [dec] = x [rad/s] * (pi / 30) * 512 * 4096 / 1875
  */

   for (Wheel &wheel : wheels_) {
    RCLCPP_INFO(
      rclcpp::get_logger("RosSystemHardware"), "Got command %.5f for '%s'!", 
      wheel.cmd, wheel.name.c_str()
    );

  // TODO: uncomment below codes after attaching ros to the real robot
    // cmd[rad/s] to target_velocity[dec]
    // wheel.dec = static_cast<int>(wheel.cmd * M_PI / 30 * 512 * 4096 / 1875);
  }

  // canopen_comm_.writeVelocity(wheels_);
  

  RCLCPP_INFO(rclcpp::get_logger("RosSystemHardware"), "Joints successfully written!");

  return hardware_interface::return_type::OK;
}

}  // namespace ros2hwif

#include "pluginlib/class_list_macros.hpp"
PLUGINLIB_EXPORT_CLASS(
  ros2hwif::RosSystemHardware, hardware_interface::SystemInterface
)
