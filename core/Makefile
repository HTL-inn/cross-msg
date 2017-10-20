# I'm using the g++ and not the llvm c compiler... I'm sorry
CC_FLAGS = -Wall -Iinclude -lpthread

# Environment
SHELL=/bin/sh
MKDIR=mkdir
CP=cp
GREP=grep
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Directorys
BUILDDIR = build/
SOURCEDIR = src/

# File names
EXEC = core
SOURCES = $(wildcard $(SOURCEDIR)*.cpp)
OBJECTS_TMP = $(SOURCES:.cpp=.o)
OBJECTS = $(OBJECTS_TMP:$(SOURCEDIR)%=$(BUILDDIR)%)

%.cpp %.o:
	$(CCC) -c $(@:$(BUILDDIR)%.o=$(SOURCEDIR)%.cpp) $(CC_FLAGS) -o $@


# Main target
$(EXEC): $(OBJECTS)
	$(CCC) $(OBJECTS) -o $(EXEC)



# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)
