/*
 * file_std.h
 *
 *  Created on: 27 Jan 2015
 *      Author: grant
 */

#ifndef SYSTEM_STD_FILE_STD_H_
#define SYSTEM_STD_FILE_STD_H_

#include <system/file.h>
#include <cstdio>

namespace gef
{


	class FileStd : public File
	{
	public:

		FileStd();
		~FileStd();

		bool Open(const char* const filename);
		bool Seek(const SeekFrom seek_from, Int32 offset/*, Int32* position = NULL*/);
		bool Read(void *buffer, const Int32 size, Int32& bytes_read);
//		bool Read(void *buffer, const Int32 size, const Int32 offset, Int32& bytes_read) =  0;
		bool Close();
		bool GetSize(Int32 &size);

        bool Exists(const char *const filename) override;

    private:
		FILE* file_handle_;
	};
}


#endif /* SYSTEM_STD_FILE_STD_H_ */
