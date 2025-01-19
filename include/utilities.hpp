#ifndef CONTROLLER_UTILITIES_UTILITIES_HEADER
#define CONTROLLER_UTILITIES_UTILITIES_HEADER

#include <linux/input.h>

#include <vector>
#include <string>

struct input_device{
    input_id id;
    std::string name;
    std::vector<std::string> handlers;
    input_device() : id({0, 0, 0, 0}), name(""), handlers({}){}
};

std::vector<std::string> split_string(const std::string& str, std::string delimiter);

std::vector<input_device> get_input_devices();

#endif
