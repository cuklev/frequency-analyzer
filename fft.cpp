#include "fft.hpp"

using ComplexNumber = std::complex<double>;

inline uint32_t reverse_bits(uint32_t n, int bits)
{
	uint32_t rn = 0;
	for(int i = 0; i < bits; ++i)
		if((n >> i) & 1)
			rn |= 1 << (bits - 1 - i);
	return rn;
}

inline void swap_reverse(std::vector<ComplexNumber>& data, int bits)
{
	for(uint32_t i = 0; i < data.size(); ++i)
	{
		auto j = reverse_bits(i, bits);
		if(i < j) std::swap(data[i], data[j]);
	}
}

void fft(std::vector<ComplexNumber>& data)
{
	int bits = __builtin_ctz(data.size());
	swap_reverse(data, bits);

	for(int k = 0; k < bits; ++k)
	{
		double angle = M_PI / (1 << k);
		ComplexNumber primitive_root = {cos(angle), sin(angle)};

		for(unsigned i = 0; i < data.size(); i += (2u << k))
		{
			ComplexNumber w = {1, 0};
			for(unsigned j = i; j < i + (1u << k); ++j)
			{
				data[j + (1u << k)] *= w;
				auto diff = data[j] - data[j + (1u << k)];
				data[j] += data[j + (1u << k)];
				data[j + (1u << k)] = diff;

				w *= primitive_root;
			}
		}
	}
}
