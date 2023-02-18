# Extended Map Server


This is part of my final degree work at the Universidad Rey Juan Carlos, Madrid, Spain, in the research group Intelligent Robotics Lab, headed by Francisco Martín Rico (@fmrico).


- Incorporated `grid_map` and `octomap` to the `map_server` and `map_saver`, in order to use Nav2 with non-planar maps. Therefore, `grid_map` and `octomap` dependencies where added to the package.

- Previous functionality conserved, allowing retrocompatibility

- Added tests

![map_saver_map_server](https://user-images.githubusercontent.com/22964725/214262420-79d4544a-6f3c-49ba-b0c0-512a04595ec6.png)

---


## How to use:

I added an `example.yaml`, `example.pgm`, `example_ele.pgm`, and `example_octo.ot` to `nav2_map_server/test` so it can be tried out easily.

### Serving the maps

At `navigation2/nav2_map_server/test`, the file `example.yaml` is:

    image: example.pgm
    mode: trinary
    resolution: 0.2
    origin: [0, 0, 0]
    negate: 0
    occupied_thresh: 0.65
    free_thresh: 0.25
    #  elevation layer
    elevation_image: example_ele.pgm
    min_height: -1.16
    max_height: 1.88
    #  octomap
    octomap_file: example_octo.ot
    binary: false     

`min_height` and `max_height` are necessary to convert the scale from the image pixels to a real metric value.

Note: octomap section is not necessary to load a grid_map with elevation information and grid_map section is not necessary to load and serve the octomap file.



With this file and `topic_name` parameter set to "map" (default):

    $ ros2 run nav2_map_server map_server --ros-args -p yaml_filename:=example.yaml
    .....
    .....

    $ ros2 lifecycle set /map_server 1
    Transition succesful
    $ ros2 lifecycle set /map_server 3
    Transition succesful

It creates 3 topics:
- `/$(topic_name)` : OccupancyGrid
- `/grid_map_$(topic_name)` : GridMap with `elevation` and `occupancy` (the same map as in /$(topic_name)) layers.
- `/octomap_$(topic_name)` : Octomap

![example_map_server](https://user-images.githubusercontent.com/22964725/214262336-1bb9d385-5071-40ac-9855-ddbccf7fd8bc.png)



### Saving the maps

When saving the maps, behaviour changes depending on the type of the topic and whether or not an octomap topic is specified.

- If the target map topic is a `nav_msgs/msg/OccupancyGrid` only a image of the 2d map is saved (the same as before).
- If the target map topic is a `grid_map_msgs/msg/GridMap` it will save the 2d map from the `occupancy` layer and the heigth information from the `elevation` layer.
- Additionally, if an octomap topic is specified, the octomap file is saved and its name written to the yaml file.

With the topics created from the previous example:

    $ ros2 run nav2_map_server map_saver_cli -t /grid_map_map --octo_t /octomap_map -f example_saving
    [INFO] [1674527902.388702271] [map_saver]: 
	map_saver lifecycle node launched. 
	Waiting on external lifecycle transitions to activate
	See https://design.ros2.org/articles/node_lifecycle.html for more information.
    [INFO] [1674527902.388795721] [map_saver]: Creating
    [INFO] [1674527902.388959642] [map_saver]: Configuring
    [INFO] [1674527902.390605276] [map_saver]: Saving map (occupancy + elevation) from '/grid_map_map' topic to 'example_saving' file
    [WARN] [1674527902.390625391] [map_saver]: Free threshold unspecified. Setting it to default value: 0.250000
    [WARN] [1674527902.390642502] [map_saver]: Occupied threshold unspecified. Setting it to default value: 0.650000
    ===> saveGridmapTopicToFile
    [WARN] [map_io]: Image format unspecified. Setting it to: pgm
    [INFO] [map_io]: Received a 300 X 300 map @ 0.2 m/pix
    [INFO] [map_io]: Writing map occupancy data to example_saving.pgm
    [WARN] [map_io]: Image format unspecified. Setting it to: pgm
    [INFO] [map_io]: Received a 300 X 300 map @ 0.2 m/pix
    [INFO] [map_io]: Writing map occupancy data to example_saving_ele.pgm
    [WARN] [map_io]: Image format unspecified. Setting it to: pgm
    [INFO] [map_io]: Writing map metadata to example_saving.yaml
    [INFO] [map_io]: Writing map metadata to example_saving.yaml
    [INFO] [map_io]: Map saved
    [INFO] [1674527902.460199348] [map_saver]: Map saved successfully
    ===> saveOctomapTopicToFile
    [INFO] [1674527902.460453576] [map_saver]: Saving octomap (3D obstacles) from '/octomap_map' topic to 'example_saving' file
    [WARN] [map_io]: Occupied threshold unspecified. Setting it to default value: 0.65
    [WARN] [map_io]: Free threshold unspecified. Setting it to default value: 0.25
    [WARN] [map_io]: Image format unspecified. Setting it to: pgm
    Map received (87 nodes, 0.2 m res), saving toexample_saving_octo.ot
    [INFO] [map_io]: Writing octomap metadata to example_saving.yaml
    [INFO] [map_io]: Octomap saved
    [INFO] [1674527902.473389494] [map_saver]: Map saved successfully
    [INFO] [1674527902.473791509] [map_saver]: Destroying

`example_saving.yaml`:

    image: example_saving_ele.pgm
    mode: trinary
    resolution: 0.2
    origin: [0, 0, 0]
    negate: 0
    occupied_thresh: 0.65
    free_thresh: 0.25
    #  elevation layer
    elevation_image: example_saving_ele.pgm
    min_height: -1.16
    max_height: 1.88
    #  octomap
    octomap_file: example_saving_octo.ot
    binary: false

To create these topics to save the files from a Gazebo world simulation, the [fmrico/gazebo_gridmap_plugin](https://github.com/fmrico/gazebo_gridmap_plugin) can be used.
