// ezMenuPlus.h

#ifndef _ezMenuPlus_h
#define _ezMenuPlus_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <M5ez.h>
class ezMenuPlus : public ezMenu
{
protected:

public:
	ezMenuPlus(String hdr) : ezMenu{hdr} {}
	void setItem(int index) {
		_selected = index - 1;
		_fixOffset();
	}
	// return how many items
	int getItemCount() {
		return _items.size();
	}
	// get the item name
	String getItemName(int ix) {
		if (!_selected != -1) {
			return _items[ix - 1].nameAndCaption;
		}
		return "";
	}
	//void init();
};

#endif

