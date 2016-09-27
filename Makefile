all: build project

build:
	@mkdir -p build
	cd build && cmake ..

project: 
	cd build && make -j8

run: build project
	cd build && src/demo
