// Copyright (c) 2018 Intel Corporation
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

/* OccupancyGrid map input-output library */

#ifndef EXTENDED_MAP_SERVER__MAP_IO_HPP_
#define EXTENDED_MAP_SERVER__MAP_IO_HPP_

#include <octomap/octomap.h>

#include <string>
#include <vector>

#include "extended_map_server/map_mode.hpp"
#include "nav_msgs/msg/occupancy_grid.hpp"
#include "geometry_msgs/msg/quaternion.hpp"
#include "grid_map_ros/grid_map_ros.hpp"
#include "grid_map_ros/GridMapRosConverter.hpp"
#include "grid_map_msgs/msg/grid_map.hpp"
#include <octomap_msgs/msg/octomap.hpp>
#include "octomap_msgs/conversions.h"
#include "octomap_ros/conversions.hpp"

/* Map input part */

namespace extended_map_server
{

struct LoadParameters
{
  std::string image_file_name;
  double resolution{0};
  std::vector<double> origin{0, 0, 0};
  double free_thresh;
  double occupied_thresh;
  MapMode mode;
  bool negate;
  std::string elevation_image_file_name;
  double min_height;
  double max_height;
  std::string octomap_file_name;
  bool binary;
};

typedef enum
{
  LOAD_MAP_SUCCESS,
  MAP_DOES_NOT_EXIST,
  INVALID_MAP_METADATA,
  INVALID_MAP_DATA
} LOAD_MAP_STATUS;

/**
 * @brief Load and parse the given YAML file
 * @param yaml_filename Name of the map file passed though parameter
 * @return Map loading parameters obtained from YAML file
 * @throw YAML::Exception
 */
LoadParameters loadMapYaml(const std::string & yaml_filename);

/**
 * @brief Load the image from map file and generate an OccupancyGrid
 * @param load_parameters Parameters of loading map
 * @param map Output loaded map
 * @throw std::exception
 */
void loadMapFromFile(
  const LoadParameters & load_parameters,
  nav_msgs::msg::OccupancyGrid & map);

/**
 * @brief Load the map YAML, image from map file and
 * generate an OccupancyGrid
 * @param yaml_file Name of input YAML file
 * @param map Output loaded map
 * @return status of map loaded
 */
LOAD_MAP_STATUS loadMapFromYaml(
  const std::string & yaml_file,
  nav_msgs::msg::OccupancyGrid & map);


/**
 * @brief Load the map YAML, image from map file and
 * generate an OccupancyGrid
 * @param yaml_file Name of input YAML file
 * @param map Output loaded map
 * @param msg_grid_map Output loaded grid_map
 * @param msg_octomap Output loaded octomap
 * @return status of map loaded
 */
LOAD_MAP_STATUS loadMapFromYaml(
  const std::string & yaml_file,
  nav_msgs::msg::OccupancyGrid & map,
  grid_map_msgs::msg::GridMap & msg_grid_map,
  octomap_msgs::msg::Octomap & msg_octomap);

/* Map output part */

struct SaveParameters
{
  std::string map_file_name{""};
  std::string image_format{""};
  double free_thresh{0.0};
  double occupied_thresh{0.0};
  MapMode mode{MapMode::Trinary};

  double max_height{0.0};
  double min_height{0.0};
};

/**
 * @brief Write OccupancyGrid map to file
 * @param map OccupancyGrid map data
 * @param save_parameters Map saving parameters.
 * @return true or false
 */
bool saveMapToFile(
  const nav_msgs::msg::OccupancyGrid & map,
  const SaveParameters & save_parameters);

bool saveMapToFile(
  const grid_map_msgs::msg::GridMap & map,
  const SaveParameters & save_parameters);
bool saveOctomapToFile(
  const octomap_msgs::msg::Octomap & octomap_msg,
  const SaveParameters & save_parameters);

}  // namespace extended_map_server

#endif  // EXTENDED_MAP_SERVER__MAP_IO_HPP_
