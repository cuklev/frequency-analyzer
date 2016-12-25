#include "stream.h"
#include "fft.hpp"

#include<iostream>

const int SAMPLE_COUNT = 1 << 15;

short buffer[SAMPLE_COUNT];

int main() {
	open_stream();
	read_stream(buffer, sizeof(buffer));
	free_stream();

	std::vector<std::complex<double>> data(SAMPLE_COUNT);
	for(int i = 0; i < SAMPLE_COUNT; ++i)
		data[i] = (double) buffer[i];

	fft(data);
	for(int i = 0; i < SAMPLE_COUNT / 2; ++i)
		std::cout << abs(data[i]) / SAMPLE_COUNT << '\n';
}
