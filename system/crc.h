#ifndef _GEF_CRC_H
#define _GEF_CRC_H

#include <gef.h>

namespace gef
{
	// http://www.codeproject.com/Articles/4251/Spoofing-the-Wily-Zip-CRC

	class CRC
	{
	public:
		static UInt32 GetCRC(const char* _pString);
		static UInt32 GetICRC(const char* _pString);
		CRC(UInt32 _r=~0);
	private:
		void Update(const char *pbuf, int len, bool toUpper = false); // update crc residual 
		inline UInt32 GetU32() { return ~r; } // object yields current CRC

		void Clk(int i=0);     // clock in data, bit at a time
		void ClkRev();        // clk residual backwards

		enum{gf=0xdb710641};  // This is the generator
		UInt32 r;                // residual, polynomial mod gf
	};
}
#endif // _CRC_H