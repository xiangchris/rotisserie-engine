INCLUDE ?= ./src/third_party/include/
CXXFLAGS ?= -std=c++17 -I$(INCLUDE) -I$(INCLUDE)SDL2/ -I$(INCLUDE)SDL_image/ -I$(INCLUDE)SDL_mixer/ -I$(INCLUDE)SDL_ttf/ -I$(INCLUDE)LuaBridge/ -I$(INCLUDE)box2d/ -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lSDL2main -llua5.4 -O3

# Compile the main executable
game_engine_linux: src/first_party/main.cpp
	clang++ $(CXXFLAGS) src/first_party/*.cpp $(INCLUDE)box2d/**/*.cpp -o game_engine_linux
