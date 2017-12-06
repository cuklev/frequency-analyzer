#include "stream.h"
#include "fft.hpp"

#include<ncurses.h>

#include<vector>
#include<complex>
#include<thread>
#include<atomic>
#include<algorithm>

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
	timeout(100);

	int min_frequency = 0;
	int max_frequency = 20000;
	int threshold = 1000;
	int sample_count = 1 << 15;

	std::vector<std::complex<double>> data(sample_count);

	std::thread sample_thread;

	bool running = true;
	std::atomic<bool> auto_sample(false);
	while(running) {
		clear();
		printw("Threshold: %d\n", threshold);
		printw("Frequency: %d - %d Hz\n", min_frequency, max_frequency);
		printw("Sample count: %d\n", sample_count);
		printw("=====================\n");

		std::vector<std::pair<double, double>> frequencies;
		for(int i = 0; i < (int) data.size() / 2; ++i) {
			double freq = (double) SAMPLE_RATE / data.size() * i;
			if(freq < min_frequency || freq > max_frequency)
				continue;
			double amp = abs(data[i]) * 2 / sample_count;
			if(amp < threshold)
				continue;
			frequencies.push_back({amp, freq});
		}
		std::sort(frequencies.begin(), frequencies.end(), [](auto x, auto y) { return x.first > y.first; });
		for(auto& f : frequencies) printw("%lfHz\n", f.second);

		if(auto_sample) {
			attron(A_BOLD);
			mvprintw(3, 3, " AUTO Sampling ");
			attroff(A_BOLD);
		}

		refresh();

		switch(getch()) {
			case 'q': // quit
				running = false;
				break;

			case 's': // make a sample
				if(auto_sample) break;

				attron(A_BOLD);
				mvprintw(3, 3, " Sampling ... ");
				attroff(A_BOLD);
				refresh();

				open_stream();
				read_stream(buffer, sizeof(short) * sample_count);
				free_stream();

				data = fft(buffer, sample_count);

				break;
			case 'S':
				if(auto_sample) {
					auto_sample = false;
					sample_thread.join();
					free_stream();
				} else {
					auto_sample = true;
					open_stream();

					sample_thread = std::thread([&] {
						while(auto_sample) {
							read_stream(buffer, sizeof(short) * sample_count);
							data = fft(buffer, sample_count);
						}
					});
				}
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
				if(sample_count > MIN_SAMPLE_COUNT)
					sample_count >>= 1;
				break;
			case 'e':
				if(sample_count < MAX_SAMPLE_COUNT)
					sample_count <<= 1;
				break;
		}
	}

	if(auto_sample) {
		auto_sample = false;
		sample_thread.join();
		free_stream();
	}

	endwin();
}
