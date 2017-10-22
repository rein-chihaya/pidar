#include <pidar_hardware/hardware_interface.h>

const double pi = 3.14159265359;

static uint8_t nxtPortStringToInt(const std::string &portString)
{
  uint8_t port = 0;
  if(portString == "A")
  {
    port = PORT_A;
  }
  else if(portString == "B")
  {
    port = PORT_B;
  }
  else if(portString == "C")
  {
    port = PORT_C;
  }
  else if(portString == "D")
  {
    port = PORT_D;
  }

  return port;
}

namespace pidar_hardware {

  PidarHW::PidarHW(ros::NodeHandle nh, ros::NodeHandle private_nh, double target_control_freq)
  :
  nh_(nh),
  private_nh_(private_nh),
  wheel_diameter_(0.0),
  max_accel_(0.0),
  max_speed_(0.0)
  {
    // Retrieve parameters
    private_nh_.param("wheel_diameter", wheel_diameter_, wheel_diameter_);
    private_nh_.param("max_accel", max_accel_, max_accel_);
    private_nh_.param("max_speed", max_speed_, max_speed_);

    // Set the NXT ports for the motors
    std::string left_wheel_port_str = "A";
    std::string right_wheel_port_str = "B";
    private_nh_.param("left_wheel_motor_port", left_wheel_port_str, left_wheel_port_str);
    private_nh_.param("right_wheel_motor_port", right_wheel_port_str, right_wheel_port_str);
    left_wheel_motor_port_ = nxtPortStringToInt(left_wheel_port_str);
    right_wheel_motor_port_ = nxtPortStringToInt(right_wheel_port_str);

    initializeHardware();
    registerControlInterfaces();
  }

  PidarHW::~PidarHW()
  {
    BP.reset_all();
  }

  void PidarHW::initializeHardware()
  {
    BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.

    // Reset the encoders
    BP.offset_motor_encoder(PORT_A, BP.get_motor_encoder(PORT_A));
    BP.offset_motor_encoder(PORT_B, BP.get_motor_encoder(PORT_B));
    BP.offset_motor_encoder(PORT_C, BP.get_motor_encoder(PORT_C));
    BP.offset_motor_encoder(PORT_D, BP.get_motor_encoder(PORT_D));
  }

  void PidarHW::registerControlInterfaces()
  {
    // Initialize and register left wheel handles
    hardware_interface::JointStateHandle left_wheel_jsh("left_wheel",
                                                              &left_wheel_.position, &left_wheel_.velocity,
                                                              &left_wheel_.effort);
    joint_state_interface_.registerHandle(left_wheel_jsh);

    hardware_interface::JointHandle left_wheel_jh(left_wheel_jsh,
                                                  &left_wheel_.velocity_command);
    velocity_joint_interface_.registerHandle(left_wheel_jh);

    // Initialize and register right wheel handles
    hardware_interface::JointStateHandle right_wheel_jsh("right_wheel",
                                                        &right_wheel_.position, &right_wheel_.velocity,
                                                        &right_wheel_.effort);
    joint_state_interface_.registerHandle(right_wheel_jsh);

    hardware_interface::JointHandle right_wheel_jh(right_wheel_jsh,
                                                  &right_wheel_.velocity_command);
    velocity_joint_interface_.registerHandle(right_wheel_jh);

    registerInterface(&joint_state_interface_);
    registerInterface(&velocity_joint_interface_);
  }


  void PidarHW::updateJointsFromHardware()
  {

  }

  void PidarHW::writeCommandsToHardware()
  {
    BP.set_motor_dps(left_wheel_motor_port_, angularToDps(left_wheel_.velocity_command));
    BP.set_motor_dps(right_wheel_motor_port_, angularToDps(right_wheel_.velocity_command));
  }

  double PidarHW::dpsToAngular(const int16_t &dps)
  {
    const double encoderWheelTicks = 360.0;
    return (2 * pi * dps) / encoderWheelTicks;
  }

  int16_t PidarHW::angularToDps(const double &angular)
  {
    const double encoderWheelTicks = 360.0;
    return static_cast<int16_t>((encoderWheelTicks * angular) / (2*pi));
  }

  void PidarHW::getInfo()
  {
    BP.detect();
    char str[33];
    BP.get_id(str);
    printf("Serial Number   : %s\n", str);
  }

}