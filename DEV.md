# Environment setup 
## Linux
* Install libraries from Ubuntu package: `sudo apt install qt5-default qtbase5-dev libglew-dev libopenal-dev libsndfile1-dev libfreetype6-dev libcppunit-dev`
* Install curl library (custom static library only for HTTP/HTTPS protocols):
  ```
  sudo apt install libssl-dev libnghttp2-dev
  rm /tmp/curl/ -rf && mkdir -p /tmp/curl/ && cd /tmp/curl/
  wget -P /tmp/curl/ https://curl.haxx.se/download/curl-7.74.0.zip
  unzip curl-7.74.0.zip && cd /tmp/curl/curl-7.74.0
  ./configure --disable-shared --enable-static --prefix=/usr/local --disable-ldap --disable-sspi --disable-ftp --disable-file --disable-dict --disable-telnet --disable-tftp --disable-rtsp --disable-pop3 --disable-imap --disable-smtp --disable-gopher --disable-smb --without-librtmp --without-libidn --with-ssl --with-nghttp2
  make
  sudo make install #create library in /usr/local/lib/libcurl.a
  sudo mv /usr/lib/x86_64-linux-gnu/libcurl.a /usr/lib/x86_64-linux-gnu/libcurl.a_backup #In case libcurl is installed via package 'libcurl4-gnutls-dev'
  cat /usr/local/lib/pkgconfig/libcurl.pc | grep "Libs.private" #Display required dependencies
  ```
* Configure system to activate core file:
  * Edit `/etc/security/limits.conf`
  * Add / change to: `*               soft    core            unlimited`
  * Restart computer
  * Check with `ulimit -a`: "core file size" must be unlimited

## Windows
* Install msys2 application:
  * Download: https://sourceforge.net/projects/msys2/
  * Execute the installer with the default values
  * Add in PATH variable: "C:\msys64\mingw64\bin"
  * In mingw64.exe (not msys2.exe): `Pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-toolchain mingw64/mingw-w64-x86_64-gcc`
* Install freetype library:
  * Download: https://download.savannah.gnu.org/releases/freetype/freetype-2.10.1.tar.gz
  * Copy in: "C:\msys64\home\greg"
  * Execute: `./configure --with-zlib=no && make && make install`
  * Copy include in "C:\msys64\mingw64\x86_64-w64-mingw32\include"
  * *Info*: libfreetype-6.dll is in "C:\msys64\mingw64\bin"
* Install openAL library:
  * Download: https://www.openal.org/downloads/OpenAL11CoreSDK.zip
  * Execute the installer and at the end, check "Yes, launch the OpenAL redist"
  * Copy lib (Win64) & include from "C:\Program Files (x86)\OpenAL 1.1 SDK" in "C:\msys64\mingw64\lib" & "C:\msys64\mingw64\x86_64-w64-mingw32\include\AL"
  * Rename the copied lib from OpenAL32.lib to libOpenAL32.dll.a
  * *Info*: OpenAL32.dll is in "C:\Windows\System32\OpenAL32.dll" and "soft_oal.dll" can be downloaded on https://community.pcgamingwiki.com/files/file/7-openal-soft/
  * Uninstall OpenAL and OpenAL SDK applications
* Install sndfile library:
  * Download: http://www.mega-nerd.com/libsndfile/files/libsndfile-1.0.28-w64.zip
  * Copy lib & include respectively in "C:\msys64\mingw64\lib" & "C:\msys64\mingw64\x86_64-w64-mingw32\include"
  * *Info*: libsndfile-1.dll is in "bin\" folder
* Install curl library (custom static library only for HTTP/HTTPS protocols):
  ```
  Pacman -S unzip openssl-devel
  rm /tmp/curl/ -rf && mkdir -p /tmp/curl/ && cd /tmp/curl/
  wget -P /tmp/curl/ https://curl.haxx.se/download/curl-7.74.0.zip
  unzip curl-7.74.0.zip && cd /tmp/curl/curl-7.74.0
  CPPFLAGS="-DNGHTTP2_STATICLIB" ./configure --disable-shared --enable-static --prefix=/usr/local --disable-ldap --disable-sspi --disable-ftp --disable-file --disable-dict --disable-telnet --disable-tftp --disable-rtsp --disable-pop3 --disable-imap --disable-smtp --disable-gopher --disable-smb --without-librtmp --without-libidn --with-ssl --with-nghttp2
  make V=1
  make install #create library in /usr/local/lib/libcurl.a
  mv /mingw64/lib/libcurl.a /mingw64/lib/libcurl.a_backup #In case libcurl is installed via package "mingw-w64-x86_64-cmake"
  cp /usr/local/lib/libcurl.a /mingw64/lib/
  cp -r /usr/local/include/curl/ /mingw64/x86_64-w64-mingw32/include/
  cat /usr/local/lib/pkgconfig/libcurl.pc | grep "Libs.private" #Display required dependencies
  ```
* Configure Clion:
  * In File > Settings... > Build, Execution, Deployment > Toolchains, Environment: C:\msys64\mingw64

## CLion configuration
* Add CMake profiles (File > Settings -> Build,Execution,Deployment > CMake)
  * Profile **Debug**:
    * Name: `Debug`
    * Build type: `Debug`
    * Generation path: `.build/debug`
  * Profile **Release**:
    * Name: `RelWithDebInfo`
    * Build type: `RelWithDebInfo`
    * Generation path: `.build/release`
* Add CMake applications
  * Application **testExecutor**:
    * Target/executable: `testExecutor`
    * Program arguments: `unit integration monkey`
  * Application **urchinMapEditor**:
    * Target/executable: `urchinMapEditor`

# Development tips
* Error handling:
  * Use exception (`throw std::runtime_error(...)`) for methods which could be wrongly used by the final user
  * Use assert (`assert(...)`) for methods which could be wrongly used by the engine developer
    * *Note:* surround assert with `#ifndef NDEBUG`/`#endif` when condition has bad performance
  * Use logger (`Logger::instance()->logError(...)`) when the result of an algorithm is not the one excepted
    * *Note:* surround logger call with `if (DebugCheck::instance()->additionalChecksEnable()) {`/`}` when condition has bad performance
* Verify when `new` operator is called:
  * Use following source code and add a debug point:
      ```
      void* operator new(std::size_t sz){
          return std::malloc(sz);
      }

      void* operator new[](std::size_t sz) {
          return std::malloc(sz);
      }
      ```
* Coordinates used:
  * 3D (e.g.: *3D models, Physics rigid bodies, 3D Nav mesh*)
    - X+ (left), X- (right)
    - Y+ (top), Y- (bottom)
    - Z+ (near), Z- (far)
  * 2D top view (e.g.: *2D Nav mesh*)
    - X+ (left), X- (right)
    - Y+ (far), Y- (near)