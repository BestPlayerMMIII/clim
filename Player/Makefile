# Project: Player for CLIM files
# Makefile by https://github.com/BestPlayerMMIII

CXX      = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -O2
INCLUDES = -Iinclude
SRC_DIR  = src
BUILD_DIR = build
OBJ      = $(BUILD_DIR)/main.o $(BUILD_DIR)/bit_reader.o $(BUILD_DIR)/binary_reader.o \
           $(BUILD_DIR)/cluster_decoder.o $(BUILD_DIR)/clim_decoder.o $(BUILD_DIR)/frame.o \
           $(BUILD_DIR)/clim_player.o $(BUILD_DIR)/audio_player.o $(BUILD_DIR)/exit_handler.o \
           $(BUILD_DIR)/filesystem.o
BIN      = player

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(OBJ) -o $(BIN)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN)
