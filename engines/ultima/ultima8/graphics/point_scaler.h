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

#ifndef ULTIMA8_GRAPHICS_SCALERS_POINTSCALER_H
#define ULTIMA8_GRAPHICS_SCALERS_POINTSCALER_H

#include "ultima/ultima8/graphics/scaler.h"

namespace Ultima {
namespace Ultima8 {
namespace Pentagram {

class PointScaler : public Scaler {
public:
	PointScaler();

	virtual uint32    ScaleBits() const;          //< bits for supported integer scaling
	virtual bool      ScaleArbitrary() const;     //< supports arbitrary scaling of any degree
};

} // End of namespace Pentagram
} // End of namespace Ultima8
} // End of namespace Ultima

#endif
