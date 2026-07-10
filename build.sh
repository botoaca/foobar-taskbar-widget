set -xe

gcc -o foobar_taskbar_widget main.c -lraylib -lopengl32 -lgdi32 -lwinmm
