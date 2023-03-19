cmake_minimum_required(VERSION 3.5)
project(extended_mapping_msgs)

find_package(ament_cmake REQUIRED)
find_package(nav2_common REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(std_msgs REQUIRED)
find_package(grid_map_msgs REQUIRED)
find_package(octomap_msgs REQUIRED)


rosidl_generate_interfaces(${PROJECT_NAME}
  "srv/LoadMap.srv"
  "srv/SaveMap.srv"
  DEPENDENCIES builtin_interfaces geometry_msgs std_msgs nav_msgs grid_map_msgs octomap_msgs
)

ament_export_dependencies(rosidl_default_runtime)

ament_package()
