FILE= main.c
LIB= `pkg-config --cflags --libs xcb xcb-randr`

default: compile
compile:
	gcc ${FILE} ${LIB} -o minfo.o
