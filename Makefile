all:
	cmake -G "MinGW Makefiles" -S . -B ./build
	make -C ./build

main: all
	./build/cl_main.exe

mem: all
	./build/ts_mem.exe