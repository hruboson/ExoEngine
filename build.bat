:: Create build folder for compiled code
if not exist build mkdir build

:: Run Cmake
cd build
cmake -S ../ -B . -G "MinGW Makefiles"

:: Compile using MinGW
mingw32-make.exe && mingw32-make.exe Shaders

:: copy ImGui file for gui window positions
copy ..\imgui.ini .

:: Finally run exoengine.exe
:: My antivirus keeps popping up while running this automatically so I commented it
:: start exoengine.exe

cd ..