OS_ID := $(shell grep '^ID=' /etc/os-release | cut -d'=' -f2 | tr -d '"')

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

deps:
	@if [ "$(OS_ID)" = "ubuntu" ]; then \
		sudo apt-get update && sudo apt-get install -y ld nasm gcc build-essential; \
	elif [ "$(OS_ID)" = "fedora" ]; then \
		sudo dnf install -y ld nasm gcc make; \
	elif [ "$(OS_ID)" = "arch" ]; then \
		sudo pacman -Sy --noconfirm base-devel nasm gcc; \
	else \
		echo "Unsupported OS. Please install dependencies manually: nasm, gcc, make"; \
	fi

install:
	make clean && make
	@if [ -z "$(DESTDIR)" ]; then \
		echo "DESTDIR is not set. Installing to /usr/bin/helium (may require sudo)"; \
		install -Dm755 ./build/helium /usr/bin/helium; \
	else \
		install -Dm755 ./build/helium "$(DESTDIR)/usr/bin/helium"; \
	fi