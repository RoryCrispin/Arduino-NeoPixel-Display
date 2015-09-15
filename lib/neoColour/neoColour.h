
#ifndef neoColour_h
#define neoColour_h
#include "Arduino.h"

class neoColour{
	public:
		void initc(int red, int green, int blue);
//		int getRed();
//		int getGreen();
//		int getBlue();
	private:
		int _red;
		int _green;
		int _blue;
};
#endif


