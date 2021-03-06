/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef ULTIMA8_FILESYS_U8SAVEFILE_H
#define ULTIMA8_FILESYS_U8SAVEFILE_H

#include "ultima/ultima8/filesys/named_archive_file.h"
#include "ultima/shared/std/containers.h"
#include "ultima/ultima8/misc/p_dynamic_cast.h"

namespace Ultima {
namespace Ultima8 {

class IDataSource;

class U8SaveFile : public NamedArchiveFile {
public:
	ENABLE_RUNTIME_CLASSTYPE()

	//! create U8SaveFile from datasource; U8SaveFile takes ownership of ds
	//! and deletes it when destructed
	explicit U8SaveFile(IDataSource *ds);
	virtual ~U8SaveFile();

	virtual bool exists(const Std::string &name);

	virtual uint8 *getObject(const Std::string &name, uint32 *size = 0);

	virtual uint32 getSize(const Std::string &name);

	virtual uint32 getCount() {
		return count;
	}

	static bool isU8SaveFile(IDataSource *ds);

protected:
	IDataSource *ds;
	uint32 count;

	Std::map<Common::String, uint32> indices;
	Std::vector<uint32> offsets;
	Std::vector<uint32> sizes;

private:
	bool readMetadata();
	bool findIndex(const Std::string &name, uint32 &index);
};

} // End of namespace Ultima8
} // End of namespace Ultima

#endif
