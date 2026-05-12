CXX = g++
EMCXX = emcc

# Native Flags
CXXFLAGS = -std=c++17 -Wall -I./include -DGL_GLEXT_PROTOTYPES
LDFLAGS = -lGL -lGLU -lglut

# Emscripten Flags
EMFLAGS = -s WASM=1 -s USE_SDL=2 -s USE_WEBGL2=1 -I./include --preload-file assets -o web/MyVillageV2.js

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
TARGET = $(BIN_DIR)/MyVillageV2

all: native

native: $(TARGET)

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

web:
	$(EMCXX) $(SOURCES) $(EMFLAGS)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) web/MyVillageV2.js web/MyVillageV2.wasm web/MyVillageV2.data

.PHONY: all native web clean
