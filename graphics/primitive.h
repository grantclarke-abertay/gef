#ifndef _GEF_PRIMITIVE_H
#define _GEF_PRIMITIVE_H

#include <gef.h>

namespace gef
{
	class Platform;
	class IndexBuffer;
	class Material;

	enum PrimitiveType
	{
		UNDEFINED = -1,
		TRIANGLE_LIST = 0,
		TRIANGLE_STRIP,
		LINE_LIST,
		NUM_PRIMITIVE_TYPES
	};

	class Primitive
	{
	public:


		Primitive(Platform& platform);
		virtual  ~Primitive();
		virtual bool InitIndexBuffer(Platform& platform, const void* indices, const UInt32 num_indices, const UInt32 index_byte_size,const bool read_only = true);


		inline const IndexBuffer* index_buffer() const { return index_buffer_; }
		inline IndexBuffer* index_buffer() { return index_buffer_; }

		inline const Material* material() const {return material_;}
		inline void set_material(const class Material* material) { material_ = material; }
		inline void set_type(PrimitiveType type) { type_ = type; }
		inline PrimitiveType type() const { return type_; }

	protected:

		const Material* material_;
		PrimitiveType type_;
		IndexBuffer* index_buffer_;
		Platform& platform_;

	};

}
#endif // _GEF_PRIMITIVE_H
