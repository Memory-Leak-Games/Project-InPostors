project(TextFlow)

add_library(TextFlow TextFlow/TextFlow.hpp)
target_include_directories(TextFlow PUBLIC TextFlow)
set_target_properties(TextFlow PROPERTIES LINKER_LANGUAGE CXX)