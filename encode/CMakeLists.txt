cmake_minimum_required(VERSION 3.12)
project(ais_nmea_encode)

SET(C_STANDARD 11)

add_executable(${PROJECT_NAME}
	main.c
)

add_executable(${PROJECT_NAME}_example
	main.cc
	encode.cc
)
