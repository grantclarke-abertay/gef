#ifndef _STRING_ID_H
#define _STRING_ID_H

#include <map>
#include <string>

#include <gef.h>

namespace gef
{
	typedef UInt32 StringId;

	class StringIdTable
	{
	public:
		StringId Add(const std::string& text);
		bool Find(const UInt32 string_id, std::string& result);
		bool Remove(StringId string_id);

		const std::map<StringId, std::string>& table() const { return table_; }
	private:
		std::map<StringId, std::string> table_;
	};

	extern StringId GetStringId(const std::string& text);
}
#endif // _STRING_ID_TABLE_H