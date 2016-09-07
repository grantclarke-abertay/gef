#include <animation/joint.h>

namespace gef
{
	bool Joint::Read(std::istream& stream)
	{
		stream.read((char*)&name_id, sizeof(StringId));
		stream.read((char*)&parent, sizeof(Int32));
		stream.read((char*)&inv_bind_pose, sizeof(Matrix44));

		return true;
	}

	bool Joint::Write(std::ostream& stream) const
	{
		stream.write((char*)&name_id, sizeof(StringId));
		stream.write((char*)&parent, sizeof(Int32));
		stream.write((char*)&inv_bind_pose, sizeof(Matrix44));

		return true;
	}
}

