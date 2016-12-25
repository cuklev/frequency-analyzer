#include<pulse/simple.h>
#include<stdio.h>

#define SAMPLE_RATE 44100

short buffer[SAMPLE_RATE];

int main() {
	pa_simple *s;
	pa_sample_spec ss;

	ss.format = PA_SAMPLE_S16NE;
	ss.channels = 1;
	ss.rate = SAMPLE_RATE;

	int error;

	s = pa_simple_new(NULL,
			"Test",
			PA_STREAM_RECORD,
			NULL,
			"Music",
			&ss,
			NULL,
			NULL,
			&error
			);

	if(!s) {
		printf("Error\n");
		return 1;
	}

	for(;;) {
		if(pa_simple_read(s, buffer, sizeof(buffer), &error) < 0) {
			printf("Error\n");
			break;
		}
	}

	if(s) pa_simple_free(s);
}
