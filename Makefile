CC = gcc
CFLAGS = -Wall -Wextra -pedantic
CLIBFLAGS = -lm `pkg-config --cflags --libs sdl3`

BPATH = ./build

objects = $(BPATH)/main.o $(BPATH)/matrices.o $(BPATH)/make_frame.o $(BPATH)/bitmap.o $(BPATH)/ascii_rendering.o

$(BPATH)/outRelease: $(objects)
	$(CC) $(CFLAGS) $(CLIBFLAGS) $(objects) -o $(BPATH)/outRelease

# Syntax - targets ...: target-pattern: prereq-patterns ...
# In the case of the first target, foo.o, the target-pattern matches foo.o and sets the "stem" to be "foo".
# It then replaces the '%' in prereq-patterns with that stem
$(objects): $(BPATH)/%.o: %.c
	$(CC) -c $(CFLAGS) $(CLIBFLAGS) $^ -o $@
# ./build/main.o: ./main.c
# 	$(CC) -c $(CFLAGS) $(CLIBFLAGS) main.c -o ./build/main.o
