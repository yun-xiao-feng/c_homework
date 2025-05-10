#标题
cmake_minimum_required(VERSION 3.10.0)
project(SDL_testhomework VERSION 0.1.0 LANGUAGES C CXX)
#编译选项
add_compile_options(/M4)
#设置编译输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

#查找并输入cmake预设
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)

#添加可执行文件
add_executable(SDL_testhomework 
"src/main.cpp"
"src/Game.cpp"
"src/Scene.cpp"
"src/SceneTitle.cpp"
"src/Scenemain.cpp"
"src/SceneEnd.cpp"
)
#链接库
target_link_libraries(SDL_testhomework 
SDL2::SDL2main
 SDL2::SDL2
 SDL2_image::SDL2_image
 SDL2_mixer::SDL2_mixer
 SDL2_ttf::SDL2_ttf
 )
