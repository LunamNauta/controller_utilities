#ifndef CONTROLLER_UTILITIES_XBOX_CONSTANTS_HEADER
#define CONTROLLER_UTILITIES_XBOX_CONSTANTS_HEADER

#include <unordered_map>
#include <string>
#include <vector>

namespace Input{
namespace Xbox{

enum class ConsoleType{
    XBOX_ONE_S = 0,
    XBOX_ONE,
    XBOX_360,
    XBOX
};
enum class ControllerType{
    WIRED = 0,
    BLUETOOTH
};

enum class ControllerInput{
    RIGHT_JOYSTICK_X = 0,
    RIGHT_JOYSTICK_Y,
    LEFT_JOYSTICK_X,
    LEFT_JOYSTICK_Y,
    RIGHT_JOYSTICK,
    LEFT_JOYSTICK,
    DPAD_X,
    DPAD_Y,
    START,
    MENU,
    HOME,
    LB,
    RB,
    A,
    B,
    X,
    Y,
    LT,
    RT,  
};

enum class DPad{
    RIGHT = 0,
    LEFT,
    DOWN, 
    UP,
    NONE
};

static constexpr std::size_t MICROSOFT_VENDOR_ID = 1118;
static const std::unordered_map<std::size_t, std::tuple<ConsoleType, ControllerType, std::string>> CONTROLLER_PRODUCT_IDS({
    //Xbox Series X/S
    //{2834, "Xbox Series X/S Controller (model 1914)"},

    //Xbox One S
    {736, {ConsoleType::XBOX_ONE_S, ControllerType::BLUETOOTH, "Xbox Wireless Controller"}},
    
    //Xbox One
    {765, {ConsoleType::XBOX_ONE, ControllerType::BLUETOOTH, "Xbox Wireless Controller"}},
    {746, {ConsoleType::XBOX_ONE, ControllerType::WIRED, "Xbox One S Controller"}},
    {739, {ConsoleType::XBOX_ONE, ControllerType::WIRED, "Xbox One Elite Controller"}},
    {733, {ConsoleType::XBOX_ONE, ControllerType::WIRED, "Xbox One Controller (Firmware 2015)"}},
    {721, {ConsoleType::XBOX_ONE, ControllerType::WIRED, "Xbox One Controller"}},
    //Xbox 360
    {654, {ConsoleType::XBOX_360, ControllerType::WIRED, "Xbox 360 Controller"}},
    //Xbox
    {649, {ConsoleType::XBOX, ControllerType::WIRED, "Xbox Controller S"}},
    {645, {ConsoleType::XBOX, ControllerType::WIRED, "Xbox Controller S"}},
    {514, {ConsoleType::XBOX, ControllerType::WIRED, "Xbox Controller"}}
});

const std::vector<std::vector<std::vector<std::size_t>>> EVENT_CODES({
    { // Wired controllers
        {}, // Xbox One S
        {3, 4, 0, 1, 318, 317, 16, 17, 315, 314, 316, 310, 311, 304, 305, 307, 308, 2, 5}, // Xbox One
        {}, // Xbox 360
        {}  // Xbox
    },
    { // Bluetooth controllers,
        {2, 5, 0, 1, 318, 317, 16, 17, 315, 158, 316, 310, 311, 304, 305, 307, 308, 10, 9}, // Xbox One S
        {2, 5, 0, 1, 318, 317, 16, 17, 315, 158, 172, 310, 311, 304, 305, 307, 308, 10, 9}, // Xbox One
        {}, // Xbox 360
        {}  // Xbox
    }
});

}}

#endif
