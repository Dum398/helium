all: ./build/helium

./build/helium: ./src/main.c
	mkdir -p ./build
	gcc ./src/main.c -o ./build/helium

clean:
	rm -f ./build/helium
	rm -f datasec.datasec
	rm -f bssec.bssec
	rm -f textsec.textsec
	rm -f out.asm
	rm -f out
