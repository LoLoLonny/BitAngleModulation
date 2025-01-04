
#include <fix_fft.h>

  /////////////////
 //fft varialbes//
/////////////////

//imaginary data
char im[128];
//real data
char data[128];
//average of the real data 64 -> 6
char data_avgs[7];

  ///////////////
 //first loop?//
///////////////
boolean startUp = 1;

  ///////////
 //letters//
///////////

byte _a[] = {B11111111, B11111111, B11010000, B11010000, B11111111, B11111111};
byte _d[] = {B00011111, B00011111, B00011011, B00011011, B11111111, B11111111};
byte _e[] = {B11111111, B11111111, B11011011, B11011011, B11011011, B11011011};
byte _r[] = {B11111111, B11111111, B11011100, B11011110, B11111011, B11111001};
byte _o[] = {B11111111, B11111111, B11000011, B11000011, B11111111, B11111111};
byte _b[] = {B11111111, B11111111, B11010011, B11010011, B11111111, B11101111};
byte _t[] = {B11000000, B11000000, B11111111, B11111111, B11000000, B11000000};
byte _y[] = {B11000000, B01110000, B00011111, B00011111, B01110000, B11000000};
byte _space[] = {B00000000, B00000000, B00000000, B00000000, B00000000, B00000000};
byte *wordPointer[12] = {_r, _o, _b, _o, _t, _o, _r, _e, _a, _d, _y};

  /////////////////////
 //RGB BAM Variables//
/////////////////////

int g_tick = 1; //for decoupling of animation and flickerrate
int bitpos = 0 ;
int colorValue = 0; //for hsv animation
byte countBit = 128;
int rowPosition = 1;
byte blue = 0;
byte green = 0;
byte red = 0;
byte led;
byte row = 1;
byte ledCounter = 128;
byte rowCounter = 128;

  ////////////////////////////////
 //Frequency analyisis Variables//
////////////////////////////////

int volume;
int incomingAudio;
//clipping
boolean clipping =0;
//storage
byte newData = 0;
byte prevData = 0;
//frequency
unsigned int timer = 0;//counts period of wave
unsigned int period;
int frequency;

  //////////////
 //PORTB Pins//
/////////////

int latchPin = 10;
//Pin connected to SH_CP of 74HC595
int clockPin = 13;
////Pin connected to DS of 74HC595
int dataPin = 11;

int latchPinPORTB = latchPin - 8;
int clockPinPORTB = clockPin - 8;
int dataPinPORTB = dataPin - 8;



void setup()
{

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  PORTD = 0x00;
  DDRD = B11000000;
  DDRD = 0xFF;

  cli(); //stop all interrupts

   ////////////////////
  //initialize TIMER//
 ////////////////////
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 1;            // compare match register
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= ((1<<CS21)|(1<<CS20)) ;
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt

   //////////////////
  //initialize ADC//
 ////////////////// Source: http://www.instructables.com/id/Arduino-Frequency-Detection/step3/Sine-Wave-Frequency-Detection/
  ADCSRA = 0;
  ADCSRB = 0;

  ADMUX |= (1 << REFS0); //set reference voltage
  ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only

  ADCSRA |= (1 << ADPS2) | (1 << ADPS0); //set ADC clock with 32 prescaler- 16mHz/32=500kHz
  ADCSRA |= (1 << ADATE); //enabble auto trigger
  ADCSRA |= (1 << ADEN); //enable ADC
  ADCSRA |= (1 << ADSC); //start ADC measurements

  sei(); //start all interrupts

  Serial.begin(9600);

}

   ///////////////////
  //Timer interrupt//
 ///////////////////

ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
  countBit <<=1;
  bitpos ++ ;
  bitpos &= 7;
  //reset timercompare and countBit
  if( countBit == 0 ) {
    OCR1A= 1;
    countBit  = 1;
  }

  //latch low
  bitClear(PORTB, latchPinPORTB);

     ///////
    //row//
   ///////

  for (int i=0; i<8; i++){
    // clock low
    bitClear(PORTB, clockPinPORTB);
    //check led position and brightness
    //and set Bit on Datapin if countBit in Brightness
    if (rowCounter&row){
      bitSet(PORTB, dataPinPORTB);
    }
    else {
      bitClear(PORTB, dataPinPORTB);
    }
    // clock low (register bit)
    bitSet(PORTB, clockPinPORTB);
    rowCounter >>= 1;
    if( rowCounter == 0 ) {
    rowCounter  = 128;
    }
  }

  /* The loops here are redundant but BAM needs to run as fast as possible to avoid flickering.
  That's why we can't afford function calls during the interrupt. Also Arduino does not support the inline keyword.
  For this prototype I don't really care about the repetition.
  TODO: Use a macro or try always_inline attribute
  */

     ////////
    //blue//
   ////////
  
  for (int i=0; i<8; i++){
    // clock low
    bitClear(PORTB, clockPinPORTB);
    //check led position and brightness
    //and set Bit on Datapin if countBit in Brightness
    if (ledCounter&led && countBit&blue){
      bitClear(PORTB, dataPinPORTB);
    }
    else {
      bitSet(PORTB, dataPinPORTB);
    }
    // clock low (register bit)
    bitSet(PORTB, clockPinPORTB);
    ledCounter >>= 1;
    if( ledCounter == 0 ) {
    ledCounter  = 128;
    }
  }


     /////////
    //green//
   /////////
  
  for (int i=0; i<8; i++){
    // clock low
    bitClear(PORTB, clockPinPORTB);
    //check led position and brightness
    //and set Bit on Datapin if countBit in Brightness
    if (ledCounter&led && countBit&green){
      bitClear(PORTB, dataPinPORTB);
    }
    else {
      bitSet(PORTB, dataPinPORTB);
    }
    // clock low (register bit)
    bitSet(PORTB, clockPinPORTB);
    ledCounter >>= 1;
    if( ledCounter == 0 ) {
    ledCounter  = 128;
    }
  }


     ///////
    //red//
   ///////
  
  for (int i=0; i<8; i++){
    // clock low
    bitClear(PORTB, clockPinPORTB);
    //check led position and brightness
    //and set Bit on Datapin if countBit in Brightness
    if (ledCounter&led && countBit&red){
      bitClear(PORTB, dataPinPORTB);
    }
    else {
      bitSet(PORTB, dataPinPORTB);
    }
    // clock low (register bit)
    bitSet(PORTB, clockPinPORTB);
    ledCounter >>= 1;
    if( ledCounter == 0 ) {
    ledCounter  = 128;
    }
  }

  // latch high to shift out data
  bitSet(PORTB, latchPinPORTB);

  //shift timer compare and set timer back to generate delay (Bit Angle Modulation)
  OCR1A <<= 1;
  TCNT1 = 0;
  if (bitpos == 7) g_tick = 1 ;
}

