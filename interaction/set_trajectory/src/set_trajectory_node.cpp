#include "ros/ros.h"
#include "sensor_msgs/Imu.h"
#include "trajectory_msgs/JointTrajectory.h"

//Initial orientation along Y and Z axis. Using Reference variables just
//incase it needs to be changed.
float orientY = 0;
float& oYRef = orientY;

float orientZ = 0;
float& oZRef = orientZ;

//Initialize orientCmd JointTrajectory message object.
trajectory_msgs::JointTrajectory orientCmd;

//To populate the seq field in the JointTrajectory message
int seq = 0;

//Callback function for reading orientation values from Imu msg.
void readimuCallback(const sensor_msgs::Imu::ConstPtr& msg)
{
  orientY = msg->orientation.y;
  orientZ = msg->orientation.z;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "set_trajectory");

  ros::NodeHandle imuMsgNodeH;

  //Subscribe to the imu/data topic.
  ros::Subscriber sub = imuMsgNodeH.subscribe("imu/data", 100, readimuCallback);

  ros::NodeHandle mvKai;
  //Publisher to publish JointTrajectory messages to /kai_controller/command
  //topic.
  ros::Publisher kPub = mvKai.advertise<trajectory_msgs::JointTrajectory>("kai_controller/command", 100);
  ros::Rate loop_rate(0.1);

  //Add joint names to the trajectory_msg
  orientCmd.joint_names.push_back("base_pole_joint");
  orientCmd.joint_names.push_back("pole_arm_joint");
 
  //Update trajectory points till failure.
  while(mvKai.ok()) {
    seq++;
    
    //Set header values.
    orientCmd.header.seq = seq;
    orientCmd.header.stamp = ros::Time::now();
    
    //Set trajecory points.
    orientCmd.points.resize(1);
    orientCmd.points[0].positions.resize(2);
    orientCmd.points[0].positions[0] = oYRef;
    orientCmd.points[0].positions[1] = oZRef;
    //
    orientCmd.points[0].time_from_start = ros::Duration(0.01);
    
    kPub.publish(orientCmd);
    ros::spinOnce();
  }
}
