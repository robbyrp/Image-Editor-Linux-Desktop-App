# compiler setup
CC=gcc 
CFLAGS=-Wall -Wextra -std=c99
CDEBUG_FLAGS=$(CFLAGS) -ggdb3

# define targets
SOURCES = $(wildcard sources/*.c)
TARGETS = ./image_editor
DEBUG_INPUT = input.txt
DEBUG_OUTPUT=output.txt
DEBUG_ERRPUT=errput.txt
DEBUG_OUTPUT_MEM=valgrind.txt
CHECKER_OUTPUT=checker.txt

build : $(TARGETS)

image_editor: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGETS) -lm

pack:
	zip -FSr 313CA_Pana_Robert_Ionut_Tema3.zip README Makefile *.c *.h

clean:
	rm -f $(TARGETS)

test:
	clear
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGETS)
	./$(TARGETS) < $(DEBUG_INPUT) 1>$(DEBUG_OUTPUT) 2>$(DEBUG_ERRPUT)
	cat $(DEBUG_OUTPUT)

memcheck:
	clear
	$(CC) $(CDEBUG_FLAGS) $(SOURCES) -o $(TARGETS)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGETS) < $(DEBUG_INPUT) 1>$(DEBUG_OUTPUT) 2>$(DEBUG_OUTPUT_MEM)
	
	cat $(DEBUG_OUTPUT_MEM)
savecheck:
	clear
	$(CC) $(CDEBUG_FLAGS) $(SOURCES) -o $(TARGETS) -lm
	sudo valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGETS) < $(DEBUG_INPUT) 1>$(DEBUG_OUTPUT) 2>$(DEBUG_OUTPUT_MEM)

checker:
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGETS)
	./check --task $(TARGETS) > $(CHECKER_OUTPUT)

.PHONY: pack clean