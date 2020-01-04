##Simple makefile

main: main.cpp
	g++ main.cpp vec3.h ray.h hitable.h sphere.h hitablelist.h camera.h randomize.h -o main.exe