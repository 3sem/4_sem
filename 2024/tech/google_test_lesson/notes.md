Here is a full example of integrating Google Test into a project with CMake:

**Step 1: Create a new project directory and create the following files and folders**
```bash
mkdir MyProject
cd MyProject
mkdir src
mkdir tst
touch CMakeLists.txt
touch src/main.cpp
touch tst/test_main.cpp
```
**Step 2: Create the `CMakeLists.txt` file**
```cmake
cmake_minimum_required(VERSION 3.14)
project(MyProject)

# Set the C++ standard to C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the FetchContent module
include(FetchContent)

# Declare and fetch Google Test
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.11.0
)

# Make Google Test available
FetchContent_MakeAvailable(googletest)

# Add the executable for the main application
add_executable(main src/main.cpp)

# Add the executable for the tests
add_executable(test_main tst/test_main.cpp)

# Link the test executable against Google Test
target_link_libraries(test_main
  PRIVATE
  GTest::GTest
  main
)

# Add a test for the test executable
add_test(test_main test_main)
```
**Step 3: Create the `src/main.cpp` file**
```cpp
#include <iostream>

int main() {
  std::cout << "Hello, World!" << std::endl;
  return 0;
}
```
**Step 4: Create the `tst/test_main.cpp` file**
```cpp
#include <gtest/gtest.h>

TEST(sample_test_case, sample_test) {
  EXPECT_EQ(1, 1);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
```
**Step 5: Build and run the project**

Open a terminal in the project directory and run the following commands:
```
mkdir build
cd build
cmake ..
cmake --build .
ctest
```
This will build the project and run the tests. You should see output indicating that the test passed.

Note: This example uses the `FetchContent` module to download and include Google Test in the project. This is a convenient way to integrate Google Test into a CMake project. Alternatively, you can also use `find_package` to find an installed version of Google Test, or include Google Test as a subdirectory in your project.

I hope this helps Let me know if you have any questions.
