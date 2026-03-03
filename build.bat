@echo off

cd Development/build
cmake .. || exit 1
cmake --build . --config Release || exit 1
cd ..