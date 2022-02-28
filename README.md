# Random Item Picker
Pick a random item from a list.<br>
You can insert, delete and pick items with given controls.

# Building
<kbd>
  windres resource.rc -o res.o <br>
  g++ -o RandomPicker Source.cpp Application.cpp res.o -std=c++17 -mwindows -lgdi32 -lcomctl32
</kbd><br>

Or simply run "run.bat"

# Additional info
  - -std=c++17 is optional, use 11 or 14 if you want.
