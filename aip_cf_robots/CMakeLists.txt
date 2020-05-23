cmake_minimum_required(VERSION 2.8.3)
project(aip_cf_robots)

add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  genmsg
  message_generation
  aip_cf_commande_locale
)

## Generate messages in the 'msg' folder
add_message_files(
   FILES
   MoveRobot.msg
   FaireTacheMsg.msg
   Msg_numrobot.msg
)

## Generate added messages and services with any dependencies listed here
generate_messages(
   DEPENDENCIES
   std_msgs
 )

catkin_package(
   CATKIN_DEPENDS message_runtime
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
# include_directories(include)
include_directories(
  ${catkin_INCLUDE_DIRS}
)

add_executable(robot src/main_robot.cpp src/Robot.cpp src/Poste.cpp)
target_link_libraries(robot ${catkin_LIBRARIES})
add_dependencies(robot ${PROJECT_NAME}_gencpp ${catkin_EXPORTED_TARGETS})

