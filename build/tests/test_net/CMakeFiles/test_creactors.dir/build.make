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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jack/code/Nancy

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jack/code/Nancy/build

# Include any dependencies generated for this target.
include tests/test_net/CMakeFiles/test_creactors.dir/depend.make

# Include the progress variables for this target.
include tests/test_net/CMakeFiles/test_creactors.dir/progress.make

# Include the compile flags for this target's objects.
include tests/test_net/CMakeFiles/test_creactors.dir/flags.make

tests/test_net/CMakeFiles/test_creactors.dir/test_creactors.cc.o: tests/test_net/CMakeFiles/test_creactors.dir/flags.make
tests/test_net/CMakeFiles/test_creactors.dir/test_creactors.cc.o: ../tests/test_net/test_creactors.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jack/code/Nancy/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tests/test_net/CMakeFiles/test_creactors.dir/test_creactors.cc.o"
	cd /home/jack/code/Nancy/build/tests/test_net && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_creactors.dir/test_creactors.cc.o -c /home/jack/code/Nancy/tests/test_net/test_creactors.cc

tests/test_net/CMakeFiles/test_creactors.dir/test_creactors.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_creactors.dir/test_creactors.cc.i"
	cd /home/jack/code/Nancy/build/tests/test_net && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jack/code/Nancy/tests/test_net/test_creactors.cc > CMakeFiles/test_creactors.dir/test_creactors.cc.i

tests/test_net/CMakeFiles/test_creactors.dir/test_creactors.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_creactors.dir/test_creactors.cc.s"
	cd /home/jack/code/Nancy/build/tests/test_net && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jack/code/Nancy/tests/test_net/test_creactors.cc -o CMakeFiles/test_creactors.dir/test_creactors.cc.s

# Object files for target test_creactors
test_creactors_OBJECTS = \
"CMakeFiles/test_creactors.dir/test_creactors.cc.o"

# External object files for target test_creactors
test_creactors_EXTERNAL_OBJECTS =

tests/test_net/test_creactors: tests/test_net/CMakeFiles/test_creactors.dir/test_creactors.cc.o
tests/test_net/test_creactors: tests/test_net/CMakeFiles/test_creactors.dir/build.make
tests/test_net/test_creactors: tests/test_net/CMakeFiles/test_creactors.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jack/code/Nancy/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable test_creactors"
	cd /home/jack/code/Nancy/build/tests/test_net && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_creactors.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/test_net/CMakeFiles/test_creactors.dir/build: tests/test_net/test_creactors

.PHONY : tests/test_net/CMakeFiles/test_creactors.dir/build

tests/test_net/CMakeFiles/test_creactors.dir/clean:
	cd /home/jack/code/Nancy/build/tests/test_net && $(CMAKE_COMMAND) -P CMakeFiles/test_creactors.dir/cmake_clean.cmake
.PHONY : tests/test_net/CMakeFiles/test_creactors.dir/clean

tests/test_net/CMakeFiles/test_creactors.dir/depend:
	cd /home/jack/code/Nancy/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jack/code/Nancy /home/jack/code/Nancy/tests/test_net /home/jack/code/Nancy/build /home/jack/code/Nancy/build/tests/test_net /home/jack/code/Nancy/build/tests/test_net/CMakeFiles/test_creactors.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/test_net/CMakeFiles/test_creactors.dir/depend

