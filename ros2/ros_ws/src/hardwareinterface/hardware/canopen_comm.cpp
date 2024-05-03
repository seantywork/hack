#include "ros2hwif/wheel.h"
#include "ros2hwif/canopen_comm.h"
#include "roscancomm/cancomm.h"

#include <array>
#include <sstream>
#include <iostream>
#include <memory>

#include "rclcpp/rclcpp.hpp"

namespace canopen_comm
{


std::string Command::getReadMessage(std::string node_id) 
{
  return "[" + sequence_ + "]" + " " + node_id + " r " + object_address_;
}

std::string Command::getWriteMessage(std::string node_id, std::string value) 
{
  return "[" + sequence_ + "]" + " " + node_id + " w " + object_address_ + " " + object_data_type_ + " " + value;
}

std::string CanopenComm::sendCanopenMsg(std::string msg) 
{


  RCLCPP_INFO(rclcpp::get_logger("RosSystemHardware"), "sending from ros: "+msg);

  std::string msg_n = msg + "\n";

  char ascii_cmd_in[MAX_CAN_CMD_IN] = {0};

  char ascii_cmd_out[MAX_CAN_CMD_OUT] = {0};

  strncpy(ascii_cmd_in, msg_n.c_str(), MAX_CAN_CMD_IN);

  int ret = 0;
  std::string result;

  if ((ret = CmdGatewayASCII(ascii_cmd_in, ascii_cmd_out)) < 0){

    RCLCPP_INFO(rclcpp::get_logger("RosSystemHardware"), "COMMAND FAILED!!");

  } else {

    result = ascii_cmd_out;

    RCLCPP_INFO(rclcpp::get_logger("RosSystemHardware"), "COMMAND SUCCESS!!");

  }




  return result;
}

int CanopenComm::hexStringToDecimal(std::string str) 
{
    // ex> "[1] 00 10 00 00"

    RCLCPP_INFO(rclcpp::get_logger("RosSystemHardware"), "inside hex: " + str);

    std::stringstream ss;
    for (char ch : str) {
        if (isxdigit(ch)) {
            ss << ch;
        }
    }

    std::string cleanedHexString = ss.str();

    std::string rearrangedHex;
    for (int i = cleanedHexString.length() - 2; i >= 0; i -= 2) {
        rearrangedHex += cleanedHexString.substr(i, 2);
    }

    int decimalValue;
    std::stringstream(rearrangedHex) >> std::hex >> decimalValue;

    return decimalValue;
}


std::string CanopenComm::read(std::string command_name, std::string node_id) 
{
  Command cmd = command_map_[command_name];
  std::string message = cmd.getReadMessage(node_id);
  return sendCanopenMsg(message);
}

std::string CanopenComm::write(std::string command_name, std::string node_id, std::string value)
{
  Command cmd = command_map_[command_name];
  std::string message = cmd.getWriteMessage(node_id, value);
  return sendCanopenMsg(message);
}

void CanopenComm::readEncoderValues(std::vector<Wheel> &wheels) 
{
  for (Wheel &wheel : wheels) {
    std::string encString = read("actual_position", wheel.node_id);
    wheel.enc = hexStringToDecimal(encString);
  }
}

void CanopenComm::writeVelocity(std::vector<Wheel> &wheels) 
{
  for (Wheel &wheel : wheels) {
    write("target_velocity", wheel.node_id, std::to_string(wheel.dec));
  }
}

int CanopenComm::readTestValue(std::string node_id)
{
  return hexStringToDecimal(read("test", node_id));
}

// TODO: Add error handling
void CanopenComm::setMotorEnable(std::string node_id)
{
  Command cmd = command_map_["control_word"];
  std::string msg = cmd.getWriteMessage(node_id, "0x0F");
  sendCanopenMsg(msg);
}

void CanopenComm::setMotorDisable(std::string node_id)
{
  Command cmd = command_map_["control_word"];
  std::string msg = cmd.getWriteMessage(node_id, "0x06");
  sendCanopenMsg(msg);
}

void CanopenComm::clearEncoderValue(std::string node_id)
{
  Command cmd = command_map_["actual_position_clear"];
  std::string msg = cmd.getWriteMessage(node_id, "1");
  sendCanopenMsg(msg);
}


} // namespace canopen_comm
