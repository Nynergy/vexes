### VARIABLES ###

# Compiler
CC := g++

# Source and build output directories
SRC_DIR := src
OBJ_DIR := obj
DEMO_DIR := demos

# Pass preprocessor flags
CPPFLAGS := -Iinclude # link include directory

# Add compiler flags
CFLAGS := -Wall -Werror

# Add linker flags
LDFLAGS := -L.

# Link against third party libraries
LDLIBS := -lncurses -ltinfo

### RECIPES ###

# Indicate when a rule does not produce any target output
.PHONY: all clean

all: $(DEMO_DIR)/demo1 $(DEMO_DIR)/demo2 $(DEMO_DIR)/demo3 $(DEMO_DIR)/demo4 $(DEMO_DIR)/demo5

# Linking Phase
$(DEMO_DIR)/demo1: $(OBJ_DIR)/demo1.o $(OBJ_DIR)/vexes.o | $(DEMO_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(DEMO_DIR)/demo2: $(OBJ_DIR)/demo2.o $(OBJ_DIR)/vexes.o | $(DEMO_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(DEMO_DIR)/demo3: $(OBJ_DIR)/demo3.o $(OBJ_DIR)/vexes.o | $(DEMO_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(DEMO_DIR)/demo4: $(OBJ_DIR)/demo4.o $(OBJ_DIR)/vexes.o | $(DEMO_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(DEMO_DIR)/demo5: $(OBJ_DIR)/demo5.o $(OBJ_DIR)/vexes.o | $(DEMO_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Compiling Phase
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# If build directory does not exist, make it
$(OBJ_DIR):
	mkdir $@

# If demos directory does not exist, make it
$(DEMO_DIR):
	mkdir $@

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(DEMO_DIR)
