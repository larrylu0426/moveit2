/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2012, Willow Garage, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Willow Garage nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

/* Author: Ioan Sucan */

#include <moveit/trajectory_execution_manager/trajectory_execution_manager.h>
#include <moveit/robot_model_loader/robot_model_loader.h>
#include <moveit/planning_scene_monitor/planning_scene_monitor.h>

static const rclcpp::Logger LOGGER = rclcpp::get_logger("moveit_ros.trajectory_execution_manager.test_app");

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("test_trajectory_execution_manager");

  auto rml = std::make_shared<robot_model_loader::RobotModelLoader>(node);
  planning_scene_monitor::PlanningSceneMonitor psm(node, rml);
  trajectory_execution_manager::TrajectoryExecutionManager tem(node, rml->getModel(), psm.getStateMonitor(), true);

  std::cout << "1:\n";
  if (!tem.ensureActiveControllersForJoints(std::vector<std::string>(1, "basej")))
    RCLCPP_ERROR(LOGGER, "Fail!");

  std::cout << "2:\n";
  if (!tem.ensureActiveController("arms"))
    RCLCPP_ERROR(LOGGER, "Fail!");

  std::cout << "3:\n";
  if (!tem.ensureActiveControllersForJoints(std::vector<std::string>(1, "rj2")))
    RCLCPP_ERROR(LOGGER, "Fail!");

  std::cout << "4:\n";
  if (!tem.ensureActiveControllersForJoints(std::vector<std::string>(1, "lj1")))
    RCLCPP_ERROR(LOGGER, "Fail!");

  std::cout << "5:\n";
  if (!tem.ensureActiveController("left_arm_head"))
    RCLCPP_ERROR(LOGGER, "Fail!");
  std::cout << "6:\n";
  if (!tem.ensureActiveController("arms"))
    RCLCPP_ERROR(LOGGER, "Fail!");

  // execute with empty set of trajectories
  tem.execute();
  if (!tem.waitForExecution())
    RCLCPP_ERROR(LOGGER, "Fail!");

  moveit_msgs::msg::RobotTrajectory traj1;
  traj1.joint_trajectory.joint_names.push_back("rj1");
  traj1.joint_trajectory.points.resize(1);
  traj1.joint_trajectory.points[0].positions.push_back(0.0);
  if (!tem.push(traj1))
    RCLCPP_ERROR(LOGGER, "Fail!");

  moveit_msgs::msg::RobotTrajectory traj2 = traj1;
  traj2.joint_trajectory.joint_names.push_back("lj2");
  traj2.joint_trajectory.points[0].positions.push_back(1.0);
  traj2.multi_dof_joint_trajectory.joint_names.push_back("basej");
  traj2.multi_dof_joint_trajectory.points.resize(1);
  traj2.multi_dof_joint_trajectory.points[0].transforms.resize(1);

  if (!tem.push(traj2))
    RCLCPP_ERROR(LOGGER, "Fail!");

  traj1.multi_dof_joint_trajectory = traj2.multi_dof_joint_trajectory;
  if (!tem.push(traj1))
    RCLCPP_ERROR(LOGGER, "Fail!");

  if (!tem.executeAndWait())
    RCLCPP_ERROR(LOGGER, "Fail!");

  rclcpp::spin(node);
  return 0;
}
