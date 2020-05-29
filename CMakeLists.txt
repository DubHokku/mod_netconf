cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 11)

# add_executable(client main.cc)	
# find_library(PTHREAD_LIBRARY netconf2)
# add_executable(client main.cc)
# target_link_libraries(client ${LIBNETCONF2_LIBRARIES})
# target_link_libraries(client ${LIBYANG_LIBRARIES})

find_package(LibNETCONF2 REQUIRED)
find_package(LibYANG REQUIRED)

include_directories(${LIBNETCONF2_INCLUDE_DIRS})
include_directories(${LIBYANG_INCLUDE_DIRS})

# add_library(nc_connect STATIC nc_connect.cc)
add_executable(connect mod_netconf.cc)



# target_link_libraries(connect ${LIBNETCONF2_LIBRARIES})
# target_link_libraries(connect ${LIBYANG_LIBRARIES})
target_link_libraries(connect 	
				${LIBNETCONF2_LIBRARIES}
				${LIBYANG_LIBRARIES})




# cp FindLibNETCONF2.cmake to
# /usr/share/cmake-3.10/Modules
