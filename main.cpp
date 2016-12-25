#include "stream.h"
#include "fft.hpp"

#include<ncurses.h>

#include<vector>
#include<complex>

const int SAMPLE_COUNT = 1 << 15;

short buffer[SAMPLE_COUNT];

const int FREQUENCY_CHANGE_STEP = 100;
const int THRESHOLD_CHANGE_STEP = 100;

int main() {
	initscr();
	raw();
	noecho();

	std::vector<std::complex<double>> data(SAMPLE_COUNT);

	int min_frequency = 0;
	int max_frequency = 20000;
	int threshold = 1000;

	bool running = true;
	while(running) {
		clear();
		printw("Threshold: %d\n", threshold);
		printw("Frequency: %d - %d Hz\n", min_frequency, max_frequency);
		printw("====================\n");
		refresh();

		for(int i = 0; i < SAMPLE_COUNT / 2; ++i) {
			double freq = 44100.0F / SAMPLE_COUNT * i;
			if(freq < min_frequency || freq > max_frequency)
				continue;
			double amp = abs(data[i]) * 2 / SAMPLE_COUNT;
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
				mvprintw(2, 3, " Sampling ... ");
				attroff(A_BOLD);
				refresh();

				open_stream();
				read_stream(buffer, sizeof(buffer));
				free_stream();

				for(int i = 0; i < SAMPLE_COUNT; ++i)
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
		}
	}

	endwin();
	return 0;
}
