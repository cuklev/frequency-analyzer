#include "fft.hpp"

using ComplexNumber = std::complex<double>;

#define MASK0 0x55555555
#define MASK1 0x33333333
#define MASK2 0x0F0F0F0F
#define MASK3 0x00FF00FF
#define MASK4 0x0000FFFF
#define SWAP_MASK(mask, bits) n = ((n & MASK##mask) << bits) | ((n >> bits) & MASK##mask)

inline uint32_t reverse_bits(uint32_t n, int bits)
{
	SWAP_MASK(0, 1);
	SWAP_MASK(1, 2);
	SWAP_MASK(2, 4);
	SWAP_MASK(3, 8);
	SWAP_MASK(4, 16);
	return n >> (32 - bits);
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
