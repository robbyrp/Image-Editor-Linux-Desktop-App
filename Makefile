CXX = g++
CC = gcc

EXE = editor
FRONTEND = frontend
IMGUI_DIR = $(FRONTEND)/imgui

SOURCES = 	$(FRONTEND)/main.cpp \
          	$(FRONTEND)/application/application.cpp \
          	$(FRONTEND)/state/editor_state.cpp \
          	$(FRONTEND)/rendering/rendering_engine.cpp \
          	$(FRONTEND)/ui/ui_callbacks.cpp \
          	$(FRONTEND)/ui/ui_windows.cpp

SOURCES += $(wildcard $(IMGUI_DIR)/*.cpp)
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

# Backend
SOURCES += $(wildcard backend/*.c)
OBJS = $(SOURCES:.cpp=.o)
OBJS := $(OBJS:.c=.o)
UNAME_S := $(shell uname -s)
LINUX_GL_LIBS = -lGL

CFLAGS = -g -Wall -Wformat -fopenmp
CFLAGS += -fsanitize=address -fno-omit-frame-pointer

CXXFLAGS = -std=c++23 -fopenmp -fsanitize=address -fno-omit-frame-pointer
CXXFLAGS += -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends \
           -I$(FRONTEND) -I$(FRONTEND)/application -I$(FRONTEND)/state \
           -I$(FRONTEND)/rendering -I$(FRONTEND)/ui \
           -g -Wall -Wformat
LIBS += -lcurl -fopenmp


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
	CFLAGS += `pkg-config --cflags glfw3`
endif


ifeq ($(OS), Windows_NT)
	ECHO_MESSAGE = "MinGW"
	LIBS += -lglfw3 -lgdi32 -lopengl32 -limm32

	CXXFLAGS += `pkg-config --cflags glfw3`
	CFLAGS += `pkg-config --cflags glfw3`
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)


$(EXE): $(OBJS)
	$(CXX)  -fsanitize=address -o $@ $^ $(CXXFLAGS) $(LIBS)


clean:
	rm -f $(EXE) $(OBJS)