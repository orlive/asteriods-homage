Homage to asteriods

(I) compile:
  (a) OSX:

    (1) open Terminal and goto project-folder

    (2) Install sdl2 und sdl2-ttf:

        brew install SDL2
        brew install SDL2_TTF
 
    (3) Install LuaBridge - Sources:

        git clone https://github.com/vinniefalco/LuaBridge.git
      
    (4) Install lua-Libraries:

        Homepage:
          https://sourceforge.net/projects/luabinaries/files/5.3.5/Other%20Libraries/
  
        Download in TERMINAL:
          curl -L https://sourceforge.net/projects/luabinaries/files/5.3.5/Other%20Libraries/lua-5.3.5_MacOS1013_lib.tar.gz --output lua-5.3.5_MacOS1013_lib.tar.gz

        Extract:
          mkdir lua-5.3.5_MacOS1013_lib
          cd lua-5.3.5_MacOS1013_lib
          tar xvzf ../lua-5.3.5_MacOS1013_lib.tar.gz
          cd ..
    (5) make

    (6) to generate asteriods.app Qt5.14.1 needs to be installed in ~/Qt5.14.1

        make app
    
