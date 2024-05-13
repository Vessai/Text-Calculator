#
# Windows puts spaces in paths. Spaces stop wildcard expansions. So I have to use a makefile :)
#

CXX = g++  	# Set C++ compiler
OUTPUT_OPTION=-MMD -MP -o $@	# -MMD generates makefile dependencies for user headers; -MP adds empty "phony" targets for each dependency, to get around make problems
CPPFLAGS=-g # $(shell root-config --cflags)	\ 
LDFLAGS=-g # $(shell root-config --ldflags)	| root-config doesn't seem to be readily available in mingw64. I'll sort this if I use a lot of libraries.
LDLIBS= # $(shell root-config --libs)		/

EXE=calc.exe
SRC=$(wildcard src/*.cpp)	# Get all c++ source files
OBJ=$(SRC:src/%.cpp=.obj/%.o)		# The target outputs for compiled individual files (*.o)
DEP=$(SRC:src/%.cpp=.obj/%.d)		# The target outputs for makefile dependency info (*.d)

.DEFAULT_GOAL := calc  # Configuration "default" just builds the main (calc) target

all: clean test  # Delete all previous objects, recompile and run the program.

# Create main.exe from all compiled *.o objects.
calc: $(OBJ)
	$(CXX) $(LDFLAGS) -o $(EXE) $(OBJ) $(LDLIBS)

.obj/%.o: src/%.cpp
	$(CXX) $(CPPFLAGS) -c $< $(OUTPUT_OPTION)
	
test: calc
	$(EXE)

clean:
	echo "OBJ Variable: $(OBJ)"
	echo "DEP Variable: $(DEP)"
	rm -rf $(EXE) $(OBJ) $(DEP)