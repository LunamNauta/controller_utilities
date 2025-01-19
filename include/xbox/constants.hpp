#ifndef CONTROLLER_UTILITIES_XBOX_CONSTANTS_HEADER
#define CONTROLLER_UTILITIES_XBOX_CONSTANTS_HEADER

#include <unordered_map>
#include <string>

namespace Input{
namespace Xbox{

static constexpr std::size_t MICROSOFT_VENDOR_ID = 1118;
static const std::unordered_map<std::size_t, std::string> CONTROLLER_PRODUCT_IDS({
    //Xbox Series X/S
    {2834, "Xbox Series X/S Controller (model 1914)"},
    //Xbox One
    {746, "Xbox One S Controllervoid set_auto_update(Controller controller);"},
    {739, "Xbox One Elite Controller"},
    {733, "Xbox One Controller (Firmware 2015)"},
    {721, "Xbox One Controller"},
    //Xbox 360
    {654, "Xbox 360 Controller"},
    //Xbox
    {649, "Xbox Controller S"},
    {645, "Xbox Controller S"},
    {514, "Xbox Controller"}
});

enum class WIRED_EVENT_CODE{
    RIGHT_JOYSTICK_X = 4,
    RIGHT_JOYSTICK_Y = 3,
    LEFT_JOYSTICK_X = 1,
    LEFT_JOYSTICK_Y = 0,
    RIGHT_JOYSTICK = 318,
    LEFT_JOYSTICK = 317,
    DPAD_X = 16,
    DPAD_Y = 17,
    START = 315,
    MENU = 314,
    HOME = 316,
    LB = 310,
    RB = 311,
    A = 304,
    B = 305,
    X = 307,
    Y = 308,
    LT = 2,
    RT = 5,  
};

}}

#endif
