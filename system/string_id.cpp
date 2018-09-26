#include <system/string_id.h>
#include <system/crc.h>

namespace gef
{
	StringId StringIdTable::Add(const std::string& text)
	{
		// string id is generated from the string converted to uppercase
		// the original string is stored
		StringId string_id = GetStringId(text);

		std::map<StringId, std::string>::iterator iter = table_.find(string_id);
		if(iter == table_.end())
			table_[string_id] = text;
		return string_id;
	}

	bool StringIdTable::Find(const UInt32 string_id, std::string& result)
	{
		std::map<StringId, std::string>::iterator iter = table_.find(string_id);
		if(iter != table_.end())
		{
			result = iter->second;
			return true;
		}
		else
			return false;
	}

	bool StringIdTable::Remove(StringId string_id)
	{
		bool found = false;

		std::map<StringId, std::string>::iterator iter = table_.find(string_id);
		if (iter != table_.end())
		{
			table_.erase(iter);
			found = true;
		}

		return found;
	}

	StringId GetStringId(const std::string& text)
	{
		return CRC::GetICRC(text.c_str());
	}
}