cmake_minimum_required(VERSION 3.23)
project(SSCVInject)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenSSL REQUIRED)

add_library(internal)
add_library(imgui STATIC)
add_library(nlohmann_json INTERFACE)

file(GLOB_RECURSE fp_internal "src/libs/internal/*.cpp")
file(GLOB_RECURSE fp_imgui "src/libs/imgui/*.cpp")
file(GLOB_RECURSE fp_json "src/libs/nlohmann/*.hpp")

#[[ INTERNAL ]]#
target_include_directories(internal
  PUBLIC
    "src/libs/internal"     # INTERNAL
    "src/libs/cpp-httplib"  # CPP-HTTPLIB
    "src/libs/stb_image"    # STB-IMAGE
)
target_link_libraries(internal 
    PUBLIC
      imgui
      glfw
      OpenGL::GL
      nlohmann_json
)
target_sources(internal PRIVATE ${fp_internal})
#[[ LIB IMGUI ]]#
target_include_directories(imgui
  PUBLIC
  "src/libs/imgui"
  "src/libs/imgui/backends"
)

target_link_libraries(imgui PUBLIC 
    glfw 
    OpenGL::GL
)
target_sources(imgui PRIVATE ${fp_imgui})
#[[ NLOHMANN JSON ]]#
target_include_directories(nlohmann_json
  INTERFACE
    "src/libs/nlohmann"
)
#[[ Main ]]#
add_executable(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME}
  PRIVATE 
    CPPHTTPLIB_OPENSSL_SUPPORT
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    internal
    imgui
)
target_sources(${PROJECT_NAME} PRIVATE src/sscv.cpp)

