#include <Adafruit_NeoPixel.h>

#define PIN             3
#define NUMPIXELS       150

int strobePin  = 2;    // Strobe Pin on the MSGEQ7
int resetPin   = 1;    // Reset Pin on the MSGEQ7
int outPin     = A0;   // Output Pin on the MSGEQ7
int level[7];          // An array to hold the values from the 7 frequency bands
int msgPercentage[7];
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int modes[];
int filters[];


void setup() {
	pixels.begin();
	// Define our pin modes
	pinMode      (strobePin, OUTPUT);
	pinMode      (resetPin,  OUTPUT);
	pinMode      (outPin,    INPUT);

	// Create an initial state for our pins
	digitalWrite (resetPin,  LOW);
	digitalWrite (strobePin, LOW);
	delay        (1);
	// Reset the MSGEQ7 as per the datasheet timing diagram
	digitalWrite (resetPin,  HIGH);
	delay        (1);
	digitalWrite (resetPin,  LOW);
	digitalWrite (strobePin, HIGH);
	delay        (1);
	setFullStrip(pixels.Color(255,0,0));
	delay(10);
	setFullStrip(pixels.Color(0,0,0));
	delay(10);
}

void setFullStrip(uint32_t colour){
	for (int i = 0; i<NUMPIXELS;i++){	
		pixels.setPixelColor(i, colour);
	}
	pixels.show();
}

void clear_pixels(){
	setFullStrip(pixels.Color(0,0,0));
}

void loop() {
	readMSG();
	calc_msg_percentage();
	clear_pixels();
	// *-----------------*
	// Mode index
	// 0	view_spectrum
	// 1	bar
	//
	// -------------------	
	// Filter Index
	// 0	spin
	//
	// *-----------------*
	uint32_t primaryColour = pixels.Color(255,0,0);
	uint32_t secondaryColour = pixels.Color(0,0,0);

	for (int i = 0; i< sizeof(modes); i++){
		switch (modes[i]):
			case 0:
				view_spectrum(primaryColour, secondaryColour);
				break;		
	}	
	pixels.show();
}


void view_spectrum(uint32_t primaryColour, uint32_t secondaryColour){
	for (int i = 0; i<NUMPIXELS;i++){
		if (i<level[0]/8){
			pixels.setPixelColor(i, pixels.Color(255,0,100));	
		}
	}

}


uint32_t colour_with_intensity(uint32_t, int intensity_percentage){
	return pixels.Color()
}


void readMSG(){
	for (int i = 0; i < 7; i++) {
		digitalWrite       (strobePin, LOW);
		delayMicroseconds  (100);                    // Delay necessary due to timing diagram
		level[i] =         analogRead (outPin);
		digitalWrite       (strobePin, HIGH);
		delayMicroseconds  (100);                    // Delay necessary due to timing diagram  
	}    
}

void calc_msg_percentage(){
	for (int i = 0; i<7; i++){
		msgPercentage[i] = map(level[i],0,1000,0,100);
	}
}
