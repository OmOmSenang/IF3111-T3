//initializes/defines the output pin of the LM35 temperature sensor
int temperaturepin= 0;
bool turnon = true;
//this sets the ground pin to LOW and the input voltage pin to high

#define switchButtonPin 2

#define switchLightPin 10

#define fanPwmPin 11

#define celsiusRange (celsiusMax-celsiusMin)
int celsiusMin;
int celsiusMax;

#define CMDLENGTH 6 //lihat parser di bawah

void setup()
{

celsiusMin = 17;
celsiusMax = 34;
Serial.begin(9600);
pinMode(switchButtonPin, INPUT);
pinMode(switchLightPin, OUTPUT);
attachInterrupt(digitalPinToInterrupt(switchButtonPin), switchTurnOnOffButton, RISING);  
pinMode(fanPwmPin ,OUTPUT);
setupSevenSegment();
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available() >= CMDLENGTH){
    handleSerialCommand();
  }
}


//main loop
void loop()
{
  if (turnon){
    digitalWrite(switchLightPin,HIGH);
    float humidity = getAverageHumidity(100);
    delay(100);
    float celsius= averageReadTempCelsius(100);

  
    int vKipas = (celsius - celsiusMin)*255/celsiusRange*humidity/100;
    if (vKipas>255)vKipas=255;
    if (vKipas<0) vKipas=0;
    
    analogWrite(fanPwmPin,vKipas);
        
    Serial.print(celsius);
    Serial.print(" degrees Celsius, ");

    Serial.print(humidity);
    Serial.print(" % RH, vKipas: ");
    
    Serial.print(vKipas*100/255);
    Serial.println("%");
    
    Serial.println("");
    Serial.println("vKipas based on temp, linear with maxtemp and mintemp, with humidity factor");
    Serial.print("maxtemp: ");Serial.print(celsiusMax);
    Serial.print(";mintemp: ");Serial.println(celsiusMin);
    Serial.println("config: TMAX XX or TMIN XX, XX number");
    Serial.println(" ");

    displayBilangan(celsius);
    delay(1000);
  }else{
    digitalWrite(switchLightPin,LOW);
    analogWrite(fanPwmPin,0);
  }

}

float averageReadTempCelsius(int n){
  int sum=0;
  for (int i=0;i<n;i++){
    sum+=readTempCelsius();
  }
  return sum/n;
  
}

float readTempCelsius(){
  
    int rawvoltage= analogRead(temperaturepin);
    float millivolts= (rawvoltage/1024.0) * 5000;
    return millivolts/10;
}

/*  Humidity sensor
*
*   https://sites.google.com/site/measuringstuff/other
*   theory explained and lots of code taken from:
*   http://arduino.cc/en/Tutorial/CapacitanceMeter
*
*/

// humidity sensor defs -----------------------------------------------------------------------------
// analog pin for measuring capacitor voltage
#define humidityAnalogPin      1
// pin to charge the capacitor - connected to one end of the charging resistor
#define humidityChargePin      7     
// pin to discharge the capacitor
#define humidityDishargePin   8         
// change this to whatever resistor value you are using in MegOhms
// F formatter tells compliler it's a floating point value
#define resistorValue  10.0F 
#define RHslope        0.34F
#define RHconstant     215.36F
                                  
unsigned long startTime;          // time in microseconds
unsigned long elapsedTime;        // time in microseconds
float picoFarads;
int humidity;

int getAverageHumidity(int n)
{
  int sum=0;
  for (int i=0;i<n;i++){
    sum+=getHumidity();
  }
  return sum/n;
}

int getHumidity()
{          
  // set humidityChargePin to output
  pinMode(humidityChargePin, OUTPUT);     
  digitalWrite(humidityChargePin, LOW);
  // set discharge pin to output 
  pinMode(humidityDishargePin, OUTPUT);  
  // set discharge pin LOW 
  digitalWrite(humidityDishargePin, LOW);          
  while(analogRead(humidityAnalogPin) > 10)
  {       
     // make sure the cap is discharged
  }
  // makes it high impedance
  pinMode(humidityDishargePin, INPUT);
  // set humidityChargePin HIGH and capacitor charging  
  digitalWrite(humidityChargePin, HIGH); 
  startTime = micros();
  while(analogRead(humidityAnalogPin) < 648)
  {       
     // 647 is 63.2% of 1023, which corresponds to full-scale voltage 
  }
  elapsedTime= micros() - startTime;
  // picoFarads is given when using microseconds and megohms 
  picoFarads = (((float)elapsedTime) / resistorValue);   
    
  humidity=(int)((picoFarads-RHconstant)/RHslope);

  if (humidity>100) return 100;
  if (humidity<0) return 0;
  
  return humidity;   
}

