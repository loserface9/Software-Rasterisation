CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O3
CLIBFLAGS = -lm `pkg-config --cflags --libs sdl3`

SRC = .
OBJ = ./obj
SRCS = $(wildcard $(SRC)/*.c)
OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
HDRS = $(wildcard $(SRC)/*.h)

all: $(OBJ)/outRelease $(OBJ)

$(OBJ)/outRelease: $(OBJS)
	$(CC) $(CFLAGS) $(CLIBFLAGS) $(OBJS) -o $(OBJ)/outRelease

# Syntax - targets ...: target-pattern: prereq-patterns ...
# In the case of the first target, foo.o, the target-pattern matches foo.o and sets the "stem" to be "foo".
# It then replaces the '%' in prereq-patterns with that stem
$(OBJS): $(OBJ)/%.o: %.c $(OBJ) $(SRC)/constants.h
	$(CC) -c $(CFLAGS) $(CLIBFLAGS) $< -o $@
# ./obj/main.o: ./main.c
# 	$(CC) -c $(CFLAGS) $(CLIBFLAGS) main.c -o ./obj/main.o

$(OBJ):
	mkdir $(OBJ)

clean:
	rm -r $(OBJ)
