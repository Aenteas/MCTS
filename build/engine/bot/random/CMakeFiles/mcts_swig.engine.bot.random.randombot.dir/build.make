# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.23

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /snap/cmake/1082/bin/cmake

# The command to remove a file.
RM = /snap/cmake/1082/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/david/projects/mcts_swig

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/david/projects/mcts_swig/build

# Include any dependencies generated for this target.
include engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/compiler_depend.make

# Include the progress variables for this target.
include engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/progress.make

# Include the compile flags for this target's objects.
include engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/flags.make

engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/mcts_swig.engine.bot.random.randombot_autogen/mocs_compilation.cpp.o: engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/flags.make
engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/mcts_swig.engine.bot.random.randombot_autogen/mocs_compilation.cpp.o: engine/bot/random/mcts_swig.engine.bot.random.randombot_autogen/mocs_compilation.cpp
engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/mcts_swig.engine.bot.random.randombot_autogen/mocs_compilation.cpp.o: engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/david/projects/mcts_swig/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/mcts_swig.engine.bot.random.randombot_autogen/mocs_compilation.cpp.o"
	cd /home/david/projects/mcts_swig/build/engine/bot/random && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/mcts_swig.engine.bot.random.randombot_autogen/mocs_compilation.cpp.o -MF CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/mcts_swig.engine.bot.random.randombot_autogen/mocs_compilation.cpp.o.d -o CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/mcts_swig.engine.bot.random.randombot_autogen/mocs_compilation.cpp.o -c /home/david/projects/mcts_swig/build/engine/bot/random/mcts_swig.engine.bot.random.randombot_autogen/mocs_compilation.cpp

engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/mcts_swig.engine.bot.random.randombot_autogen/mocs_compilation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/mcts_swig.engine.bot.random.randombot_autogen/mocs_compilation.cpp.i"
	cd /home/david/projects/mcts_swig/build/engine/bot/random && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/david/projects/mcts_swig/build/engine/bot/random/mcts_swig.engine.bot.random.randombot_autogen/mocs_compilation.cpp > CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/mcts_swig.engine.bot.random.randombot_autogen/mocs_compilation.cpp.i

engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/mcts_swig.engine.bot.random.randombot_autogen/mocs_compilation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/mcts_swig.engine.bot.random.randombot_autogen/mocs_compilation.cpp.s"
	cd /home/david/projects/mcts_swig/build/engine/bot/random && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/david/projects/mcts_swig/build/engine/bot/random/mcts_swig.engine.bot.random.randombot_autogen/mocs_compilation.cpp -o CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/mcts_swig.engine.bot.random.randombot_autogen/mocs_compilation.cpp.s

engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/src/randombot.cpp.o: engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/flags.make
engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/src/randombot.cpp.o: ../engine/bot/random/src/randombot.cpp
engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/src/randombot.cpp.o: engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/david/projects/mcts_swig/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/src/randombot.cpp.o"
	cd /home/david/projects/mcts_swig/build/engine/bot/random && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/src/randombot.cpp.o -MF CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/src/randombot.cpp.o.d -o CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/src/randombot.cpp.o -c /home/david/projects/mcts_swig/engine/bot/random/src/randombot.cpp

engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/src/randombot.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/src/randombot.cpp.i"
	cd /home/david/projects/mcts_swig/build/engine/bot/random && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/david/projects/mcts_swig/engine/bot/random/src/randombot.cpp > CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/src/randombot.cpp.i

engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/src/randombot.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/src/randombot.cpp.s"
	cd /home/david/projects/mcts_swig/build/engine/bot/random && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/david/projects/mcts_swig/engine/bot/random/src/randombot.cpp -o CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/src/randombot.cpp.s

# Object files for target mcts_swig.engine.bot.random.randombot
mcts_swig_engine_bot_random_randombot_OBJECTS = \
"CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/mcts_swig.engine.bot.random.randombot_autogen/mocs_compilation.cpp.o" \
"CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/src/randombot.cpp.o"

# External object files for target mcts_swig.engine.bot.random.randombot
mcts_swig_engine_bot_random_randombot_EXTERNAL_OBJECTS =

engine/bot/random/libmcts_swig.engine.bot.random.randombot.so: engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/mcts_swig.engine.bot.random.randombot_autogen/mocs_compilation.cpp.o
engine/bot/random/libmcts_swig.engine.bot.random.randombot.so: engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/src/randombot.cpp.o
engine/bot/random/libmcts_swig.engine.bot.random.randombot.so: engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/build.make
engine/bot/random/libmcts_swig.engine.bot.random.randombot.so: engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/david/projects/mcts_swig/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX shared library libmcts_swig.engine.bot.random.randombot.so"
	cd /home/david/projects/mcts_swig/build/engine/bot/random && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/build: engine/bot/random/libmcts_swig.engine.bot.random.randombot.so
.PHONY : engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/build

engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/clean:
	cd /home/david/projects/mcts_swig/build/engine/bot/random && $(CMAKE_COMMAND) -P CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/cmake_clean.cmake
.PHONY : engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/clean

engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/depend:
	cd /home/david/projects/mcts_swig/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/david/projects/mcts_swig /home/david/projects/mcts_swig/engine/bot/random /home/david/projects/mcts_swig/build /home/david/projects/mcts_swig/build/engine/bot/random /home/david/projects/mcts_swig/build/engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : engine/bot/random/CMakeFiles/mcts_swig.engine.bot.random.randombot.dir/depend

