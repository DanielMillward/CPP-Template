Don't let Visual Studio add files automatically!! It's redundant. To stop it from asking, follow these steps: https://stackoverflow.com/a/77932221 

Go to Tools > Options > CMake > Uncheck "Enable automatic CMake script modification for file operations from folder view"

## How to release
Build/Run the main executable with preset "release". Then run:
```cmd
dumpbin /dependents build\release\myexecutable.exe
```
To verify that there are no other dependencies. Then copy just the executable in `/build/release/myexecutable.exe` and the `/resources` folder to wherever you want!

## How to add another third-party library that uses CMake
1. Copy/paste the source code into `third_party/OtherLibNamev1.0`
2. Somewhere above the targets definition in CMakeLists.txt, add the subdirectory with any options:
```cmake
# ---- GoogleTest from vendored source (static) ----
set(some_var_of_the_library ON CACHE BOOL "" FORCE)
add_subdirectory(third_party/OtherLibNamev1.0 EXCLUDE_FROM_ALL)
```
3. Add the library to the list of files in the targets, e.g.
```
target_link_libraries(${MAIN_EXE_NAME} PRIVATE SDL3::SDL3 OTHERLIBNAME::Otherlib-main)
```