# RobotSimulator

A C++17 simulator for robot navigation in initially unknown 2D
environments. Select a target cell and watch the robot discover
obstacles, update its internal map, and adjust its planned route.

<!-- Add a screenshot or demo GIF here. -->






## Overview

RobotSimulator demonstrates grid-based mapping and path planning
in an interactive 2D environment.

The robot begins without knowledge of the surrounding obstacles.
As it moves toward a selected destination, simulated distance
measurements reveal the environment, allowing it to update its map
and replan its route.

## Technologies

- **C++17**
- **SFML 3** for graphics and window handling
- **CMake 3.22 or newer**


## Environment assumptions
- Static obstacles.
- Initially unknown environment.
- Perfect sensors with infinite range.
- All cells initially assumed to be free.
- Mapping is grid-based.


## How it works

<img width="707" height="451" alt="image" src="https://github.com/user-attachments/assets/8829f957-0fc2-4891-bf59-2d933f0cca1d" />


## Algorithms and Structures


| Component | Implementation |
|---|---|
| Environment representation | Grid-based map |
| Mapping | Incremental obstacle discovery |
| Navigation | A* + custom start and destination selection |
| Collision avoidance | Obstacle inflation |
| Perception | Three raycasting sensors |
| Ray traversal | Amanatides-Woo |
| Dynamic adaptation | Periodic, event-triggered replanning |


### 1. Memory Grids

The environment is represented by a grid covering the entire screen. Each cell corresponds to a 16 × 16 pixel square.

**Obstacle Grid**

The obstacle grid represents the robot's internal map of the environment. Each cell stores:

- `0` – No obstacle detected.
- `> 0` – Timestamp (in seconds) indicating when an obstacle was discovered.

Initially, all cells are set to `0`, meaning the robot assumes the entire environment is obstacle-free. As the robot moves and its sensors discover obstacles, the internal map is gradually updated.

Since obstacles are static in the current version, discovered obstacles only need to be added to the map, not removed.

The sensors only need to mark the cells containing obstacles. All cells traversed by a sensor ray before reaching an obstacle are already considered free.

**Passability Grid**

To prevent the robot from getting too close to walls, obstacles are inflated by approximately the robot's radius. This creates a safety margin around each obstacle.

The resulting passability grid has the same dimensions as the obstacle grid:

- `1` – Passable cell.
- `0` – Impassable cell.

Whenever a new obstacle is discovered, the corresponding cell and its surrounding cells are marked as impassable.

Instead of recalculating the entire grid, only the area surrounding the newly discovered obstacle is updated. This avoids unnecessary calculations.

(Picture illustrating obstacle inflation and the resulting safety margin.)

### 2. Pathfinding

Navigation is based on the *A\* algorithm*, which searches for the shortest path through the passability grid.

A\* combines the actual cost of reaching a cell with a heuristic estimate of its distance to the destination. Unlike Dijkstra's algorithm, this allows it to prioritize cells that appear more promising for reaching the goal.

If the destination is unreachable, the algorithm searches for an alternative destination. It gradually examines larger squares around the requested destination until it finds passable cells, then selects the one closest to the original goal according to the heuristic distance.

**Current limitation:** The alternative destination might itself be unreachable from the robot's position. In this case, the algorithm can repeatedly select the same unreachable cells, resulting in an infinite loop. This will be addressed in a future update.

Another special case occurs when the robot discovers obstacles too late and finds itself inside an impassable area according to its updated passability grid. When calculating a new path, the algorithm adds the nearest passable cell to the goal to the beginning of the path, allowing the robot to leave the affected area and continue navigating.

**Automatic replanning**

The robot automatically recalculates its path when both of the following conditions are met:

- At least 300 ms have passed since the previous path update.
- At least one new obstacle has been discovered since the previous update.

Currently, replanning occurs whenever a new obstacle is discovered, even if it does not affect the existing path. More selective replanning is planned for a future update.

### 3. Sensors

The robot is equipped with three ideal distance sensors with unlimited range.

Each sensor uses raycasting to detect the nearest obstacle along its direction and returns the coordinates of the detected obstacle.

Ray traversal is implemented using the *Amanatides-Woo voxel traversal algorithm*. It efficiently determines which grid cells a ray intersects and identifies the first obstacle encountered, avoiding unnecessary geometric calculations.

To handle rays that run directly along grid boundaries, a small offset is added to the ray's endpoint in its direction of travel. This nudges the endpoint into the next cell along the ray and resolves ambiguity when determining the final cell.




## Further improvements

### Planned
- **Smarter replanning:** Only recalculate the path when newly discovered obstacles affect the current route.
- **Improved destination selection:** Prevent infinite loops when the requested destination is unreachable.
- **Dynamic obstacles:** Support moving obstacles and update the internal map accordingly.

### Potential extensions
- **Sensor noise:** Introduce inaccuracies into distance measurements.
- **Limited sensor range:** Simulate more realistic distance sensors.
- **Robot localization:** Introduce uncertainty in the robot's estimated position.

## Project Structure

```text
RobotSimulator/
├── assets/
│   └── fonts/        # Font assets
├── include/          # Header files
├── src/              # C++ implementation files
└── CMakeLists.txt    # Build configuration
```



## Build

### Requirements

- A compiler supporting C++17
- CMake 3.22 or newer
- Git and internet access for the initial SFML download
- Platform-specific dependencies required to build SFML

SFML is fetched from its upstream repository during CMake
configuration.

### Configure and compile

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
```

The executable is named `robot_simulator`. Its location depends
on the selected CMake generator and platform.

### Optional profiling build

GNU gprof instrumentation can be enabled when building with GCC:

```sh
cmake -S . -B build-profile \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DROBOT_SIMULATOR_ENABLE_PROFILING=ON

cmake --build build-profile --parallel
```

Collecting and inspecting profiling results also requires a
compatible platform and the `gprof` tool.
