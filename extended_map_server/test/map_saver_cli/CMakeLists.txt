include_directories(${PROJECT_SOURCE_DIR}/test)

# map_saver CLI
ament_add_gtest(test_map_saver_cli
  test_map_saver_cli.cpp
  ${PROJECT_SOURCE_DIR}/test/test_constants.cpp
)

ament_target_dependencies(test_map_saver_cli rclcpp nav_msgs grid_map_msgs grid_map_ros octomap octomap_msgs octomap_ros)
target_link_libraries(test_map_saver_cli
  stdc++fs
  ${dependencies}
)
