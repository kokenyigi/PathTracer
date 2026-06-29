if exist PathTracer.exe del PathTracer.exe

REM build mappa létrehozása
if not exist build mkdir build

REM belépés
cd build

REM CMake konfigurálás
cmake .. -G "Ninja"

REM Build
cmake --build . 

cd ..

if exist PathTracer.exe start PathTracer.exe



