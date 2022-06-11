cmake_minimum_required(VERSION 3.16)
project(TrainTicketSystem2022)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS " -Ofast")
include_directories(back_end)
include_directories(sjtu_hpp_library)

add_executable(TrainTicketSystem2022
        back_end/bptree.hpp
        back_end/bufferList.hpp
        back_end/database.hpp
        back_end/main.cpp
        back_end/order.hpp
        back_end/parser.hpp
        back_end/system.hpp
        back_end/train.hpp
        back_end/user.hpp
        sjtu_hpp_library/date.hpp
        sjtu_hpp_library/exceptions.hpp
        sjtu_hpp_library/linked_hashmap.hpp
        sjtu_hpp_library/string.hpp
        sjtu_hpp_library/utility.hpp
        sjtu_hpp_library/vector.hpp)
