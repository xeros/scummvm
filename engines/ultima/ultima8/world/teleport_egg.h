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

#ifndef ULTIMA8_WORLD_TELEPORTEGG_H
#define ULTIMA8_WORLD_TELEPORTEGG_H

#include "ultima/ultima8/world/egg.h"

namespace Ultima {
namespace Ultima8 {

class TeleportEgg : public Egg {
	friend class ItemFactory;
public:
	TeleportEgg();
	virtual ~TeleportEgg();

	ENABLE_RUNTIME_CLASSTYPE()

	bool isTeleporter() const {
		return frame != 1;
	}
	int getTeleportId() const {
		return (quality & 0xFF);
	}

	bool loadData(IDataSource *ids, uint32 version);

	virtual uint16 hatch();
protected:
	virtual void saveData(ODataSource *ods);

};

} // End of namespace Ultima8
} // End of namespace Ultima

#endif
