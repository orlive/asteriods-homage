CC	 = clang++
CPPFLAGS = -std=c++17 -frelaxed-template-template-args $(sdl2-config --cflags) -I/usr/local/include/SDL2 -D_THREAD_SAFE

OBJ	= main.o
LIBS	= -L/usr/local/lib -lSDL2 $(sdl2-config --libs)

.cpp.o: $(OBJ)
	$(CC) $(CPPFLAGS) -c $< -o $@

asteroids: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

