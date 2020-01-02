##Simple makefile

main: main.cpp vec3
	g++ main.cpp vec3.h -o main.exe

vec3:
	g++ -c vec3.cpp