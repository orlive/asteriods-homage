CC	 = clang++
CPPFLAGS = -std=c++17 -frelaxed-template-template-args $(sdl2-config --cflags) -I/usr/local/include/SDL2 -D_THREAD_SAFE

OBJ	= main.o tools.o sdlEngine.o vector.o force.o base.o rock.o laser.o particle.o ship.o
LIBS	= -L/usr/local/lib -lSDL2 $(sdl2-config --libs) -lSDL2_TTF

HEADERS = base.h force.h gameWorld.h laser.h particle.h rock.h sdlEngine.h ship.h tools.h vector.h my_header.h my_other_header.h

$(BUILD_DIR)/%.o: %.c $(HEADERS)
	$(CC) $< -o $@ $(CFLAGS) $(LIBS)

.cpp.o: $(OBJ)
	$(CC) $(CPPFLAGS) -c $< -o $@

asteroids: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

cleanall:
	rm *.o; make

