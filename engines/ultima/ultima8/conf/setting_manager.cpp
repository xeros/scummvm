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

#include "ultima/ultima8/misc/pent_include.h"
#include "ultima/ultima8/conf/setting_manager.h"
#include "ultima/ultima8/conf/config_file_manager.h"
#include "common/config-manager.h"

namespace Ultima {
namespace Ultima8 {

using Pentagram::istring;

SettingManager *SettingManager::settingmanager = 0;

SettingManager::SettingManager() {
	con->Print(MM_INFO, "Creating SettingManager...\n");

	settingmanager = this;

	domains.resize(DOM_GAME + 1);

	conffileman = ConfigFileManager::get_instance();

	conffileman->readConfigString("", "defaultsettings", false);

	// Setup ScummVM configuration settings
	setupScummVMSettings();
}

void SettingManager::setupScummVMSettings() {
	conffileman->readConfigString("", "ScummVM", false);

	int saveSlot = ConfMan.hasKey("save_slot") ? ConfMan.getInt("save_slot") : -1;
	if (saveSlot != -1)
		set("lastSave", Std::string::format("@save/%d", saveSlot), DOM_SCUMMVM);
}

SettingManager::~SettingManager() {
	con->Print(MM_INFO, "Destroying SettingManager...\n");

	settingmanager = 0;
}

bool SettingManager::readConfigFile(Std::string fname, bool readonly) {
	return conffileman->readConfigFile(fname, "settings", readonly);
}

void SettingManager::write() {
	conffileman->write("settings");
}

bool SettingManager::exists(istring key, Domain dom) {
	Domain temp;

	return findKeyDomain(key, dom, temp);
}

bool SettingManager::get(istring key, Std::string &ret, Domain dom) {
	Domain keydom;
	bool found = findKeyDomain(key, dom, keydom);
	if (!found) return false;

	conffileman->get(getConfigKey(key, keydom), ret);

	return true;
}

bool SettingManager::get(istring key, int &ret, Domain dom) {
	Domain keydom;
	bool found = findKeyDomain(key, dom, keydom);
	if (!found) return false;

	conffileman->get(getConfigKey(key, keydom), ret);

	return true;
}

bool SettingManager::get(istring key, bool &ret, Domain dom) {
	Domain keydom;
	bool found = findKeyDomain(key, dom, keydom);
	if (!found) return false;

	conffileman->get(getConfigKey(key, keydom), ret);

	return true;
}


void SettingManager::set(istring key, Std::string value, Domain dom) {
	conffileman->set(getConfigKey(key, dom), value);

	callCallbacks(key);
}

void SettingManager::set(istring key, const char *value, Domain dom) {
	conffileman->set(getConfigKey(key, dom), value);

	callCallbacks(key);
}

void SettingManager::set(istring key, int value, Domain dom) {
	conffileman->set(getConfigKey(key, dom), value);

	callCallbacks(key);
}

void SettingManager::set(istring key, bool value, Domain dom) {
	conffileman->set(getConfigKey(key, dom), value);

	callCallbacks(key);
}

void SettingManager::unset(istring key, Domain dom) {
	conffileman->unset(getConfigKey(key, dom));

	callCallbacks(key);
}




void SettingManager::setDefault(istring key, Std::string value) {
	set(key, value, DOM_DEFAULTS);
}

void SettingManager::setDefault(istring key, const char *value) {
	set(key, value, DOM_DEFAULTS);
}

void SettingManager::setDefault(istring key, int value) {
	set(key, value, DOM_DEFAULTS);
}

void SettingManager::setDefault(istring key, bool value) {
	set(key, value, DOM_DEFAULTS);
}



void SettingManager::setCurrentDomain(Domain dom) {
	currentDomain = dom;
}

void SettingManager::setDomainName(Domain dom, istring section) {
	unsigned int d = static_cast<unsigned int>(dom);

	if (domains.size() <= d) domains.resize(d + 1);
	domains[d] = section;
}

void SettingManager::registerCallback(istring key, ConfigCallback callback) {
	callbacks[key].push_back(callback);
}

void SettingManager::unregisterCallback(istring key, ConfigCallback callback) {
	Callbacks::iterator i = callbacks.find(key);
	if (i == callbacks.end())
		return;

	Std::vector<ConfigCallback> &cb = (*i)._value;
	Std::vector<ConfigCallback>::iterator iter;
	for (iter = cb.begin(); iter != cb.end(); ++iter) {
		if (*iter == callback) {
			cb.erase(iter);
			return;
		}
	}
}

Std::vector<istring> SettingManager::listGames() {
	Std::vector<istring> games;
	games.push_back("pentagram");
	games.push_back("ultima8");

	return games;
}

Std::vector<istring> SettingManager::listDataKeys(istring section) {
	istring csection = "settings/" + domains[DOM_GAME] + ":" + section;

	return conffileman->listKeys(csection, false);
}

KeyMap SettingManager::listDataValues(istring section) {
	istring csection = "settings/" + domains[DOM_GAME] + ":" + section;

	return conffileman->listKeyValues(csection, false);
}

bool SettingManager::findKeyDomain(istring key, Domain dom, Domain &keydom) {
	// if domain is DOM_CURRENT we search through all domains below the
	//    current domain.
	// otherwise, we search only the domain passed

	if (dom == DOM_CURRENT) {
		int d = static_cast<int>(currentDomain);
		for (; d >= 0; --d) {
			if (conffileman->exists(getConfigKey(key, static_cast<Domain>(d)))) {
				keydom = static_cast<Domain>(d);
				return true;
			}
		}
		return false;
	} else {
		keydom = dom;
		return conffileman->exists(getConfigKey(key, dom));
	}
}

istring SettingManager::getConfigKey(istring key, Domain dom) {
	istring ckey;

	if (dom == DOM_CURRENT) dom = currentDomain;

	if (dom == DOM_DEFAULTS) {
		ckey = "defaultsettings/";
	} else if (dom == DOM_SCUMMVM) {
		ckey = "ScummVM/";
	} else {
		ckey = "settings/" + domains[dom];
	}

	istring::size_type pos = key.find('/');

	if (pos != istring::npos) {
		ckey += ":" + key;
	} else {
		ckey += "/" + key;
	}

	return ckey;
}

void SettingManager::callCallbacks(istring key) {
	Callbacks::iterator i;
	i = callbacks.find(key);

	if (i == callbacks.end()) return;

	Std::vector<ConfigCallback> &cb = (*i)._value;
	Std::vector<ConfigCallback>::iterator iter;
	for (iter = cb.begin(); iter != cb.end(); ++iter) {
		(*iter)(key);
	}
}

} // End of namespace Ultima8
} // End of namespace Ultima
