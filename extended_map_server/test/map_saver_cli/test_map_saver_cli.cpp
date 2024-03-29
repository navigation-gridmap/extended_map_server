// Copyright (c) 2020 Samsung Research America
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <octomap/octomap.h>

#include <gtest/gtest.h>
#include <experimental/filesystem>
#include <string>
#include <memory>
#include <utility>
#include <grid_map_ros/grid_map_ros.hpp>
#include <octomap_msgs/msg/octomap.hpp>

#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/occupancy_grid.hpp"
#include "grid_map_msgs/msg/grid_map.hpp"

#include "octomap_msgs/conversions.h"

#include "octomap_ros/conversions.hpp"

TEST(MapSaverCLI, CLITest)
{
  std::string path = "/tmp/";
  std::string file = "test_map";
  std::string file_path = path + file;
  std::string grid_map_file_path = path + "gridmap_" + file;

  rclcpp::init(0, nullptr);

  auto node = std::make_shared<rclcpp::Node>("CLI_Test_Node");
  RCLCPP_INFO(node->get_logger(), "Testing Map Saver CLI");

  auto publisher = node->create_publisher<nav_msgs::msg::OccupancyGrid>(
    "/map",
    rclcpp::QoS(rclcpp::KeepLast(1)).transient_local().reliable());

  auto msg = std::make_unique<nav_msgs::msg::OccupancyGrid>();
  msg->header.frame_id = "map";
  msg->header.stamp = node->now();
  msg->info.map_load_time = node->now();
  msg->info.resolution = 0.05;
  msg->info.width = 3;
  msg->info.height = 3;
  msg->info.origin.position.x = 0.0;
  msg->info.origin.position.y = 0.0;
  msg->info.origin.orientation.w = 1.0;
  msg->data.resize(9);
  msg->data[0] = 0;
  msg->data[2] = 100;
  msg->data[1] = 101;
  msg->data[3] = 50;

  RCLCPP_INFO(node->get_logger(), "Publishing occupancy grid...");

  publisher->publish(std::move(msg));

  rclcpp::Rate(1).sleep();

  // succeed on real map
  RCLCPP_INFO(node->get_logger(), "Calling saver...");

  EXPECT_FALSE(std::experimental::filesystem::exists(file_path + ".yaml"));

  std::string command =
    std::string(
    "ros2 run extended_map_server map_saver_cli -f ") + file_path;
  auto return_code = system(command.c_str());
  EXPECT_EQ(return_code, 0);

  rclcpp::Rate(0.5).sleep();

  RCLCPP_INFO(node->get_logger(), "Checking on file...");

  EXPECT_TRUE(std::experimental::filesystem::exists(file_path + ".pgm"));
  EXPECT_EQ(std::experimental::filesystem::file_size(file_path + ".pgm"), 20ul);

  if (std::experimental::filesystem::exists(file_path + ".yaml")) {
    std::experimental::filesystem::remove(file_path + ".yaml");
  }
  if (std::experimental::filesystem::exists(file_path + ".pgm")) {
    std::experimental::filesystem::remove(file_path + ".pgm");
  }

  auto publisher_gridmap = node->create_publisher<grid_map_msgs::msg::GridMap>(
    "/grid_map",
    rclcpp::QoS(rclcpp::KeepLast(1)).transient_local().reliable());

  grid_map::GridMap test_gridmap = grid_map::GridMap({"occupancy", "elevation"});

  test_gridmap.setFrameId("map");
  test_gridmap.setGeometry(grid_map::Length(3.0, 3.0), 0.05, grid_map::Position(0.0, 0.0));

  test_gridmap.add("elevation", 0.0);
  test_gridmap.add("occupancy", 1.0);

  std::unique_ptr<grid_map_msgs::msg::GridMap> msg_gridmap =
    grid_map::GridMapRosConverter::toMessage(test_gridmap);

  msg_gridmap->header.frame_id = "map";
  msg_gridmap->header.stamp = node->now();

  publisher_gridmap->publish(std::move(msg_gridmap));

  auto octomap_pub = node->create_publisher<octomap_msgs::msg::Octomap>(
    "/octomap", rclcpp::QoS(rclcpp::KeepLast(1)).transient_local().reliable());
  std::unique_ptr<octomap::OcTree> tree = std::make_unique<octomap::OcTree>(0.5);
  octomap_msgs::msg::Octomap octo_msg;

  octomap_msgs::fullMapToMsg(*tree, octo_msg);
  octo_msg.header.frame_id = "octomap_frame";
  octo_msg.header.stamp = node->now();
  octomap_pub->publish(octo_msg);

  rclcpp::Rate(0.5).sleep();

  // succeed on real map
  RCLCPP_INFO(node->get_logger(), "Calling saver...");

  EXPECT_FALSE(std::experimental::filesystem::exists(grid_map_file_path + ".yaml"));

  command =
    std::string(
    "ros2 run extended_map_server map_saver_cli -t /grid_map --octo_t /octomap -f ") +
    grid_map_file_path;
  return_code = system(command.c_str());
  EXPECT_EQ(return_code, 0);

  RCLCPP_INFO(node->get_logger(), "Checking on file...");

  EXPECT_TRUE(std::experimental::filesystem::exists(grid_map_file_path + ".pgm"));
  EXPECT_TRUE(std::experimental::filesystem::exists(grid_map_file_path + "_ele.pgm"));
  EXPECT_TRUE(std::experimental::filesystem::exists(grid_map_file_path + "_octo.ot"));

  if (std::experimental::filesystem::exists(grid_map_file_path + ".yaml")) {
    std::experimental::filesystem::remove(grid_map_file_path + ".yaml");
  }
  if (std::experimental::filesystem::exists(grid_map_file_path + ".pgm")) {
    std::experimental::filesystem::remove(grid_map_file_path + ".pgm");
  }
  if (std::experimental::filesystem::exists(grid_map_file_path + "_ele.pgm")) {
    std::experimental::filesystem::remove(grid_map_file_path + "_ele.pgm");
  }
  if (std::experimental::filesystem::exists(grid_map_file_path + "_octo.ot")) {
    std::experimental::filesystem::remove(grid_map_file_path + "_octo.ot");
  }

  rclcpp::Rate(0.5).sleep();

  // fail on bogus map
  RCLCPP_INFO(node->get_logger(), "Calling saver...");

  EXPECT_FALSE(std::experimental::filesystem::exists(file_path + ".yaml"));

  command =
    std::string(
    "ros2 run extended_map_server map_saver_cli "
    "-t map_failure --occ 100 --free 2 --mode trinary --fmt png -f ") + file_path +
    std::string("--ros-args __node:=map_saver_test_node");
  return_code = system(command.c_str());
  EXPECT_EQ(return_code, 65280);

  rclcpp::Rate(0.25).sleep();

  RCLCPP_INFO(node->get_logger(), "Checking on file...");

  EXPECT_FALSE(std::experimental::filesystem::exists(file_path + ".yaml"));

  RCLCPP_INFO(node->get_logger(), "Testing help...");
  command =
    std::string(
    "ros2 run extended_map_server map_saver_cli -h");
  return_code = system(command.c_str());
  EXPECT_EQ(return_code, 0);

  rclcpp::Rate(0.5).sleep();

  RCLCPP_INFO(node->get_logger(), "Testing invalid mode...");
  command =
    std::string(
    "ros2 run extended_map_server map_saver_cli --mode fake_mode");
  return_code = system(command.c_str());
  EXPECT_EQ(return_code, 0);

  rclcpp::Rate(0.5).sleep();

  RCLCPP_INFO(node->get_logger(), "Testing missing argument...");
  command =
    std::string(
    "ros2 run extended_map_server map_saver_cli --mode");
  return_code = system(command.c_str());
  EXPECT_EQ(return_code, 65280);

  rclcpp::Rate(0.5).sleep();

  RCLCPP_INFO(node->get_logger(), "Testing wrong argument...");
  command =
    std::string(
    "ros2 run extended_map_server map_saver_cli --free 0 0");
  return_code = system(command.c_str());
  EXPECT_EQ(return_code, 65280);

  rclcpp::Rate(0.5).sleep();

  command =
    std::string(
    "ros2 run extended_map_server map_saver_cli --ros-args -r __node:=map_saver_test_node");
  return_code = system(command.c_str());
  EXPECT_EQ(return_code, 0);
}
