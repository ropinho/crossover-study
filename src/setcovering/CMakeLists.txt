cmake_minimum_required(VERSION 3.14)
project(SetCovering LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

include(GNUInstallDirs)
string(TOLOWER ${PROJECT_NAME} executable_name)

add_executable(${executable_name}
  main.cpp
  scp_matrix.cpp
  scp_model.cpp
  set_covering_problem.cpp
  genetic_algorithm_scp.cpp
)

target_include_directories(${executable_name} PRIVATE ../../include)
target_link_libraries(${executable_name} PRIVATE GA CLI utils vsqlitepp)

install(TARGETS ${executable_name} DESTINATION ${CMAKE_INSTALL_BINARYDIR})
