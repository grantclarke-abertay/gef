#include <system/crc.h>
#include <string.h>
#include <cctype>

namespace gef
{
	UInt32 CRC::GetCRC(const char* string)
	{
		CRC crc;

		if(string)
		{
			int length = (int)strlen(string);
			crc.Update(string, length);
		}

		return crc.GetU32();
	}

	UInt32 CRC::GetICRC(const char* string)
	{
		CRC crc;

		if(string)
		{
			int length = (int)strlen(string);
			crc.Update(string, length, true);
		}

		return crc.GetU32();
	}


	CRC::CRC(UInt32 _r) : r(_r)
	{

	}

	// This reverses CRC::clk(0)
	// in Galois terms: R := i + R*X
	void CRC::ClkRev()
	{
		if (r & 0x80000000)
			r = (r << 1) ^ gf;
		else
			r = (r << 1);
	}

	// this is the basic "gut's" of crc calcuation.
	// and updates the crc's 32 bit residual for a
	// single bit, in terms of galois theory
	// this is: R := R*X^(-1)
	void CRC::Clk(int i)
	{
		r ^= i;
		// rotation combined with possible xor of "gf"
		if (1 & r)
			r = ((r^gf) >> 1) | 0x80000000;
		else
			r >>= 1;
	}

	// This does a byte at a time by calculating the
	// crc bit by bit, lsb first. Slow, but clear.
	void CRC::Update(const char *buffer, int len, bool toUpper)
	{
		while (len--)
		{
			int bitcnt, byte = *buffer++;
			if(toUpper)
				byte = std::toupper(byte);
			for (bitcnt=0; bitcnt < 8; bitcnt++)
			{
				Clk(byte & 1);
				byte >>= 1;
			}
		}
	}
}
