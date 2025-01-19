#include "utilities.hpp"

#include <stdexcept>
#include <fstream>

//https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
std::vector<std::string> split_string(const std::string& str, std::string delimiter) {
    std::size_t pos_start = 0, pos_end;
    std::size_t delim_len = delimiter.length();
    std::vector<std::string> result;
    std::string token;
    while ((pos_end = str.find(delimiter, pos_start)) != std::string::npos) {
        token = str.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        result.push_back(token);
    }
    result.push_back (str.substr(pos_start));
    return result;
}

std::vector<input_device> get_input_devices(){
    std::string path = "/proc/bus/input/devices";
    std::ifstream file(path);
    if (!file.is_open()) throw std::runtime_error("");
    std::string line;
    std::vector<input_device> devices;
    input_device device;
    bool found_device = false;
    while (std::getline(file, line)){
        if (line.size() == 0 && found_device){
            devices.push_back(device);
            found_device = false;
        }
        if (line[0] == 'I'){
            std::string bus = line.substr(line.find("Bus=")+4, 4);
            std::string vendor = line.substr(line.find("Vendor=")+7, 4);
            std::string product = line.substr(line.find("Product=")+8, 4);
            std::string version = line.substr(line.find("Version=")+8, 4);
            device.id.bustype = std::stoull(bus, nullptr, 16);
            device.id.vendor = std::stoull(vendor, nullptr, 16);
            device.id.product = std::stoull(product, nullptr, 16);
            device.id.version = std::stoull(product, nullptr, 16);
            found_device = true;
        } else if (line[0] == 'N'){
            std::size_t pos = line.find("Name=")+6;
            std::string name = line.substr(pos, line.size()-pos-1);
            device.name = name;
        } else if (line[0] == 'H'){
            std::size_t pos = line.find("Handlers=")+9;
            device.handlers = split_string(line.substr(pos), " ");
        }
    }
    return devices;
}
