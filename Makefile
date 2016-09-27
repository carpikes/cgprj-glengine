all: build project

build:
	@mkdir -p build
	cd build && cmake ..

project: 
	cd build && make -j8

run: build project
	cd build && src/demo

clean:
	rm -rf build/

again: clean build project

.PHONY: project run clean again
