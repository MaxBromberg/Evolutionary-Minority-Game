# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.9

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2017.3\bin\cmake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2017.3\bin\cmake\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "C:\Users\K-2SO\OneDrive\University\Thesis\Minority Game Repository"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "C:\Users\K-2SO\OneDrive\University\Thesis\Minority Game Repository"

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	"C:\Program Files\JetBrains\CLion 2017.3\bin\cmake\bin\cmake.exe" -E echo "No interactive CMake dialog available."
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	"C:\Program Files\JetBrains\CLion 2017.3\bin\cmake\bin\cmake.exe" -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start "C:\Users\K-2SO\OneDrive\University\Thesis\Minority Game Repository\CMakeFiles" "C:\Users\K-2SO\OneDrive\University\Thesis\Minority Game Repository\CMakeFiles\progress.marks"
	$(MAKE) -f CMakeFiles\Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start "C:\Users\K-2SO\OneDrive\University\Thesis\Minority Game Repository\CMakeFiles" 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles\Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles\Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles\Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles\Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named Minority_Game_Programming

# Build rule for target.
Minority_Game_Programming: cmake_check_build_system
	$(MAKE) -f CMakeFiles\Makefile2 Minority_Game_Programming
.PHONY : Minority_Game_Programming

# fast build rule for target.
Minority_Game_Programming/fast:
	$(MAKE) -f CMakeFiles\Minority_Game_Programming.dir\build.make CMakeFiles/Minority_Game_Programming.dir/build
.PHONY : Minority_Game_Programming/fast

#=============================================================================
# Target rules for targets named Minority_Game_UnitTests

# Build rule for target.
Minority_Game_UnitTests: cmake_check_build_system
	$(MAKE) -f CMakeFiles\Makefile2 Minority_Game_UnitTests
.PHONY : Minority_Game_UnitTests

# fast build rule for target.
Minority_Game_UnitTests/fast:
	$(MAKE) -f CMakeFiles\Minority_Game_UnitTests.dir\build.make CMakeFiles/Minority_Game_UnitTests.dir/build
.PHONY : Minority_Game_UnitTests/fast

Evolutionary_Minority_Game.obj: Evolutionary_Minority_Game.cpp.obj

.PHONY : Evolutionary_Minority_Game.obj

# target to build an object file
Evolutionary_Minority_Game.cpp.obj:
	$(MAKE) -f CMakeFiles\Minority_Game_Programming.dir\build.make CMakeFiles/Minority_Game_Programming.dir/Evolutionary_Minority_Game.cpp.obj
	$(MAKE) -f CMakeFiles\Minority_Game_UnitTests.dir\build.make CMakeFiles/Minority_Game_UnitTests.dir/Evolutionary_Minority_Game.cpp.obj
.PHONY : Evolutionary_Minority_Game.cpp.obj

Evolutionary_Minority_Game.i: Evolutionary_Minority_Game.cpp.i

.PHONY : Evolutionary_Minority_Game.i

# target to preprocess a source file
Evolutionary_Minority_Game.cpp.i:
	$(MAKE) -f CMakeFiles\Minority_Game_Programming.dir\build.make CMakeFiles/Minority_Game_Programming.dir/Evolutionary_Minority_Game.cpp.i
	$(MAKE) -f CMakeFiles\Minority_Game_UnitTests.dir\build.make CMakeFiles/Minority_Game_UnitTests.dir/Evolutionary_Minority_Game.cpp.i
.PHONY : Evolutionary_Minority_Game.cpp.i

Evolutionary_Minority_Game.s: Evolutionary_Minority_Game.cpp.s

.PHONY : Evolutionary_Minority_Game.s

# target to generate assembly for a file
Evolutionary_Minority_Game.cpp.s:
	$(MAKE) -f CMakeFiles\Minority_Game_Programming.dir\build.make CMakeFiles/Minority_Game_Programming.dir/Evolutionary_Minority_Game.cpp.s
	$(MAKE) -f CMakeFiles\Minority_Game_UnitTests.dir\build.make CMakeFiles/Minority_Game_UnitTests.dir/Evolutionary_Minority_Game.cpp.s
.PHONY : Evolutionary_Minority_Game.cpp.s

Evolutionary_Minority_Game_Test.obj: Evolutionary_Minority_Game_Test.cpp.obj

.PHONY : Evolutionary_Minority_Game_Test.obj