#define minPushButtonSignalDelay 100
int lastSignalTime=0;
void switchTurnOnOffButton(){
  int signalTime = millis();
  if (signalTime-lastSignalTime>minPushButtonSignalDelay)
  {
    if (turnon){
      Serial.println("switch off");
      turnon=false;
      digitalWrite(switchLightPin,LOW);
    }else{
      Serial.println("switch on");
      digitalWrite(switchLightPin,HIGH);
      turnon=true;
    }
  }
  lastSignalTime=signalTime;
}

void handleSerialCommand(){
  if (Serial.read()=='T'){
    if (Serial.read()=='M'){
      char a = Serial.read();
      if (a =='A'){
        if (Serial.read()=='X'){
          if ( Serial.read()==' '){
            char firstdigit = Serial.read();
            if (firstdigit>='0'&&firstdigit<='9'){
              char secondDigit = Serial.read();
              if (secondDigit>='0'&&secondDigit<='9'){
                celsiusMax = (firstdigit-'0')*10+(secondDigit-'0');
              }
            }
            
          }
        }
      }else if (a == 'I'){
        if (Serial.read()=='N'){

          if ( Serial.read()==' '){
            char firstdigit = Serial.read();
            if (firstdigit>='0'&&firstdigit<='9'){
              char secondDigit = Serial.read();
              if (secondDigit>='0'&&secondDigit<='9'){
                celsiusMin = (firstdigit-'0')*10+(secondDigit-'0');
              }
            }
            
          }
        }
      }
    }
  }
}

/* Comment

Uses a 74C164 shift register to count from 0 t0 F (HEX)
on a 7 segment common anode LED display.

To use a common cathode display change line
digitalWrite(data_bit, !k); ->   digitalWrite(data_bit, k);

connect common to GND

 This program assumes a seven segment display with a bit pattern:

Q7 segment a
Q6 segment b
Q5 segment c
Q4 segment d
Q3 segment e
Q2 segment f
Q1 segment g
Q0 segment dp

*/

#define data_bit 12
#define data_bit2 13
#define clk 9 // pin connected to display segment 'a'  


// segment patterns 0 - 9, A - F Hex + dp
byte Digit[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66,
                0x6d, 0x7d, 0x07, 0x7f, 0x67, 0x77, 0x7c, 0x39,
                0x5e, 0x79, 0x71, 0x80
               };




void setupSevenSegment()   {



  pinMode(clk, OUTPUT);
  pinMode(data_bit, OUTPUT);
  pinMode(data_bit2, OUTPUT);


  digitalWrite(clk, 0);
  digitalWrite(data_bit, 0);
  digitalWrite(data_bit2, 0);


} // end setup

void displayBilangan(int n){
  if (n<100){
    int digitdepan = n/10;
    int digitbelakang = n - digitdepan*10;
    
    digitOut(Digit[digitbelakang],Digit[digitdepan]);
  }
  
}

void   digitOut(byte j,byte l)   {
  byte k;
  byte m;
  for (int i = 0; i < 8; i++)   {
    k = j & 0x01;
    m = l &0x01;
    digitalWrite(data_bit2, !k);
    digitalWrite(data_bit, !m);
    // use just k instead of !k for common cathode displays.


    pulsout(clk, 1);

    j = j >> 1;
    l = l >> 1;
  }

}



// toggle the output state on a pin
void toggle(int pinNum)
{
  byte pinState = digitalRead(pinNum);
  pinState = !pinState;
  digitalWrite(pinNum, pinState);
}


// inverts state of pin, delays, then reverts state back
void    pulsout(byte x, int y)   {
  byte z = digitalRead(x);
  z = !z;
  digitalWrite(x, z);
  delayMicroseconds(10 * y);
  z = !z; // return to original state
  digitalWrite(x, z);
  return;
} // end pulsout()
