cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(dync2p)
set (CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${dync2p_SOURCE_DIR}/src)
add_executable(dync2p main.cpp Lexer.cpp Parser.cpp Token.cpp Node.cpp Error.cpp Runner.cpp Lexer.h Parser.h Token.h Node.h Error.h Runner.h)