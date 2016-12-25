#include "fft.hpp"

using number = std::complex<double>;

uint32_t reverse_bits(uint32_t n, int bits)
{
	uint32_t rn = 0;
	for(int i = 0; i < bits; ++i)
		if((n >> i) & 1)
			rn |= 1 << (bits - 1 - i);
	return rn;
}

void swap_reverse(std::vector<number>& data)
{
	int bits = __builtin_ctz(data.size());
	for(unsigned i = 0; i < data.size(); ++i)
	{
		auto j = reverse_bits(i, bits);
		if(i < j) std::swap(data[i], data[j]);
	}
}

void fft(std::vector<number>& data)
{
	swap_reverse(data);

	for(unsigned k = 0; (2u << k) <= data.size(); ++k)
	{
		double angle = M_PI / (1 << k);
		number primitive_root = {cos(angle), sin(angle)};

		for(unsigned i = 0; i < data.size(); i += (2u << k))
		{
			number w = {1, 0};
			for(unsigned j = 0; j < (1u << k); ++j)
			{
				data[i + (1u << k) + j] *= w;
				auto diff = data[i + j] - data[i + (1u << k) + j];
				data[i + j] += data[i + (1u << k) + j];
				data[i + (1u << k) + j] = diff;

				w *= primitive_root;
			}
		}
	}
}
