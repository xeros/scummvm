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

#include "ultima/ultima8/misc/pent_include.h"
#include "ultima/ultima8/gumps/inverter_gump.h"

#include "ultima/ultima8/graphics/render_surface.h"
#include "ultima/ultima8/graphics/texture.h"
#include "ultima/ultima8/ultima8.h"

namespace Ultima {
namespace Ultima8 {

DEFINE_RUNTIME_CLASSTYPE_CODE(InverterGump, DesktopGump)

InverterGump::InverterGump(int32 _x, int32 _y, int32 _width, int32 _height)
	: DesktopGump(_x, _y, _width, _height) {
	buffer = 0;
}

InverterGump::~InverterGump() {
	delete buffer;
}

static inline int getLine(int index, int n) {
	index = index % (2 * n);

	if (index >= n)
		return 2 * n - 1 - 2 * (index - n);
	else
		return 2 * index;
}

static inline int getIndex(int line, int n) {
	if (line % 2 == 0)
		return line / 2;
	else
		return 2 * n - 1 - (line / 2);
}

void InverterGump::Paint(RenderSurface *surf, int32 lerp_factor, bool scaled) {
	// Skip the clipping rect/origin setting, since they will already be set
	// correctly by our parent.
	// (Or maybe I'm just to lazy to figure out the correct coordinates
	//  to use to compensate for the flipping... -wjp :-) )

	// Don't paint if hidden
	if (IsHidden()) return;

	// Paint This
	PaintThis(surf, lerp_factor, scaled);

	// Paint children
	PaintChildren(surf, lerp_factor, scaled);
}


void InverterGump::PaintChildren(RenderSurface *surf, int32 lerp_factor, bool scaled) {
	unsigned int state = Ultima8Engine::get_instance()->getInversion();

	if (state == 0) {
		DesktopGump::PaintChildren(surf, lerp_factor, scaled);
		return;
	} else if (state == 0x8000) {
		bool old_flipped = surf->IsFlipped();
		surf->SetFlipped(!old_flipped);

		DesktopGump::PaintChildren(surf, lerp_factor, scaled);

		surf->SetFlipped(old_flipped);
		return;
	}

	int width = dims.w, height = dims.h;


	// need a backbuffer
	if (!buffer) {
		buffer = RenderSurface::CreateSecondaryRenderSurface(width, height);
	}

	DesktopGump::PaintChildren(buffer, lerp_factor, scaled);

	Texture *tex = buffer->GetSurfaceAsTexture();

	// now invert-blit buffer to screen
	int t = (state * height) / 0x10000;

	for (int i = 0; i < height; ++i) {
		int src = getLine(getIndex(i, height / 2) + t, height / 2);
//		pout << src << " -> " << i << Std::endl;
		surf->Blit(tex, 0, src, width, 1, 0, i);
	}
}

// Convert a parent relative point to a gump point
void InverterGump::ParentToGump(int &px, int &py, PointRoundDir) {
	px -= x;
	px += dims.x;
	py -= y;
	if (Ultima8Engine::get_instance()->isInverted()) py = dims.h - py - 1;
	py += dims.y;
}

// Convert a gump point to parent relative point
void InverterGump::GumpToParent(int &gx, int &gy, PointRoundDir) {
	gx -= dims.x;
	gx += x;
	gy -= dims.y;
	if (Ultima8Engine::get_instance()->isInverted()) gy = dims.h - gy - 1;
	gy += y;
}

void InverterGump::RenderSurfaceChanged() {
	DesktopGump::RenderSurfaceChanged();
	FORGET_OBJECT(buffer);
}

} // End of namespace Ultima8
} // End of namespace Ultima
