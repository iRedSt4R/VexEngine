mkdir Build
cd Build
cmake ../ -G "Visual Studio 17 2022" -A x64 -DCMAKE_CONFIGURATION_TYPES="Debug;Release;DebugCooker;ReleaseCooker"