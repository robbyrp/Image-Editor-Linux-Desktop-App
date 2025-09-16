
CXX = g++
# CXX = clang++

EXE = editor
END_FRONT = endfront
IMGUI_DIR = $(END_FRONT)/imgui
SOURCES = $(END_FRONT)/main.cpp $(END_FRONT)/funcs.cpp
SOURCES += $(wildcard $(IMGUI_DIR)/*.cpp)
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp
# Backend
SOURCES += $(wildcard backend/*.c)
OBJS = $(SOURCES:.cpp=.o)
OBJS := $(OBJS:.c=.o)
UNAME_S := $(shell uname -s)
LINUX_GL_LIBS = -lGL

CXXFLAGS = -std=c++11 -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends
CXXFLAGS += -g -Wall -Wformat
LIBS =

##---------------------------------------------------------------------
## OPENGL ES
##---------------------------------------------------------------------

## This assumes a GL ES library available in the system, e.g. libGLESv2.so
# CXXFLAGS += -DIMGUI_IMPL_OPENGL_ES2
# LINUX_GL_LIBS = -lGLESv2

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LIBS += $(LINUX_GL_LIBS) `pkg-config --static --libs glfw3`

	CXXFLAGS += `pkg-config --cflags glfw3`
	CFLAGS = $(CXXFLAGS)
endif


ifeq ($(OS), Windows_NT)
	ECHO_MESSAGE = "MinGW"
	LIBS += -lglfw3 -lgdi32 -lopengl32 -limm32

	CXXFLAGS += `pkg-config --cflags glfw3`
	CFLAGS = $(CXXFLAGS)
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------


%.o:endfront/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:endfront/imgui/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:endfront/imgui/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:backend/%.c
	$(CXX) $(CXXFLAGS) -c -o $@ $<


all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)


$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)


clean:
	rm -f $(EXE) $(OBJS)
