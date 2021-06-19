cmake_minimum_Required(VERSION 3.15)

project(polylux VERSION 0.0.1 LANGUAGES CXX)

add_library(polylux STATIC)

target_sources(polylux PRIVATE src/polylux.cc src/entry/entry_php.cc src/entry/entry_python.cc)


set_target_properties(polylux PROPERTIES CXX_STANDARD 17)
set_target_properties(polylux PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(polylux PROPERTIES CXX_VISIBILITY_PRESET hidden)

include(GNUInstallDirs)

target_include_directories(polylux PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

install(
	TARGETS polylux
	EXPORT polylux-config
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
	EXPORT polylux-config
	NAMESPACE polylux::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/polylux)

install(
	DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

add_subdirectory(sample)
