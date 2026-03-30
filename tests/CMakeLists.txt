find_package(doctest CONFIG REQUIRED)

add_executable(all.t
  all.t.cpp
  hello.t.cpp
)

target_link_libraries(all.t PRIVATE doctest::doctest)