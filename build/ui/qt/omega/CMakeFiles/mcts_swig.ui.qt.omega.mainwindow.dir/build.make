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
include ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/compiler_depend.make

# Include the progress variables for this target.
include ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/progress.make

# Include the compile flags for this target's objects.
include ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/flags.make

ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/mcts_swig.ui.qt.omega.mainwindow_autogen/mocs_compilation.cpp.o: ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/flags.make
ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/mcts_swig.ui.qt.omega.mainwindow_autogen/mocs_compilation.cpp.o: ui/qt/omega/mcts_swig.ui.qt.omega.mainwindow_autogen/mocs_compilation.cpp
ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/mcts_swig.ui.qt.omega.mainwindow_autogen/mocs_compilation.cpp.o: ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/david/projects/mcts_swig/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/mcts_swig.ui.qt.omega.mainwindow_autogen/mocs_compilation.cpp.o"
	cd /home/david/projects/mcts_swig/build/ui/qt/omega && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/mcts_swig.ui.qt.omega.mainwindow_autogen/mocs_compilation.cpp.o -MF CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/mcts_swig.ui.qt.omega.mainwindow_autogen/mocs_compilation.cpp.o.d -o CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/mcts_swig.ui.qt.omega.mainwindow_autogen/mocs_compilation.cpp.o -c /home/david/projects/mcts_swig/build/ui/qt/omega/mcts_swig.ui.qt.omega.mainwindow_autogen/mocs_compilation.cpp

ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/mcts_swig.ui.qt.omega.mainwindow_autogen/mocs_compilation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/mcts_swig.ui.qt.omega.mainwindow_autogen/mocs_compilation.cpp.i"
	cd /home/david/projects/mcts_swig/build/ui/qt/omega && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/david/projects/mcts_swig/build/ui/qt/omega/mcts_swig.ui.qt.omega.mainwindow_autogen/mocs_compilation.cpp > CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/mcts_swig.ui.qt.omega.mainwindow_autogen/mocs_compilation.cpp.i

ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/mcts_swig.ui.qt.omega.mainwindow_autogen/mocs_compilation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/mcts_swig.ui.qt.omega.mainwindow_autogen/mocs_compilation.cpp.s"
	cd /home/david/projects/mcts_swig/build/ui/qt/omega && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/david/projects/mcts_swig/build/ui/qt/omega/mcts_swig.ui.qt.omega.mainwindow_autogen/mocs_compilation.cpp -o CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/mcts_swig.ui.qt.omega.mainwindow_autogen/mocs_compilation.cpp.s

ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/src/mainwindow.cpp.o: ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/flags.make
ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/src/mainwindow.cpp.o: ../ui/qt/omega/src/mainwindow.cpp
ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/src/mainwindow.cpp.o: ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/david/projects/mcts_swig/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/src/mainwindow.cpp.o"
	cd /home/david/projects/mcts_swig/build/ui/qt/omega && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/src/mainwindow.cpp.o -MF CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/src/mainwindow.cpp.o.d -o CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/src/mainwindow.cpp.o -c /home/david/projects/mcts_swig/ui/qt/omega/src/mainwindow.cpp

ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/src/mainwindow.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/src/mainwindow.cpp.i"
	cd /home/david/projects/mcts_swig/build/ui/qt/omega && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/david/projects/mcts_swig/ui/qt/omega/src/mainwindow.cpp > CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/src/mainwindow.cpp.i

ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/src/mainwindow.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/src/mainwindow.cpp.s"
	cd /home/david/projects/mcts_swig/build/ui/qt/omega && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/david/projects/mcts_swig/ui/qt/omega/src/mainwindow.cpp -o CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/src/mainwindow.cpp.s

# Object files for target mcts_swig.ui.qt.omega.mainwindow
mcts_swig_ui_qt_omega_mainwindow_OBJECTS = \
"CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/mcts_swig.ui.qt.omega.mainwindow_autogen/mocs_compilation.cpp.o" \
"CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/src/mainwindow.cpp.o"

