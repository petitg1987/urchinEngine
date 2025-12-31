# Environment setup 
## Linux
* Install required libraries:
  ```
  wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo tee /etc/apt/trusted.gpg.d/lunarg.asc
  sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-noble.list http://packages.lunarg.com/vulkan/lunarg-vulkan-noble.list
  sudo apt install vulkan-tools libvulkan-dev vulkan-validationlayers-dev spirv-tools vulkan-sdk
  sudo apt install qtbase5-dev libopenal-dev libogg-dev libvorbis-dev libfreetype6-dev libcppunit-dev libssl-dev libnghttp2-dev libbrotli-dev
  ```

## Windows
* Install Msys2 application and libraries:
  * Install with default values: https://www.msys2.org/
  * Add in PATH variable: "C:\msys64\clang64\bin"
  * In clang64.exe, execute `pacman -Syu` (console will restart) and re-execute again `pacman -Syu`
  * In clang64.exe, execute:
    ```
    pacman -S mingw-w64-clang-x86_64-toolchain mingw-w64-clang-x86_64-cmake mingw-w64-clang-x86_64-libvorbis mingw-w64-clang-x86_64-libogg mingw-w64-clang-x86_64-make make
    ```
* Install Freetype library:
  * Unzip https://download.savannah.gnu.org/releases/freetype/freetype-2.14.1.tar.gz in: "C:\msys64\home\greg"
  * Execute:
    ```
    cd ~/freetype-2.14.1 && ./configure --with-zlib=no && make && make install
    cp -r ~/freetype-2.14.1/include/. /clang64/include/
    ```
* Install OpenAL library:
  * Install with "Yes, launch the OpenAL redist" checked: https://www.openal.org/downloads/OpenAL11CoreSDK.zip
  * Copy lib (Win64) & include from "C:\Program Files (x86)\OpenAL 1.1 SDK" in "C:\msys64\clang64\lib" & "C:\msys64\clang64\include\AL"
  * Rename the copied lib from OpenAL32.lib to libOpenAL32.dll.a
  * *Info*: OpenAL32.dll is in "C:\Windows\System32\OpenAL32.dll" and soft_oal.dll can be downloaded on https://community.pcgamingwiki.com/files/file/7-openal-soft/
  * Uninstall OpenAL and OpenAL SDK applications
* Install Vulkan library:
  * Install Vulkan SDK in default folder (C:\VulkanSDK): https://vulkan.lunarg.com/sdk/home#windows
  * Copy lib & include respectively in "C:\msys64\clang64\lib" and in "C:\msys64\clang64\include"
  * Execute (adapt SDK version):
  ```
  ln -s ${HOMEDRIVE}/VulkanSDK/1.4.335.0/Bin /home/greg/vulkan-bin
  echo 'export PATH=$PATH:/home/greg/vulkan-bin' >> ~/.bashrc
  ```
  * *Info*: vuklan-1.dll can be found in the runtime zip: https://vulkan.lunarg.com/sdk/home#windows
  * *Info*: Do not uninstall Vulkan SDK application because it is required for validation layer & during release process to compile shaders
* In clang64.exe, install curl library (custom static library only for HTTP/HTTPS protocols):
  ```
  pacman -S unzip openssl-devel
  rm /tmp/curl/ -rf && mkdir -p /tmp/curl/ && cd /tmp/curl/
  wget -P /tmp/curl/ https://curl.haxx.se/download/curl-8.11.0.zip
  unzip curl-8.11.0.zip && cd /tmp/curl/curl-8.11.0
  ./configure CPPFLAGS=-DNGHTTP2_STATICLIB --disable-shared --enable-static --prefix=/usr/local --disable-ldap --disable-sspi --disable-ftp --disable-file --disable-dict --disable-telnet --disable-tftp --disable-hsts --disable-alt-svc --disable-rtsp --disable-pop3 --disable-imap --disable-smtp --disable-gopher --disable-smb --without-librtmp --without-libidn2 --without-libpsl --with-ssl --with-nghttp2
  make V=1
  make install #create library in /usr/local/lib/libcurl.a
  mv /clang64/lib/libcurl.a /clang64/lib/libcurl.a_backup
  cp /usr/local/lib/libcurl.a /clang64/lib/
  cp -r /usr/local/include/curl/ /clang64/include/
  cat /usr/local/lib/pkgconfig/libcurl.pc | grep "Libs.private" #Display required dependencies
  ```

