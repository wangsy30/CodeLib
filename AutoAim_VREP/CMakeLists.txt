cmake_minimum_required(VERSION 3.5)
project(AutoAim_VREP)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_PREFIX_PATH "/usr/local/opencv/opencv-4.5.1")


find_package(OpenCV REQUIRED)

find_package(Eigen3 REQUIRED)

# include_directories(${EIGEN3_INCLUDE_DIRS})
# message("${EIGEN3_INCLUDE_DIRS}")
# dir for lib & src


include_directories(
        ${OpenCV_INCLUDE_DIRS}
        ${EIGEN3_INCLUDE_DIRS}
        ${Sophus_INCLUDE_DIRS}
		CoppeliaSim
		CoppeliaSim/include
		CoppeliaSim/remoteApi
		CoppeliaSim/include/stack
		thread/inc
		para
		ProcessBox
)

add_subdirectory(./CoppeliaSim)
add_subdirectory(para)
add_subdirectory(ProcessBox)

add_executable(AutoAim_VREP
        main.cpp
		armorDetector.cpp
        )
add_library(
		thread
		thread/src/thread.cpp
		thread/src/cameraThread.cpp
		thread/src/serialPortReadThread.cpp
		thread/src/serialPortWriteThread.cpp
)

target_link_libraries(
		AutoAim_VREP
		thread
		para
		processbox
		#coppeliaSim
		coppeliaSim_client
		${OpenCV_LIBS}
		# -lopencv_video
		-lboost_system
		-lboost_filesystem
		-lpthread
        )

