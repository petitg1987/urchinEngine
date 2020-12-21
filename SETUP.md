# Linux
* Install libraries:
  * `sudo apt install qt5-default qtbase5-dev libglew-dev libopenal-dev libsndfile1-dev libfreetype6-dev libcppunit-dev libcurl4-gnutls-dev`
* Configure system to activate core file:
  * Edit `/etc/security/limits.conf`
  * Add / change to: `*               soft    core            unlimited`
  * Restart computer
  * Check with `ulimit -a`: "core file size" must be unlimited

# Windows
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
  * Download and execute windows installer: http://www.mega-nerd.com/libsndfile/files/libsndfile-1.0.28-w64-setup.exe
  * Copy lib & include respectively in C:\MinGW\mingw64\lib & C:\MinGW\mingw64\x86_64-w64-mingw32\include
  * Note: Necessary to rename .lib file into .a file (prefixed with lib) to match library name.
* GLFW:
  * Download: https://github.com/glfw/glfw/releases/download/3.3.2/glfw-3.3.2.bin.WIN64.zip
  * Copy lib (mingw-w64) & include respectively in C:\MinGW\mingw64\lib & C:\MinGW\mingw64\x86_64-w64-mingw32\include
* Libcurl:
  * Download curl for Windows 64bits (https://curl.haxx.se/windows/dl-7.64.1_1/curl-7.64.1_1-win64-mingw.zip). Note: associate DLL can be also downloaded on this page (OpenSSL 1.1.1)
  * Copy lib & include respectively in C:\MinGW\mingw64\lib & C:\MinGW\mingw64\x86_64-w64-mingw32\include
* Zip:
  * Download zip (http://stahlworks.com/dev/zip.exe)
  * Copy file in C:\MinGW\mingw64\bin
* Path configuration (only to start application inside CLion):
  * For debug build, add the following directories in "PATH" environment variable:
    * `C:\msys\1.0\home\greg\project\laserRiddle\.build\release\urchinEngine\3dEngine`
    * `C:\msys\1.0\home\greg\project\laserRiddle\.build\release\urchinEngine\AIEngine`
    * `C:\msys\1.0\home\greg\project\laserRiddle\.build\release\urchinEngine\common`
    * `C:\msys\1.0\home\greg\project\laserRiddle\.build\release\urchinEngine\mapHandler`
    * `C:\msys\1.0\home\greg\project\laserRiddle\.build\release\urchinEngine\physicsEngine`
    * `C:\msys\1.0\home\greg\project\laserRiddle\.build\release\urchinEngine\soundEngine`
    * `C:\msys\1.0\home\greg\project\laserRiddle\.build\release\urchinEngine\networkEngine`
    * `C:\msys\1.0\home\greg\project\laserRiddle\release\windows\resources\lib`
  * For release build, add the following directories in "PATH" environment variable:
    * `C:\msys\1.0\home\greg\project\laserRiddle\.build\debug\urchinEngine\3dEngine`
    * `C:\msys\1.0\home\greg\project\laserRiddle\.build\debug\urchinEngine\AIEngine`
    * `C:\msys\1.0\home\greg\project\laserRiddle\.build\debug\urchinEngine\common`
    * `C:\msys\1.0\home\greg\project\laserRiddle\.build\debug\urchinEngine\mapHandler`
    * `C:\msys\1.0\home\greg\project\laserRiddle\.build\debug\urchinEngine\physicsEngine`
    * `C:\msys\1.0\home\greg\project\laserRiddle\.build\debug\urchinEngine\soundEngine`
    * `C:\msys\1.0\home\greg\project\laserRiddle\.build\debug\urchinEngine\networkEngine`
    * `C:\msys\1.0\home\greg\project\laserRiddle\release\windows\resources\lib`