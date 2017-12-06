#include "stream.h"
#include "fft.hpp"

#include <ncurses.h>

#include <vector>
#include <thread>
#include <atomic>
#include <algorithm>

const int MinSampleCount = 32;
const int MaxSampleCount = 1 << 16;

const int FrequencyChangeStep = 100;
const int ThresholdChangeStep = 100;

const int SampleRate = 44100;

short buffer[MaxSampleCount];
double amplitudes[MaxSampleCount];

int main() {
	initscr();
	raw();
	noecho();
	timeout(100);

	int min_frequency = 0;
	int max_frequency = 20000;
	int threshold = 1000;
	int sample_count = 1 << 15;

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
		for(int i = 0; i < sample_count / 2; ++i) {
			double freq = (double) SampleRate / sample_count * i;
			if(freq < min_frequency || freq > max_frequency)
				continue;
			double amp = amplitudes[i] * 2 / sample_count;
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

				fft_get_amplitudes(buffer, sample_count, amplitudes);
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
							fft_get_amplitudes(buffer, sample_count, amplitudes);
						}
					});
				}
				break;

			case 'z':
				if(min_frequency - FrequencyChangeStep >= 0)
					min_frequency -= FrequencyChangeStep;
				break;
			case 'x':
				if(min_frequency + FrequencyChangeStep < max_frequency)
					min_frequency += FrequencyChangeStep;
				break;
			case 'c':
				if(max_frequency - FrequencyChangeStep > min_frequency)
					max_frequency -= FrequencyChangeStep;
				break;
			case 'v':
				if(max_frequency + FrequencyChangeStep <= 20000)
					max_frequency += FrequencyChangeStep;
				break;

			case '[':
				if(threshold - ThresholdChangeStep >= 0)
					threshold -= ThresholdChangeStep;
				break;
			case ']':
				threshold += ThresholdChangeStep;
				break;

			case 'w':
				if(sample_count > MinSampleCount)
					sample_count >>= 1;
				break;
			case 'e':
				if(sample_count < MaxSampleCount)
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
