#ifndef PIDAR_HARDWARE_HARDWARE_INTERFACE_H
#define PIDAR_HARDWARE_HARDWARE_INTERFACE_H

#include <hardware_interface/joint_state_interface.h>
#include <hardware_interface/joint_command_interface.h>
#include <hardware_interface/robot_hw.h>

#include <ros/ros.h>
#include <ros/node_handle.h>
#include <sensor_msgs/JointState.h>
#include <string.h>

#include <BrickPi3/BrickPi3.h>

namespace pidar_hardware {

  class PidarHW : public hardware_interface::RobotHW
  {
  public:
    PidarHW(ros::NodeHandle nh, ros::NodeHandle private_nh, double target_control_freq);
    ~PidarHW();

    void updateJointsFromHardware(const ros::Duration &duration);

    void writeCommandsToHardware();

    void spinLidar();

    void stopLidar();

    void getInfo();


  private:
    void initializeHardware();

    void registerControlInterfaces();

    double dpsToAngular(const int16_t &dps);
    int16_t angularToDps(const double &angular);

    BrickPi3 BP;

    ros::NodeHandle nh_, private_nh_;

    // ROS Control interfaces
    hardware_interface::JointStateInterface joint_state_interface_;
    hardware_interface::VelocityJointInterface velocity_joint_interface_;

    // ROS Parameters
    double wheel_diameter_, max_accel_, max_speed_;

    double polling_timeout_;

    int lidar_pwm_;

    uint8_t left_wheel_motor_port_, right_wheel_motor_port_, lidar_motor_port_;

    struct Joint
    {
      double position;
      double position_offset;
      double velocity;
      double effort;
      double velocity_command;

      Joint() :
              position(0), velocity(0), effort(0), velocity_command(0)
      { }
    };

    Joint left_wheel_, right_wheel_;

    std::string left_wheel_handle_, right_wheel_handle_;

  };

} // namespace pidar_hardware
#endif //PIDAR_HARDWARE_HARDWARE_INTERFACE_H
