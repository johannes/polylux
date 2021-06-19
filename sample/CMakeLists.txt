add_library(polylux_sample SHARED)
set_property(TARGET polylux_sample PROPERTY CXX_STANDARD 17)
set_property(TARGET polylux_sample PROPERTY PREFIX "")

target_sources(polylux_sample PRIVATE sample.cc)
target_link_libraries(polylux_sample polylux)


