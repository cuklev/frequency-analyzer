#include "stream.h"

#include<pulse/simple.h>

static pa_simple *s;
static pa_sample_spec ss = {
	.format = PA_SAMPLE_S16NE,
	.channels = 1,
	.rate = 44100
};

void open_stream(void) {
	s = pa_simple_new(NULL,
		"Analyzer",
		PA_STREAM_RECORD,
		NULL,
		"Recording",
		&ss,
		NULL,
		NULL,
		NULL);
}

void read_stream(void *buffer, int buf_size) {
	pa_simple_read(s, buffer, buf_size, NULL);
}

void free_stream(void) {
	if(s) pa_simple_free(s);
}
