all:
	gcc -std=gnu99 main.c -o xnotify -lX11 -lXft `pkg-config --cflags freetype2`

clean:
	rm -rf xnotify
