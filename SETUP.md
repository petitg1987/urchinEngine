# Linux
* Install libraries:
  * `sudo apt install qt5-default qtbase5-dev libglew-dev libopenal-dev libsndfile1-dev libfreetype6-dev libcppunit-dev libcurl4-gnutls-dev`
* Configure system to activate core file:
  * Edit `/etc/security/limits.conf`
  * Add / change to: `*               soft    core            unlimited`
  * Restart computer
  * Check with `ulimit -a`: "core file size" must be unlimited

# Windows
* Msys2:
  * Download: https://sourceforge.net/projects/msys2/
  * Setup with defautl values
  * Add env. variable PATH: C:\msys64\mingw64\bin
  * In **mingw64.exe** (not msys2.exe): `Pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-toolchain mingw64/mingw-w64-x86_64-gcc`
* Freetype:
  * Download: https://download.savannah.gnu.org/releases/freetype/freetype-2.10.1.tar.gz
  * Copy in: C:\msys64\home\greg
  * Execute: `./configure --with-zlib=no && make && make install`
  * Copy "include" in C:\msys64\mingw64\x86_64-w64-mingw32\include
* Glew:
  * Download: https://sourceforge.net/projects/glew/files/glew/2.2.0/glew-2.2.0-win32.zip/download
  * Copy lib (x84) & include respectively in C:\msys64\mingw64\lib & C:\msys64\mingw64\x86_64-w64-mingw32\include
  * Rename .lib files into .a file prefixed with lib: libglew32.a and libglew32s.a
* OpenAL:
  * Download and execute windows installer: https://www.openal.org/downloads/oalinst.zip
  * Download: https://www.openal.org/downloads/OpenAL11CoreSDK.zip
  * Copy lib (Win64) & include respectively in C:\msys64\mingw64\lib & C:\msys64\mingw64\x86_64-w64-mingw32\include\AL
  * Rename .lib file into .a file prefixed with lib: libOpenAL32.a
* Libsnd:
  * Download: http://www.mega-nerd.com/libsndfile/files/libsndfile-1.0.28-w64.zip
  * Copy lib & include respectively in C:\msys64\mingw64\lib & C:\msys64\mingw64\x86_64-w64-mingw32\include
* GLFW:
  * Download: https://github.com/glfw/glfw/releases/download/3.3.2/glfw-3.3.2.bin.WIN64.zip
  * Copy lib (mingw-w64) & include respectively in C:\msys64\mingw64\lib & C:\msys64\mingw64\x86_64-w64-mingw32\include
* Libcurl:
  * Check curl version: `curl --version` (curl should be installed via 'mingw-w64-x86_64-cmake' package)
  * Download curl for Windows 64bits matching the version found (example: https://curl.haxx.se/windows/dl-7.73.0_1/curl-7.73.0_1-win64-mingw.zip)
  * Copy include in C:\msys64\mingw64\x86_64-w64-mingw32\include (don't need to copy lib because already installed via 'mingw-w64-x86_64-cmake' package)
  * The required DLL 'libcurl-4.dll', 'libbrotlicommon.dll', 'libbrotlidec.dll', 'libiconv-2.dll', 'libidn2-0.dll', 'libintl-8.dll', 'libnghttp2-14.dll', 'libpsl-5.dll', 'libssh2-1.dll', 'libssl-1_1-x64.dll', 'libunistring-2.dll', 'libzstd.dll' and 'zlib1.dll' are in C:\msys64\mingw64\bin
  * Tips: the 'libssl-1_1-x64.dll' and 'libcrypto-1_1-x64.dll' are also in the corresponding OpenSSL archive (example: https://curl.se/windows/dl-7.73.0_1/openssl-1.1.1h_1-win64-mingw.zip)
* Zip:
  * Download zip (http://stahlworks.com/dev/zip.exe)
  * Copy file in C:\msys64\mingw64\bin
* Configure Clion:
  * In File > Settings... > Build, Execution, Deployment > Toolchains, Environment: C:\msys64\mingw64
