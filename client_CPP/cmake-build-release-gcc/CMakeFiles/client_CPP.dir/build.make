# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/nero/CLion-2020.1.2/clion-2020.1.2/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/nero/CLion-2020.1.2/clion-2020.1.2/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/nero/CLionProjects/client_CPP

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/nero/CLionProjects/client_CPP/cmake-build-release-gcc

# Include any dependencies generated for this target.
include CMakeFiles/client_CPP.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/client_CPP.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/client_CPP.dir/flags.make

CMakeFiles/client_CPP.dir/main.cpp.o: CMakeFiles/client_CPP.dir/flags.make
CMakeFiles/client_CPP.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nero/CLionProjects/client_CPP/cmake-build-release-gcc/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/client_CPP.dir/main.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/client_CPP.dir/main.cpp.o -c /home/nero/CLionProjects/client_CPP/main.cpp

CMakeFiles/client_CPP.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/client_CPP.dir/main.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nero/CLionProjects/client_CPP/main.cpp > CMakeFiles/client_CPP.dir/main.cpp.i

CMakeFiles/client_CPP.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/client_CPP.dir/main.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nero/CLionProjects/client_CPP/main.cpp -o CMakeFiles/client_CPP.dir/main.cpp.s

# Object files for target client_CPP
client_CPP_OBJECTS = \
"CMakeFiles/client_CPP.dir/main.cpp.o"

# External object files for target client_CPP
client_CPP_EXTERNAL_OBJECTS =

client_CPP: CMakeFiles/client_CPP.dir/main.cpp.o
client_CPP: CMakeFiles/client_CPP.dir/build.make
client_CPP: CMakeFiles/client_CPP.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/nero/CLionProjects/client_CPP/cmake-build-release-gcc/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable client_CPP"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/client_CPP.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/client_CPP.dir/build: client_CPP

.PHONY : CMakeFiles/client_CPP.dir/build

CMakeFiles/client_CPP.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/client_CPP.dir/cmake_clean.cmake
.PHONY : CMakeFiles/client_CPP.dir/clean

CMakeFiles/client_CPP.dir/depend:
	cd /home/nero/CLionProjects/client_CPP/cmake-build-release-gcc && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/nero/CLionProjects/client_CPP /home/nero/CLionProjects/client_CPP /home/nero/CLionProjects/client_CPP/cmake-build-release-gcc /home/nero/CLionProjects/client_CPP/cmake-build-release-gcc /home/nero/CLionProjects/client_CPP/cmake-build-release-gcc/CMakeFiles/client_CPP.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/client_CPP.dir/depend