void loop(){
  byte rowCounter = 1;

   //////////////////
  //font animation//
 //////////////////

  if(startUp){

  for (int letterCounter=0; letterCounter<11;){
    byte temp [] = {0,0,0,0,0,0};

    for (int i=0; i<12; i++){ // counting animation steps - 6 steps for fade in and 6 for fade out
        if (i == 1){colorValue += 40;
          if (colorValue >360) colorValue = 1;
      }

      hsvToRgb(colorValue, 1, 1);

      if(i<6){  // if letter right from center (first 6 steps)
        for (int j=0; j<i%6+1 ;j++){ // iterate through arrays (r[] and temp[]) modolo restricts i to (0 - 6)
          temp[5-i%6+j+1] = wordPointer[letterCounter][j]; // write r[] to the current animationstate temp[];
        }
      }
      else {  // if letter left from center (second 6 steps)
        for (int j=0; j<6 ;j++){
          if(j>5-i%6){ // overvrite last i indexes with 0 --> depending on animation state
            temp[j] = B00000000;
          }
          else {  // write r [] to temp "shifted" i to the left
            temp[j] = wordPointer[letterCounter][j+i%6];}
        }
      }

      //hold display steps for 25 processing cycles
      for (int hold=0; hold<15; hold++){
        while(g_tick==0){ /*wait for g_tick to be non-zero*/ }
        g_tick = 0 ; //consume the tick
        // display animation
        for (int x=0; x<6; x++){
          if (row>32)row=1;
          led = temp[x];
          delay(1);
          row <<=1;
        }
      }
    }
    letterCounter++;
   }

   // set startup false
   startUp = 0;
}

   //////////////////
  //fft animation//
 //////////////////
  
  hsvToRgb(120, 1, 1);
  int static i = 0;
  int val; //ADHC storage

  if (i < 128){
    val = ADCH;
    data[i] = val - 128;
    im[i] = 0;
    i++;}

  else{
    //this could be done with the fix_fftr function without the im array.
    fix_fft(data,im,7,0);
    // I am only interessted in the absolute value of the transformation
    for (i=0; i< 64;i++){
       data[i] = sqrt(data[i] * data[i] + im[i] * im[i]);
       }
    // convert data array to a resolution which is suitable for the matrix
    for (i=1; i<7; i++) {
      data_avgs[i] = (data[i*4] + data[i*4 + 1] + data[i*4 + 2] + data[i*4 + 3]);   // average together
      data_avgs[i] = map(data_avgs[i], 3, 30, 0, 8);                              // remap values to a resolution of 0-8 ignore 0-3 of data to make a "noise gate"
      }

    //animate ftt values
    for (int y=0; y<5;y++){ // hold animation = flickering vs. speed
    for (int x=1; x<7; x++){
      if (row>32)row=1;
      led = (1<<int(data_avgs[x]))-1; // leds from 0 to data_avgs[x]
      delay(2);
      row <<=1;
      }
    }
    }
  }



  /////////////////////////
 //Hsv to Rgb conversion//
/////////////////////////

 void hsvToRgb(float h, float s, float v){
    float r;
    float g;
    float b;
    int i = h/60;
    float f = h/60 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - s * (1 - f));

    switch(i){
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }
    red = r*255;
    green = g*255;
    blue = b*255;
}
