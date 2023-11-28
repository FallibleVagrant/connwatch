#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
# You will need SDL2 (http://www.libsdl.org):
# Linux:
#   apt-get install libsdl2-dev
# Mac OS X:
#   brew install sdl2
# MSYS2:
#   pacman -S mingw-w64-i686-SDL2
#

#CXX = g++
#CXX = clang++

EXE = connwatch
SRC_DIR = ./src
OBJ_DIR = ./obj
SOURCES += $(SRC_DIR)/main.cpp $(SRC_DIR)/info_window.cpp $(SRC_DIR)/connections_window.cpp $(SRC_DIR)/action_window.cpp $(SRC_DIR)/window_demon.cpp $(SRC_DIR)/ncurses_funcs.cpp $(SRC_DIR)/controller_god.cpp $(SRC_DIR)/model_angel.cpp $(SRC_DIR)/config_bar.cpp $(SRC_DIR)/warn_window.cpp $(SRC_DIR)/text_animations.cpp $(SRC_DIR)/alarum_window.cpp $(SRC_DIR)/networking_agent.cpp $(SRC_DIR)/scrolling_machine.cpp $(SRC_DIR)/main_window_header.cpp $(SRC_DIR)/log_window.cpp
OBJS = $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))
UNAME_S := $(shell uname -s)

CXXFLAGS = -I./include -g
CXXFLAGS += -Wall
LIBS = -L./lib -lncurses

CXXDEBUGFLAGS = -g
CXXDEBUGFLAGS += $(CXXFLAGS)

TEST_SOURCES += $(filter-out $(SRC_DIR)/main.cpp, $(SOURCES)) $(SRC_DIR)/test.cpp
TEST_OBJS += $(filter-out $(OBJ_DIR)/main.o, $(OBJS)) $(OBJ_DIR)/test.o

##---------------------------------------------------------------------
## OPENGL ES
##---------------------------------------------------------------------

## This assumes a GL ES library available in the system, e.g. libGLESv2.so
# CXXFLAGS += -DIMGUI_IMPL_OPENGL_ES2
# LINUX_GL_LIBS = -lGLESv2
## If you're on a Raspberry Pi and want to use the legacy drivers,
## use the following instead:
# LINUX_GL_LIBS = -L/opt/vc/lib -lbrcmGLESv2

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"

	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"
	LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo `sdl2-config --libs`
	LIBS += -L/usr/local/lib -L/opt/local/lib

	CXXFLAGS += `sdl2-config --cflags`
	CXXFLAGS += -I/usr/local/include -I/opt/local/include
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(OS), Windows_NT)
    ECHO_MESSAGE = "MinGW"
    LIBS += -lgdi32 -lopengl32 -limm32 `pkg-config --static --libs sdl2`

    CXXFLAGS += `pkg-config --cflags sdl2`
    CFLAGS = $(CXXFLAGS)
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

$(OBJ_DIR)/%.o:$(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(EXE) $(OBJS) $(TEST_OBJS) test debug

debug: $(OBJS)
	$(CXX) -o $@ $^ $(CXXDEBUGFLAGS) $(LIBS)

test: $(TEST_OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)
