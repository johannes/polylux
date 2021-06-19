add_library(polylux_demo SHARED)
set_property(TARGET polylux_demo PROPERTY CXX_STANDARD 17)
set_property(TARGET polylux_demo PROPERTY PREFIX "")

target_sources(polylux_demo PRIVATE sample.cc)
target_link_libraries(polylux_demo polylux)


