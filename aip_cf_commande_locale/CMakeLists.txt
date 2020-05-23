cmake_minimum_required(VERSION 2.8.3)
project(aip_cf_commande_locale)

add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  sensor_msgs
  image_transport
  cv_bridge
  genmsg
  message_generation
  aip_cf_shuttles
  aip_cf_aiguillages)

find_package(OpenCV)

include_directories(${catkin_INCLUDE_DIRS})
include_directories(include ${OpenCV_INCLUDE_DIRS})

add_service_files(
  FILES 
  SrvAddProduct.srv 
  SrvFinInit.srv
  SrvAddProductPushBack.srv
  )

add_message_files(
  FILES
  Msg_AddProduct.msg
  Msg_StopControl.msg
  Msg_Color.msg
  Msg_SwitchControl.msg
  Msg_PinControl.msg
  Msg_AddProduct.msg
  Msg_Erreur.msg
  RobotJoints.msg
  TacheFinieMsg.msg
  DeplacerPieceMsg.msg
  Actionneurs.msg
  Capteurs.msg
  )

generate_messages( DEPENDENCIES std_msgs)
catkin_package( CATKIN_DEPENDS message_runtime)

add_executable(simulation
  src/commande_locale.cpp
  src/inOutController.cpp
  src/vrepController.cpp)

target_link_libraries(simulation ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
add_dependencies(simulation
  aip_cf_commande_locale_generate_messages_cpp)

add_executable(display_node src/display.cpp)
target_link_libraries(display_node ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
add_dependencies(display_node
  aip_cf_commande_locale_generate_messages_cpp)

add_executable(log_manager src/LogManager.cpp)
target_link_libraries(log_manager ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
add_dependencies(log_manager
  aip_cf_commande_locale_generate_messages_cpp)
