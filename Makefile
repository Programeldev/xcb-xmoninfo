FILE= main.c
LIB= `pkg-config --cflags --libs xcb xcb-randr xcb-atom`

default: compile
compile:
	gcc ${FILE} ${LIB} -o xmoninfo