# CLion configuration
## Build configuration
* Configure compiler to Clang (File > Settings > Build,Execution,Deployment > Toolchains)
  * Toolset (Windows only): `C:\msys64\clang64`
  * C Compiler: `/usr/bin/clang` / `C:\msys64\clang64\bin\clang.exe`
  * C++ Compiler: `/usr/bin/clang++` / `C:\msys64\clang64\bin\clang++.exe`
* Add CMake profiles (File > Settings > Build,Execution,Deployment > CMake)
  * Profile **Debug**:
    * Name: `Debug`
    * Build type: `Debug`
    * Build directory: `.build/debug`
    * Build options: `-j 14`
  * Profile **Release**:
    * Name: `RelWithDebInfo`
    * Build type: `RelWithDebInfo`
    * Build directory: `.build/release`
    * Build options: `-j 14`
* Add CMake applications
  * Application **testExecutor (monkey)**:
    * Target/executable: `testExecutor`
    * Program arguments: `unit integration monkey`
  * Application **testExecutor**:
    * Target/executable: `testExecutor`
    * Program arguments: `unit integration`    
  * Application **urchinMapEditor**:
    * Target/executable: `urchinMapEditor`

## TODO menu
* Scope: `(file[urchinEngine]:*.h||file[urchinEngine]:*.cpp||file[urchinEngine]:*.inl||file[urchinEngine]:*.frag||file[urchinEngine]:*.vert||file[urchinEngine]:*.comp||file[urchinEngine]:*.properties)&&!file[urchinEngine]:.build//*&&!file[urchinEngine]:.idea//*`

# Development tips
## Error handling
* Use exception (`throw std::runtime_error(...)`) for methods which could be wrongly used by the final user
* Use UserAuthorityException for errors where user has authority to fix the problem
* Use assert (`assert(...)`) for methods which could be wrongly used by the engine developer
  * *Note:* surround assert with `#ifdef URCHIN_DEBUG`/`#endif` when condition has bad performance
* Use logger (`Logger::instance()->logError(...)`) when the result of an algorithm is not the one excepted
  * *Note:* surround logger call with `if (DebugCheck::additionalChecksEnable()) {`/`}` when condition has bad performance

## Check when "new" operator is called
* Use following source code and add a debug point:
    ```
    void* operator new(std::size_t sz) {
        return std::malloc(sz);
    }

    void* operator new[](std::size_t sz) {
        return std::malloc(sz);
    }
    ```

# Profiling
## CPU profiler: Flame Graph
* Install:
    ```
    sudo apt install linux-tools-generic
    cd ~/various/tools/ && git clone https://github.com/brendangregg/FlameGraph.git
    ```
* Launch application in debug mode
* Record (replace 'myAppName'):
    ```
    sudo perf record -F 5000 --strict-freq -p $(pidof myAppName) -g -- sleep 25
    ```
* Convert in SVG:
    ```
    sudo perf script > out.perf
    ~/various/tools/FlameGraph/stackcollapse-perf.pl out.perf > out.folded
    ~/various/tools/FlameGraph/flamegraph.pl out.folded > app.svg
    ```

## GPU profiler: Nsight
* Install:
  * Download Linux 'run' from https://developer.nvidia.com/nsight-graphics
  * Execute file and install in ~/various/tools/nsight
* Launch Nsight (as root ? Nsight not working on Ubuntu 21.10):
    ```
    ~/various/tools/nsight/host/linux-desktop-nomad-x64/ngfx-ui
    ```

## VRAM visualizer
* Launch:
    ```
    nvidia-smi -lms=1000
    ```

## RAM profiler: Valgrind & Massif
* Install:
    ```
    sudo apt install valgrind massif-visualizer
    ```
* Launch Valgrind with massif tool (replace 'myAppName --dev'):
    ```
    /usr/bin/valgrind --tool=massif ./myAppName --dev
    ```
* Execute Massif visualizer and select file produced by Valgrind
    ```
    massif-visualizer
    ```
