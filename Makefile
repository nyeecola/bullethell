all: lint compile run

lint:
	cppcheck . --suppress=passedByValue --suppress=memleak --enable=warning,performance,portability --error-exitcode=5

compile:
	mkdir -p build
	g++ src/main.cpp -O0 -o build/main.exe -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lm -Wconversion -Wall -Wextra -pedantic -std=c++11 -lGL -g

clean:
	rm -rf build

run:
	./build/main.exe
