#include "stream.h"

#include<stdio.h>

short buffer[256];

int main(void) {
	open_stream();
	read_stream(buffer, sizeof(buffer));
	free_stream();
}