# External object files for target mcts_swig.ui.qt.omega.mainwindow
mcts_swig_ui_qt_omega_mainwindow_EXTERNAL_OBJECTS =

ui/qt/omega/libmcts_swig.ui.qt.omega.mainwindow.so: ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/mcts_swig.ui.qt.omega.mainwindow_autogen/mocs_compilation.cpp.o
ui/qt/omega/libmcts_swig.ui.qt.omega.mainwindow.so: ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/src/mainwindow.cpp.o
ui/qt/omega/libmcts_swig.ui.qt.omega.mainwindow.so: ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/build.make
ui/qt/omega/libmcts_swig.ui.qt.omega.mainwindow.so: ui/qt/omega/libui_mainwindow.a
ui/qt/omega/libmcts_swig.ui.qt.omega.mainwindow.so: ui/qt/omega/libmcts_swig.ui.qt.omega.boarddialog.so
ui/qt/omega/libmcts_swig.ui.qt.omega.mainwindow.so: ui/qt/omega/libui_mainwindow.a
ui/qt/omega/libmcts_swig.ui.qt.omega.mainwindow.so: engine/bot/mcts/libmcts_swig.engine.bot.mcts.mctsbot.so
ui/qt/omega/libmcts_swig.ui.qt.omega.mainwindow.so: engine/bot/random/libmcts_swig.engine.bot.random.randombot.so
ui/qt/omega/libmcts_swig.ui.qt.omega.mainwindow.so: ui/qt/omega/libmcts_swig.ui.qt.omega.qtbotwrapper.so
ui/qt/omega/libmcts_swig.ui.qt.omega.mainwindow.so: engine/bot/base/libmcts_swig.engine.bot.base.aibotbase.so
ui/qt/omega/libmcts_swig.ui.qt.omega.mainwindow.so: ui/qt/omega/libmcts_swig.ui.qt.omega.canvas.so
ui/qt/omega/libmcts_swig.ui.qt.omega.mainwindow.so: /usr/lib/x86_64-linux-gnu/libQt5Widgets.so.5.9.5
ui/qt/omega/libmcts_swig.ui.qt.omega.mainwindow.so: engine/game/omega/libmcts_swig.engine.game.omega.omega.so
ui/qt/omega/libmcts_swig.ui.qt.omega.mainwindow.so: ui/qt/omega/libmcts_swig.ui.qt.omega.hexagon.so
ui/qt/omega/libmcts_swig.ui.qt.omega.mainwindow.so: /usr/lib/x86_64-linux-gnu/libQt5Gui.so.5.9.5
ui/qt/omega/libmcts_swig.ui.qt.omega.mainwindow.so: /usr/lib/x86_64-linux-gnu/libQt5Core.so.5.9.5
ui/qt/omega/libmcts_swig.ui.qt.omega.mainwindow.so: ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/david/projects/mcts_swig/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX shared library libmcts_swig.ui.qt.omega.mainwindow.so"
	cd /home/david/projects/mcts_swig/build/ui/qt/omega && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/build: ui/qt/omega/libmcts_swig.ui.qt.omega.mainwindow.so
.PHONY : ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/build

ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/clean:
	cd /home/david/projects/mcts_swig/build/ui/qt/omega && $(CMAKE_COMMAND) -P CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/cmake_clean.cmake
.PHONY : ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/clean

ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/depend:
	cd /home/david/projects/mcts_swig/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/david/projects/mcts_swig /home/david/projects/mcts_swig/ui/qt/omega /home/david/projects/mcts_swig/build /home/david/projects/mcts_swig/build/ui/qt/omega /home/david/projects/mcts_swig/build/ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : ui/qt/omega/CMakeFiles/mcts_swig.ui.qt.omega.mainwindow.dir/depend
