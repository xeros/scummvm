/*
Copyright (C) 2004 The Pentagram team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef ULTIMA8_CONF_INIFILE_H
#define ULTIMA8_CONF_INIFILE_H

#include "ultima/shared/std/containers.h"

namespace Ultima {
namespace Ultima8 {

typedef Std::map<Pentagram::istring, Std::string, Common::IgnoreCase_Hash> KeyMap;

class INIFile {
public:
	INIFile();
	INIFile(Std::string fname, Pentagram::istring root);
	~INIFile();

	bool readConfigFile(Std::string fname);

	//! read configuration from a string s. Lines must be separated by \n
	bool readConfigString(Std::string s);

	void clear(Pentagram::istring root);

	Std::string dump();
	void write();

	void setReadonly() {
		readonly = true;
	}
	bool isReadonly() const {
		return readonly;
	}

	bool hasSection(Pentagram::istring section);
	bool hasKey(Pentagram::istring key);
	bool checkRoot(Pentagram::istring key);

	// get value
	bool value(Pentagram::istring key, Std::string &ret);
	bool value(Pentagram::istring key, int &ret);
	bool value(Pentagram::istring key, bool &ret);

	// set value
	void set(Pentagram::istring key, Std::string value);
	void set(Pentagram::istring key, const char *value);
	void set(Pentagram::istring key, int value);
	void set(Pentagram::istring key, bool value);

	// remove key
	void unset(Pentagram::istring key);

	void listKeys(Std::set<Pentagram::istring> &keys,
	              Pentagram::istring section,
	              bool longformat = false);

	void listSections(Std::set<Pentagram::istring> &sections,
	                  bool longformat = false);

	void listKeyValues(KeyMap &keyvalues,
	                   Pentagram::istring section,
	                   bool longformat = false);

private:
	Std::string filename;
	Pentagram::istring root;
	bool is_file;
	bool readonly;

	struct KeyValue {
		Pentagram::istring key;
		Std::string value;
		Std::string comment;
	};

	struct Section {
		Pentagram::istring name;
		Std::list<KeyValue> keys;
		Std::string comment;

		bool hasKey(Pentagram::istring key);
		KeyValue *getKey(Pentagram::istring key);
		void setKey(Pentagram::istring key, Std::string value);
		void unsetKey(Pentagram::istring key);

		Std::string dump();
	};

	Std::list<Section> sections;


	bool stripRoot(Pentagram::istring &key);
	Section *getSection(Pentagram::istring section);
	bool splitKey(Pentagram::istring key, Pentagram::istring &section,
	              Pentagram::istring &sectionkey);

};

} // End of namespace Ultima8
} // End of namespace Ultima

#endif
