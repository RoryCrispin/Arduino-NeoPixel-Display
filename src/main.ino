#include <Adafruit_NeoPixel.h>
#include <neoColour.h>

#define PIN             3
#define NUMPIXELS       150
#define array_length(x)(sizeof(x)/sizeof(*x)) //Returns the number of items in array

int strobePin  = 2;    // Strobe Pin on the MSGEQ7
int resetPin   = 1;    // Reset Pin on the MSGEQ7
int outPin     = A0;   // Output Pin on the MSGEQ7
int level[7];          // An array to hold the values from the 7 frequency bands
int msgPercentage[7];
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int modes[5] = {0};
int filters[5];


// Mode Global Vars
// (I hate writing non OO code as much as you hate reading it)
// I am so sorry

// --: mode_spin
int rotation_index = 0;


void setFullStrip(uint32_t colour, boolean show=false){
	for (int i = 0; i<NUMPIXELS;i++){	
		pixels.setPixelColor(i, colour);
	}
	if(show){
		pixels.show();
	}
}
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
	setFullStrip(pixels.Color(255,0,0), true);
	delay(10);
	setFullStrip(pixels.Color(0,0,0), true);
	delay(10);
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
	uint32_t secondaryColour = pixels.Color(0,0,255);

	modes[0] = 2;
	for (int i = 0; i< array_length(modes); i++){
		switch (modes[i]){
			case 0:
				break;		
			case 1:
				view_spectrum(primaryColour, secondaryColour);
				break;
			case 2:
				mode_spin(primaryColour, secondaryColour);
				break;
			case 3:
				setFullStrip(pixels.Color(100,100,255), true);
				break;
	}	}
	pixels.show();
	
//	neoColour::init(255,255,255);
}


void view_spectrum(uint32_t primaryColour, uint32_t secondaryColour){
	for (int i = 0; i<NUMPIXELS;i++){
		if (i<level[0]/8){
			pixels.setPixelColor(i, pixels.Color(255,0,100));	
		}
	}

}

void mode_spin(uint32_t primaryColour, uint32_t secondaryColour){
	int halfpoint = NUMPIXELS/2;
	boolean pixel_is_primary[NUMPIXELS]; 	// To save memory, represent primary colour
	// by true and secondaryColour as false in
	// intermediate arrays for the rotation function.

	for (int i = 0; i<NUMPIXELS;i++){
		// We know that our array is the same length as NUMPIXELS
		// So why calculate the length again with array_length();
		// when we can use NUMPIXELS 

		if (i<halfpoint){
			pixel_is_primary[i] = true;
		} else {
			pixel_is_primary[i] = false;
		}
	}

	for (int i = 0; i<rotation_index;i++){
		for (int i = 0; i<NUMPIXELS;i++){

		boolean firstItem = pixel_is_primary[0];
			pixel_is_primary[i] = pixel_is_primary[i+1];

		pixel_is_primary[NUMPIXELS] = firstItem;
		}
	}

	for (int i = 0; i<NUMPIXELS;i++){
		// We know that our array is the same length as NUMPIXELS
		// So why calculate the length again with array_length();
		// when we can use NUMPIXELS 

		if (pixel_is_primary[i]){
			pixels.setPixelColor(i, primaryColour);
		} else {
			pixels.setPixelColor(i, secondaryColour);
		}
	}
		rotation_index = rotation_index + 1;
		delay(10);
		if (rotation_index == NUMPIXELS){
			rotation_index = 0;
		}

}

uint32_t colour_with_intensity(uint32_t, int intensity_percentage){
	//return pixels.Color()
	return 0;
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



