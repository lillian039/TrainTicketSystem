cmake_minimum_required(VERSION 3.16)
project(TrainTicketSystem2022)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS " -Ofast")

include_directories(back_end)
include_directories(sjtu_hpp_library)

add_executable(code
        back_end_rollback/bptree_roll.hpp
        back_end_rollback/bufferList.hpp
        back_end_rollback/database.hpp
        back_end_rollback/main.cpp
        back_end_rollback/order.hpp
        back_end_rollback/parser.hpp
        back_end_rollback/train.hpp
        back_end_rollback/user.hpp
        sjtu_hpp_library/date.hpp
        sjtu_hpp_library/exceptions.hpp
        sjtu_hpp_library/linked_hashmap.hpp
        sjtu_hpp_library/string.hpp
        sjtu_hpp_library/utility.hpp
        sjtu_hpp_library/vector.hpp)
