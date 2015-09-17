#include <Adafruit_NeoPixel.h>
#include <neoColour.h>

#define PIN             3
#define NUMPIXELS       150
#define array_length(x)(sizeof(x)/sizeof(*x)) //Returns the number of items in array

int strobePin  = 2;    // Strobe Pin on the MSGEQ7
int resetPin   = 1;    // Reset Pin on the MSGEQ7
int outPin     = A0;   // Output Pin on the MSGEQ7
int level[3][7]; 	       // An array to hold the values from the 7 frequency bands. 
// Where levels[n][0] is the latest reading and [n][2] is the oldest stored reading.


int xoyo[5][5];
int msgPercentage[7];
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int modes[5] = {0};
int filters[5];


// Mode Global Vars
// (I hate writing non OO code as much as you hate reading it)
// I am not a C++ programmer, this project is *not* intended to be
// reused on other hardware. This is not a library.
//
// I originally started a neat, extendable OO python library which
// is also in GitHub, although the RPi doesn't perform fast enough to
// control the LED strip in response to moment to moment music voltages. 
//

// --| AudioAnalytics
int level_delta[7]; // Store the delta of each block over 
boolean newBeat;  // True if a new beat is detected
int audio_volume;

// --| mode_spin
boolean rotate_clockwise = false;
int rotation_index = 20;
boolean spin_bool_array[NUMPIXELS] = {0}; 
// To save memory, represent primary colour
// by true and secondaryColour as false in
// intermediate arrays for the rotation function.
int halfpoint = NUMPIXELS/2;
//	boolean  pixel_is_primary[NUMPIXELS]; 

	boolean starting_spin_array[NUMPIXELS] = {0};


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
	delay(3);
	setFullStrip(pixels.Color(0,0,0), true);


	//Mode Specific setup files

	setup_mode_spin();

}

void setup_mode_spin(){
	// --| mode_spin
	for (int i = 0; i<NUMPIXELS;i++){
		// We know that our array is the same length as NUMPIXELS
		// So why calculate the length again with array_length();
		// when we can use NUMPIXELS 

		if (i<halfpoint){
		starting_spin_array[i] = true;
		} else {
			starting_spin_array[i] = false;
		}
	}


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
	uint32_t primaryColour = pixels.Color(50,0,0);
	uint32_t secondaryColour = pixels.Color(0,0,55);

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

void rotate_bool_array(boolean clockwise){
	boolean firstItem = spin_bool_array[0];
	bool lastItem = spin_bool_array[NUMPIXELS];

	if (clockwise){
		for (int i = 0; i<NUMPIXELS;i++){
			spin_bool_array[i] = spin_bool_array[i+1];
		}

		spin_bool_array[NUMPIXELS] = firstItem;

	} else {
		for (int i = NUMPIXELS; i!=0;i--){
			spin_bool_array[i] = spin_bool_array[i-1];
		}
		spin_bool_array[0] = lastItem;
	}
}

uint32_t colour_with_intensity(uint32_t, int intensity_percentage){
	//return pixels.Color()
	return 0;
}

void view_spectrum(uint32_t primaryColour, uint32_t secondaryColour){
	for (int i = 0; i<NUMPIXELS;i++){
		if (i<level[0][0]/8){
			pixels.setPixelColor(i, pixels.Color(255,0,100));	
		}
	}

}

void mode_spin(uint32_t primaryColour, uint32_t secondaryColour){
	memcpy(spin_bool_array, starting_spin_array, sizeof(spin_bool_array));
	//spin_bool_array = starting_spin_array;
	for (int i = 0; i<=rotation_index;i++){
		rotate_bool_array(rotate_clockwise);
	}
	
	for (int i = 0; i<NUMPIXELS;i++){
		// We know that our array is the same length as NUMPIXELS
		// So why calculate the length again with array_length();
		// when we can use NUMPIXELS 

		if (spin_bool_array[i]){
			pixels.setPixelColor(i, primaryColour);
		} else {
			pixels.setPixelColor(i, secondaryColour);
		}
	}
	rotation_index = rotation_index + 1;
	if (rotation_index == NUMPIXELS){
		rotation_index = 0;
	}
}

void AudioAnalytics(){
	calc_audio_volume();
	newBeat = beatDetect();
	

}

void calc_audio_volume(){
	audio_volume = 0;
	for (int i = 0; i <7; i++){
		audio_volume = audio_volume + level[0][i];		
	}
	audio_volume = audio_volume /7;
}

bool beatDetect(){
	if (level[0][3] > 40 ){
		return true;
	} else {
		return false;
	}

}




void readMSG(){

	for (int i = 2; i-->1;){ //Shift the arrays positions backwards in history 
		for (int l_pos = 0; l_pos < 7; l_pos++) {
			level[i][l_pos] = level[i-1][l_pos];
		}
	} 
	for (int i = 0; i < 7; i++) { // Read new values from ADC and place them in the 0th position 
		digitalWrite       (strobePin, LOW);
		delayMicroseconds  (100);                    // Delay necessary due to timing diagram
		level[0][0] =         analogRead (outPin);
		digitalWrite       (strobePin, HIGH);
		delayMicroseconds  (100);                    // Delay necessary due to timing diagram  
	}    
}

void calc_msg_percentage(){
	for (int i = 0; i<7; i++){
		msgPercentage[i] = map(level[0][i],0,1000,0,100);
	}
}



