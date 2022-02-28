windres resource.rc -o res.o &
g++ -o RandomPicker Source.cpp Application.cpp res.o -mwindows -std=c++17 -lgdi32 -lcomctl32 &
RandomPicker.exe