CONFIG += noconsole c++11
#CONFIG -= app_bundle
CONFIG -= qt
INCLUDEPATH = /usr/local/include/SDL2
LIBS	+= -L/usr/local/lib -lSDL2 -lSDL2_TTF -lPNG
ICON = logo.icns

TARGET = asteroids
TEMPLATE = app

SOURCES += \
        base.cpp \
        force.cpp \
        laser.cpp \
        main.cpp \
        particle.cpp \
        rock.cpp \
        sdlEngine.cpp \
        ship.cpp \
        tools.cpp \
        vector.cpp

HEADERS += \
        base.h \
        force.h \
        gameWorld.h \
        laser.h \
        particle.h \
        rock.h \
        sdlEngine.h \
        ship.h \
        tools.h \
        vector.h

RESOURCES +=
