# Mingw install
* Mingw:
  * Download: https://sourceforge.net/projects/mingw-w64/
  * Select architecture: X86_64, select path: C:\MinGW
  * Add env. variable PATH = C:\MinGW\mingw64\bin
* Msys:
  * Download: https://sourceforge.net/projects/mingw/files/MSYS/Base/msys-core/msys-1.0.11/MSYS-1.0.11.exe/download
  * Select path: C:\msys\1.0
  * Check: open C:\msys\1.0\bin\m.exe and type "g++ --version"
* Freetpye:
  * Download: https://download.savannah.gnu.org/releases/freetype/freetype-2.9.tar.gz
  * Copy in C:\msys\1.0\home\greg\
  * ./configure --with-zlib=no
  * Execute: make && make install
  * Copy "objs\\\.libs" & "include" respectively in C:\MinGW\mingw64\lib & C:\MinGW\mingw64\x86_64-w64-mingw32\include
* Glew: 
  * Download: https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0-win32.zip/download
  * Copy lib (x84) & include respectively in C:\MinGW\mingw64\lib & C:\MinGW\mingw64\x86_64-w64-mingw32\include
  * Note: Necessary to rename .lib file into .a file (prefixed with lib) to match library name.
* OpenAL:
  * Download and execute windows installer: https://www.openal.org/downloads/oalinst.zip 
  * Download: https://www.openal.org/downloads/OpenAL11CoreSDK.zip
  * Copy lib (Win64) & include respectively in C:\MinGW\mingw64\lib & C:\MinGW\mingw64\x86_64-w64-mingw32\include\AL
  * Note: Necessary to rename .lib file into .a file (prefixed with lib) to match library name.
* Libsnd:
  * Donwload and execute windows installer: http://www.mega-nerd.com/libsndfile/files/libsndfile-1.0.28-w64-setup.exe
  * Copy lib & include respectively in C:\MinGW\mingw64\lib & C:\MinGW\mingw64\x86_64-w64-mingw32\include
  * Note: Necessary to rename .lib file into .a file (prefixed with lib) to match library name.
* SFML:
  * Donwload CMake (https://github.com/Kitware/CMake/releases/download/v3.14.4/cmake-3.14.4-win64-x64.zip) and install (copy files in C:\msys\1.0)
  * Donwload SFML source: https://www.sfml-dev.org/files/SFML-2.5.1-sources.zip
  * Copy in C:\msys\1.0\home\greg\
  * Execute CMake-gui:
    * Define source code: C:/msys/1.0/home/greg/SFML-2.5.1
    * Define build location: C:/msys/1.0/home/greg/SFML-2.5.1/build
    * Click "configure" and select "MinGW-makefile"
    * Set SFML_USE_STATIC_STD_LIBS=true and BUILD_SHARED_LIBS=false
    * Add CMAKE_CXX_STANDARD entry (String) with value: 17
    * Click "configure"
    * Click "generate"
  * Execute mingw32-make in "C:/msys/1.0/home/greg/SFML-2.5.1/build"
  * Copy lib & include respectively in C:\MinGW\mingw64\lib & C:\MinGW\mingw64\x86_64-w64-mingw32\include
* Libcurl:
  * Download curl for Windows 64bits (https://curl.haxx.se/windows/dl-7.64.1_1/curl-7.64.1_1-win64-mingw.zip). Note: associate DLL can be also downloaded on this page (OpenSSL 1.1.1)
  * Copy lib & include respectively in C:\MinGW\mingw64\lib & C:\MinGW\mingw64\x86_64-w64-mingw32\include
* Path configuration (only to start app. inside IntelliJ):
  * Add in "Path" Windows env. variable, the DLL directory:
    * `C:\Users\greg\Desktop\Project\greenCity\.build\release\urchinEngine\3dEngine`
    * `C:\Users\greg\Desktop\Project\greenCity\.build\release\urchinEngine\AIEngine`
    * `C:\Users\greg\Desktop\Project\greenCity\.build\release\urchinEngine\common`
    * `C:\Users\greg\Desktop\Project\greenCity\.build\release\urchinEngine\mapHandler`
    * `C:\Users\greg\Desktop\Project\greenCity\.build\release\urchinEngine\physicsEngine`
    * `C:\Users\greg\Desktop\Project\greenCity\.build\release\urchinEngine\soundEngine`
    * `C:\Users\greg\Desktop\Project\greenCity\.build\release\urchinEngine\networkEngine`
    * `C:\Users\greg\Desktop\Project\greenCity\.build\debug\urchinEngine\3dEngine`
    * `C:\Users\greg\Desktop\Project\greenCity\.build\debug\urchinEngine\AIEngine`
    * `C:\Users\greg\Desktop\Project\greenCity\.build\debug\urchinEngine\common`
    * `C:\Users\greg\Desktop\Project\greenCity\.build\debug\urchinEngine\mapHandler`
    * `C:\Users\greg\Desktop\Project\greenCity\.build\debug\urchinEngine\physicsEngine`
    * `C:\Users\greg\Desktop\Project\greenCity\.build\debug\urchinEngine\soundEngine`
    * `C:\Users\greg\Desktop\Project\greenCity\.build\debug\urchinEngine\networkEngine`
    * `C:\Users\greg\Desktop\Project\greenCity\release\windows\resources\lib`