# target to build an object file
Evolutionary_Minority_Game_Test.cpp.obj:
	$(MAKE) -f CMakeFiles\Minority_Game_UnitTests.dir\build.make CMakeFiles/Minority_Game_UnitTests.dir/Evolutionary_Minority_Game_Test.cpp.obj
.PHONY : Evolutionary_Minority_Game_Test.cpp.obj

Evolutionary_Minority_Game_Test.i: Evolutionary_Minority_Game_Test.cpp.i

.PHONY : Evolutionary_Minority_Game_Test.i

# target to preprocess a source file
Evolutionary_Minority_Game_Test.cpp.i:
	$(MAKE) -f CMakeFiles\Minority_Game_UnitTests.dir\build.make CMakeFiles/Minority_Game_UnitTests.dir/Evolutionary_Minority_Game_Test.cpp.i
.PHONY : Evolutionary_Minority_Game_Test.cpp.i

Evolutionary_Minority_Game_Test.s: Evolutionary_Minority_Game_Test.cpp.s

.PHONY : Evolutionary_Minority_Game_Test.s

# target to generate assembly for a file
Evolutionary_Minority_Game_Test.cpp.s:
	$(MAKE) -f CMakeFiles\Minority_Game_UnitTests.dir\build.make CMakeFiles/Minority_Game_UnitTests.dir/Evolutionary_Minority_Game_Test.cpp.s
.PHONY : Evolutionary_Minority_Game_Test.cpp.s

main.obj: main.cpp.obj

.PHONY : main.obj

# target to build an object file
main.cpp.obj:
	$(MAKE) -f CMakeFiles\Minority_Game_Programming.dir\build.make CMakeFiles/Minority_Game_Programming.dir/main.cpp.obj
.PHONY : main.cpp.obj

main.i: main.cpp.i

.PHONY : main.i

# target to preprocess a source file
main.cpp.i:
	$(MAKE) -f CMakeFiles\Minority_Game_Programming.dir\build.make CMakeFiles/Minority_Game_Programming.dir/main.cpp.i
.PHONY : main.cpp.i

main.s: main.cpp.s

.PHONY : main.s

# target to generate assembly for a file
main.cpp.s:
	$(MAKE) -f CMakeFiles\Minority_Game_Programming.dir\build.make CMakeFiles/Minority_Game_Programming.dir/main.cpp.s
.PHONY : main.cpp.s

test_main.obj: test_main.cpp.obj

.PHONY : test_main.obj

# target to build an object file
test_main.cpp.obj:
	$(MAKE) -f CMakeFiles\Minority_Game_UnitTests.dir\build.make CMakeFiles/Minority_Game_UnitTests.dir/test_main.cpp.obj
.PHONY : test_main.cpp.obj

test_main.i: test_main.cpp.i

.PHONY : test_main.i

# target to preprocess a source file
test_main.cpp.i:
	$(MAKE) -f CMakeFiles\Minority_Game_UnitTests.dir\build.make CMakeFiles/Minority_Game_UnitTests.dir/test_main.cpp.i
.PHONY : test_main.cpp.i

test_main.s: test_main.cpp.s

.PHONY : test_main.s

# target to generate assembly for a file
test_main.cpp.s:
	$(MAKE) -f CMakeFiles\Minority_Game_UnitTests.dir\build.make CMakeFiles/Minority_Game_UnitTests.dir/test_main.cpp.s
.PHONY : test_main.cpp.s

# Help Target
help:
	@echo The following are some of the valid targets for this Makefile:
	@echo ... all (the default if no target is provided)
	@echo ... clean
	@echo ... depend
	@echo ... Minority_Game_Programming
	@echo ... Minority_Game_UnitTests
	@echo ... edit_cache
	@echo ... rebuild_cache
	@echo ... Evolutionary_Minority_Game.obj
	@echo ... Evolutionary_Minority_Game.i
	@echo ... Evolutionary_Minority_Game.s
	@echo ... Evolutionary_Minority_Game_Test.obj
	@echo ... Evolutionary_Minority_Game_Test.i
	@echo ... Evolutionary_Minority_Game_Test.s
	@echo ... main.obj
	@echo ... main.i
	@echo ... main.s
	@echo ... test_main.obj
	@echo ... test_main.i
	@echo ... test_main.s
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles\Makefile.cmake 0
.PHONY : cmake_check_build_system

