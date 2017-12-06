#include "fft.hpp"


namespace {

using ComplexNumber = std::complex<double>;

const uint32_t Mask0 = 0x55555555;
const uint32_t Mask1 = 0x33333333;
const uint32_t Mask2 = 0x0F0F0F0F;
const uint32_t Mask3 = 0x00FF00FF;
const uint32_t Mask4 = 0x0000FFFF;

inline void swap_mask(uint32_t& n, uint32_t mask, int bit_count) {
	n = ((n & mask) << bit_count) | ((n >> bit_count) & mask);
}

inline uint32_t reverse_bits(uint32_t n, int bit_count) {
	swap_mask(n, Mask0, 1);
	swap_mask(n, Mask1, 2);
	swap_mask(n, Mask2, 4);
	swap_mask(n, Mask3, 8);
	swap_mask(n, Mask4, 16);
	return n >> (32 - bit_count);
}

inline void swap_reverse(std::vector<ComplexNumber>& data, int bits)
{
	for(uint32_t i = 0; i < data.size(); ++i)
	{
		auto j = reverse_bits(i, bits);
		if(i < j) std::swap(data[i], data[j]);
	}
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
