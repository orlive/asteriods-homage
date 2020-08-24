CC	 = clang++
CPPFLAGS = -std=c++17 -frelaxed-template-template-args $(sdl2-config --cflags) -I/usr/local/include/SDL2 -D_THREAD_SAFE -Ilua-5.3.5_MacOS1013_lib/include  -ILuaBridge/Source/ -ILuaBridge/Source/LuaBridge/
FRAMEWORKS:= -framework Foundation

OBJ	= main.o tools.o sdlEngine.o vector.o force.o base.o rock.o laser.o particle.o ship.o config.o MacBundle.o
LIBS	= -L/usr/local/lib -lSDL2 $(sdl2-config --libs) -lSDL2_TTF -Llua-5.3.5_MacOS1013_lib -llua53

HEADERS = base.h force.h config.h laser.h particle.h rock.h sdlEngine.h ship.h tools.h vector.h my_header.h my_other_header.h

$(BUILD_DIR)/%.o: %.c $(HEADERS)
	$(CC) $< -o $@ $(CFLAGS) $(LIBS)

%.o: %.mm
	$(CC) -c $< -o $@

%.o: %.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

asteroids: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(FRAMEWORKS)

app:
	make; \
	rm -fr asteroids.app; \
	tar xvfz asteroids.app.template.tgz asteroids.app; \
	cp asteroids  asteroids.app/Contents/MacOS/; \
	cp config.lua asteroids.app/Contents/Resources/; \
        cp -rp assets asteroids.app/Contents/Resources/; \
	~/Qt5.14.1/5.14.1/clang_64/bin/macdeployqt asteroids.app; \
    	install_name_tool -change "@rpath/liblua53.dylib" "@executable_path/../Frameworks/liblua53.dylib" asteroids.app/Contents/MacOS/asteroids; \
    	install_name_tool -change "liblua53.dylib" "@executable_path/../Frameworks/liblua53.dylib" asteroids.app/Contents/MacOS/asteroids; \
	cp lua-5.3.5_MacOS1013_lib/liblua53.dylib asteroids.app/Contents/Frameworks/; \
	otool -L asteroids.app/Contents/MacOS/asteroids; \
	cp -frp asteroids.app /Applications/
 
cleanall:
	rm *.o; \
	rm -rf asteroids.app; \
	make

