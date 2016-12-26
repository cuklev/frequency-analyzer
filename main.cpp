#include "stream.h"
#include "fft.hpp"

#include<ncurses.h>

#include<vector>
#include<complex>

const int MIN_SAMPLE_COUNT = 32;
const int MAX_SAMPLE_COUNT = 1 << 16;

const int FREQUENCY_CHANGE_STEP = 100;
const int THRESHOLD_CHANGE_STEP = 100;

const int SAMPLE_RATE = 44100;

short buffer[MAX_SAMPLE_COUNT];

int main() {
	initscr();
	raw();
	noecho();

	int min_frequency = 0;
	int max_frequency = 20000;
	int threshold = 1000;
	int sample_count = MIN_SAMPLE_COUNT;

	std::vector<std::complex<double>> data(sample_count);

	bool running = true;
	while(running) {
		clear();
		printw("Threshold: %d\n", threshold);
		printw("Frequency: %d - %d Hz\n", min_frequency, max_frequency);
		printw("Sample count: %d\n", sample_count);
		printw("====================\n");
		refresh();

		for(int i = 0; i < sample_count / 2; ++i) {
			double freq = (double) SAMPLE_RATE / sample_count * i;
			if(freq < min_frequency || freq > max_frequency)
				continue;
			double amp = abs(data[i]) * 2 / sample_count;
			if(amp < threshold)
				continue;
			printw("%lfHz\n", freq);
		}

		switch(getch()) {
			case 'q': // quit
				running = false;
				break;

			case 's': // make a sample
				attron(A_BOLD);
				mvprintw(3, 3, " Sampling ... ");
				attroff(A_BOLD);
				refresh();

				open_stream();
				read_stream(buffer, sizeof(short) * sample_count);
				free_stream();

				for(int i = 0; i < sample_count; ++i)
					data[i] = (double) buffer[i];
				fft(data);

				break;

			case 'z':
				if(min_frequency - FREQUENCY_CHANGE_STEP >= 0)
					min_frequency -= FREQUENCY_CHANGE_STEP;
				break;
			case 'x':
				if(min_frequency + FREQUENCY_CHANGE_STEP < max_frequency)
					min_frequency += FREQUENCY_CHANGE_STEP;
				break;
			case 'c':
				if(max_frequency - FREQUENCY_CHANGE_STEP > min_frequency)
					max_frequency -= FREQUENCY_CHANGE_STEP;
				break;
			case 'v':
				if(max_frequency + FREQUENCY_CHANGE_STEP <= 20000)
					max_frequency += FREQUENCY_CHANGE_STEP;
				break;

			case '[':
				if(threshold - THRESHOLD_CHANGE_STEP >= 0)
					threshold -= THRESHOLD_CHANGE_STEP;
				break;
			case ']':
				threshold += THRESHOLD_CHANGE_STEP;
				break;

			case 'w':
				if(sample_count > MIN_SAMPLE_COUNT) {
					sample_count >>= 1;
					data.resize(sample_count);
				}
				break;
			case 'e':
				if(sample_count < MAX_SAMPLE_COUNT) {
					sample_count <<= 1;
					data.resize(sample_count);
				}
				break;
		}
	}

	endwin();
	return 0;
}
