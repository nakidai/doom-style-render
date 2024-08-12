all:
	cmake -S . -B ./build
	cmake --build build

main: all
	./build/cl_main.exe

mem: all
	./build/ts_mem.exe
