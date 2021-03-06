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
#include "ultima/ultima8/gumps/u8_save_gump.h"

#include "ultima/ultima8/graphics/render_surface.h"
#include "ultima/ultima8/gumps/desktop_gump.h"
#include "ultima/ultima8/gumps/widgets/edit_widget.h"
#include "ultima/ultima8/gumps/widgets/text_widget.h"
#include "ultima/ultima8/ultima8.h"
#include "ultima/ultima8/games/game_data.h"
#include "ultima/ultima8/graphics/shape.h"
#include "ultima/ultima8/graphics/shape_frame.h"
#include "ultima/ultima8/filesys/file_system.h"
#include "ultima/ultima8/filesys/savegame.h"
#include "ultima/ultima8/gumps/paged_gump.h"
#include "ultima/ultima8/world/get_object.h"
#include "ultima/ultima8/world/actors/main_actor.h"

#include "ultima/ultima8/filesys/idata_source.h"
#include "ultima/ultima8/filesys/odata_source.h"

namespace Ultima {
namespace Ultima8 {

static const int entryfont = 4;

DEFINE_RUNTIME_CLASSTYPE_CODE(U8SaveGump, Gump)

U8SaveGump::U8SaveGump(bool save_, int page_)
	: Gump(0, 0, 5, 5), save(save_), page(page_) {

}

U8SaveGump::~U8SaveGump() {

}


// gumps: 36/0-11: number 1-12
//        46/0: "Entry"

void U8SaveGump::InitGump(Gump *newparent, bool take_focus) {
	Gump::InitGump(newparent, take_focus);

	dims.w = 220;
	dims.h = 170;

	FrameID entry_id(GameData::GUMPS, 46, 0);
	entry_id = _TL_SHP_(entry_id);

	Shape *entryShape;
	entryShape = GameData::get_instance()->getShape(entry_id);
	ShapeFrame *sf = entryShape->getFrame(entry_id.framenum);
	int entrywidth = sf->width;
	int entryheight = sf->height;

	if (save)
		editwidgets.resize(6); // constant!

	loadDescriptions();

	for (int i = 0; i < 6; ++i) {
		int index_ = page * 6 + i;


		int xbase = 3;
		int yi = i;
		if (i >= 3) {
			xbase += dims.w / 2 + 9;
			yi -= 3;
		}

		Gump *gump = new Gump(xbase, 3 + 40 * yi, 1, 1);
		gump->SetShape(entry_id, true);
		gump->InitGump(this, false);

		int x_ = xbase + 2 + entrywidth;

		if (index_ >= 9) { // index_ 9 is labelled "10"
			FrameID entrynum1_id(GameData::GUMPS, 36, (index_ + 1) / 10 - 1);
			entrynum1_id = _TL_SHP_(entrynum1_id);
			entryShape = GameData::get_instance()->getShape(entrynum1_id);
			sf = entryShape->getFrame(entrynum1_id.framenum);
			x_ += 1 + sf->width;

			gump = new Gump(xbase + 2 + entrywidth, 3 + 40 * yi, 1, 1);
			gump->SetShape(entrynum1_id, true);
			gump->InitGump(this, false);
		}

		FrameID entrynum_id(GameData::GUMPS, 36, index_ % 10);
		entrynum_id = _TL_SHP_(entrynum_id);

		gump = new Gump(x_, 3 + 40 * yi, 1, 1);
		gump->SetShape(entrynum_id, true);

		if (index_ % 10 == 9) {
			// HACK: There is no frame for '0', so we re-use part of the
			// frame for '10', cutting off the first 6 pixels.
			Pentagram::Rect rect;
			gump->GetDims(rect);
			rect.x += 6;
			gump->SetDims(rect);
		}
		gump->InitGump(this, false);

		if (index_ == 0) {
			// special case for 'The Beginning...' save
			Gump *widget = new TextWidget(xbase, 12 + entryheight,
			                              _TL_("The Beginning..."),
			                              true, entryfont, 95);
			widget->InitGump(this, false);

		} else {

			if (save) {
				EditWidget *ew = new EditWidget(xbase, entryheight + 4 + 40 * yi,
				                                descriptions[i],
				                                true, entryfont,
				                                95, 38 - entryheight, 0, true);
				ew->SetIndex(i + 1);
				ew->InitGump(this, false);
				editwidgets[i] = ew;
			} else {
				// load
				Gump *widget = new TextWidget(xbase, entryheight + 4 + 40 * yi,
				                              descriptions[i], true, entryfont,
				                              95);
				widget->InitGump(this, false);
			}
		}

	}

	// remove focus from children (just in case)
	if (focus_child) focus_child->OnFocus(false);
	focus_child = 0;
}

void U8SaveGump::Close(bool no_del) {
	Gump::Close(no_del);
}

void U8SaveGump::OnFocus(bool gain) {
	if (gain) {
		if (save)
			Mouse::get_instance()->setMouseCursor(Mouse::MOUSE_QUILL);
		else
			Mouse::get_instance()->setMouseCursor(Mouse::MOUSE_MAGGLASS);
	}
}

Gump *U8SaveGump::OnMouseDown(int button, int mx, int my) {
	// take all clicks
	return this;
}


void U8SaveGump::OnMouseClick(int button, int mx, int my) {
	if (button != Shared::BUTTON_LEFT) return;

	ParentToGump(mx, my);

	int x_;
	if (mx >= 3 && mx <= 100)
		x_ = 0;
	else if (mx >= dims.w / 2 + 10)
		x_ = 1;
	else
		return;

	int y_;
	if (my >= 3 && my <= 40)
		y_ = 0;
	else if (my >= 43 && my <= 80)
		y_ = 1;
	else if (my >= 83 && my <= 120)
		y_ = 2;
	else
		return;

	int i = 3 * x_ + y_;
	int index_ = 6 * page + i + 1;

	if (save && !focus_child && editwidgets[i]) {
		editwidgets[i]->MakeFocus();
		PagedGump *p = p_dynamic_cast<PagedGump *>(parent);
		if (p) p->enableButtons(false);
	}

	if (!save) {

		// If our parent has a notifiy process, we'll put our result in it and wont actually load the game
		GumpNotifyProcess *p = parent->GetNotifyProcess();
		if (p) {
			// Do nothing in this case
			if (index_ != 1 && descriptions[i].empty()) return;

			parent->SetResult(index_);
			parent->Close(); // close PagedGump (and us)
			return;
		}

		loadgame(index_); // 'this' will be deleted here!
	}
}

void U8SaveGump::ChildNotify(Gump *child, uint32 message) {
	if (child->IsOfType<EditWidget>() && message == EditWidget::EDIT_ENTER) {
		// save
		assert(save);

		EditWidget *widget = p_dynamic_cast<EditWidget *>(child);
		assert(widget);

		Std::string name = widget->getText();
		if (name.empty()) return;

		if (savegame(widget->GetIndex() + 6 * page, name))
			parent->Close(); // close PagedGump (and us)

		return;
	}

	if (child->IsOfType<EditWidget>() && message == EditWidget::EDIT_ESCAPE) {
		// cancel edit
		assert(save);

		// remove focus
		if (focus_child) focus_child->OnFocus(false);
		focus_child = 0;

		PagedGump *p = p_dynamic_cast<PagedGump *>(parent);
		if (p) p->enableButtons(true);

		EditWidget *widget = p_dynamic_cast<EditWidget *>(child);
		assert(widget);
		widget->setText(descriptions[widget->GetIndex() - 1]);

		return;
	}

}

bool U8SaveGump::OnKeyDown(int key, int mod) {
	if (Gump::OnKeyDown(key, mod)) return true;

	return false;
}

Std::string U8SaveGump::getFilename(int index) {
	return Std::string::format("@save/%d", index);
}

bool U8SaveGump::loadgame(int saveIndex) {
	if (saveIndex == 1) {
		Ultima8Engine::get_instance()->newGame(Std::string());
		return true;
	}

	pout << "Load " << saveIndex << Std::endl;

	Std::string filename = getFilename(saveIndex);
	Ultima8Engine::get_instance()->loadGame(filename);

	return true;
}

bool U8SaveGump::savegame(int saveIndex, const Std::string &name) {
	pout << "Save " << saveIndex << ": \"" << name << "\"" << Std::endl;

	if (name.empty()) return false;

	Std::string filename = getFilename(saveIndex);
	Ultima8Engine::get_instance()->saveGame(filename, name, true);
	return true;
}

void U8SaveGump::loadDescriptions() {
	descriptions.resize(6);

	for (int i = 0; i < 6; ++i) {
		int saveIndex = 6 * page + i + 1;

		Std::string filename = getFilename(saveIndex);
		IDataSource *ids = FileSystem::get_instance()->ReadFile(filename);
		if (!ids) continue;

		SavegameReader *sg = new SavegameReader(ids, true);
		SavegameReader::State state = sg->isValid();
		descriptions[i] = "";

		// FIXME: move version checks elsewhere!!
		switch (state) {
		case SavegameReader::SAVE_CORRUPT:
			descriptions[i] = "[corrupt] ";
			break;
		case SavegameReader::SAVE_OUT_OF_DATE:
			descriptions[i] = "[outdated] ";
			break;
		case SavegameReader::SAVE_TOO_RECENT:
			descriptions[i] = "[too modern] ";
			break;
		default:
			break;
		}

		descriptions[i] += sg->getDescription();
		delete sg;
	}
}

//static
Gump *U8SaveGump::showLoadSaveGump(Gump *parent, bool save) {
	if (save) {
		// can't save if game over
		// FIXME: this check should probably be in Game or GUIApp
		MainActor *av = getMainActor();
		if (!av || (av->getActorFlags() & Actor::ACT_DEAD)) return 0;
	}

	PagedGump *gump = new PagedGump(34, -38, 3, 35);
	gump->InitGump(parent);

	U8SaveGump *s;

	for (int page = 0; page < 16; ++page) {
		s = new U8SaveGump(save, page);
		s->InitGump(gump, false);
		gump->addPage(s);
	}


	gump->setRelativePosition(CENTER);

	return gump;
}

} // End of namespace Ultima8
} // End of namespace Ultima
