#ifndef ROS2HWIF_CANOPEN_COMM_H
#define ROS2HWIF_CANOPEN_COMM_H

#include <string>
#include <map>
#include <vector>

namespace canopen_comm 
{


class Command {
    public:
        Command() = default;
        Command(
            std::string object_address, 
            std::string object_data_type,
            std::string sequence
        ) {
            object_address_ = object_address;
            object_data_type_ = object_data_type;
            sequence_ = sequence;
        }

        std::string getReadMessage(std::string node_id);
        std::string getWriteMessage(std::string node_id, std::string value);

    private:
        std::string object_address_;
        std::string object_data_type_;
        std::string sequence_;
};

class CanopenComm {
    public:
        CanopenComm() {
            // 0x0F Enable, 0x06 Disable, 0x86 Clear error
            command_map_["control_word"] = Command("0x6040 0", "i16", "101");

            command_map_["actual_position"] = Command("0x6063 0", "i32", "201");

            // set 1 to clear "Actual position" when motor is disabled
            command_map_["actual_position_clear"] = Command("0x607C 2", "u8", "202");

            // y [dec] = x [rpm] * 512 * 4096 / 1875
            command_map_["target_velocity"] = Command("0x60FF 0", "i32", "301");

            // y [dec] = x [rps/s] * 256 * 4096 / 15625
            command_map_["acceleration"] = Command("0x6083 0", "u32", "401");
            command_map_["deceleration"] = Command("0x6084 0", "u32", "402");

            // for test (available on virtual can node of canopend)
            command_map_["test"] = Command("0x1017 0", "u16", "999");
        }

        void readEncoderValues(std::vector<Wheel> &wheels);
        void writeVelocity(std::vector<Wheel> &wheels);
        int readTestValue(std::string node_id);
        void setMotorEnable(std::string node_id);
        void setMotorDisable(std::string node_id);
        void clearEncoderValue(std::string node_id);

    private:
        int node_id_;
        std::map<std::string, Command> command_map_;
        std::string sendCanopenMsg(std::string msg);
        int hexStringToDecimal(std::string str);
        std::string read(std::string command_name, std::string node_id);
        std::string write(std::string command_name, std::string node_id, std::string value);
};


} 

#endif 