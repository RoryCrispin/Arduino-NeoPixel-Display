#include <Adafruit_NeoPixel.h>

#define PIN             3
#define NUMPIXELS       150

int strobePin  = 2;    // Strobe Pin on the MSGEQ7
int resetPin   = 1;    // Reset Pin on the MSGEQ7
int outPin     = A0;   // Output Pin on the MSGEQ7
int level[7];          // An array to hold the values from the 7 frequency bands
int msgPercentage[7];
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setFullStrip(uint32_t colour){
	for (int i = 0; i<NUMPIXELS;i++){	
		pixels.setPixelColor(i, colour);
	}
	pixels.show();
}
 
void setup() {
	pixels.begin();
	Serial.begin(9600);
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
	delay(100);
	setFullStrip(pixels.Color(0,0,0));
}
 




 
void loop() {
	readMSG();
	calc_msg_percentage();
	for (int i = 0; i<NUMPIXELS;i++){
		if (msgPercentage[i] > i){
			pixels.setPixelColor(i, pixels.Color(0,0,255));
		} else {
			pixels.setPixelColor(i, pixels.Color(0,0,0));
		}
	}
	pixels.show();
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
