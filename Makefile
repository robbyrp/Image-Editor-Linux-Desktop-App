CXX = g++
CC = gcc

EXE = editor
END_FRONT = endfront
IMGUI_DIR = $(END_FRONT)/imgui

SOURCES = 	$(END_FRONT)/main.cpp \
          	$(END_FRONT)/application/application.cpp \
          	$(END_FRONT)/state/editor_state.cpp \
          	$(END_FRONT)/rendering/rendering_engine.cpp \
          	$(END_FRONT)/ui/ui_callbacks.cpp \
          	$(END_FRONT)/ui/ui_windows.cpp

SOURCES += $(wildcard $(IMGUI_DIR)/*.cpp)
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

# Backend
SOURCES += $(wildcard backend/*.c)
OBJS = $(SOURCES:.cpp=.o)
OBJS := $(OBJS:.c=.o)
UNAME_S := $(shell uname -s)
LINUX_GL_LIBS = -lGL

CFLAGS = -g -Wall -Wformat 
CFLAGS += -fsanitize=address -fno-omit-frame-pointer

CXXFLAGS = -std=c++23 -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends \
           -I$(END_FRONT) -I$(END_FRONT)/application -I$(END_FRONT)/state \
           -I$(END_FRONT)/rendering -I$(END_FRONT)/ui \
           -g -Wall -Wformat
CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
LIBS += -lcurl


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
