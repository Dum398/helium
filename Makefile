all: ./build/helium

./build/helium: ./src/main.c
	mkdir -p ./build
	gcc ./src/main.c -o ./build/helium

clean:
	rm -f ./build/helium