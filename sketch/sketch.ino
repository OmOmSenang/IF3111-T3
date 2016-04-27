//initializes/defines the output pin of the LM35 temperature sensor
int temperaturepin= 0;
bool turnon = true;
//this sets the ground pin to LOW and the input voltage pin to high
void setup()
{
Serial.begin(9600);

pinMode(11,OUTPUT);
}

#define celsiusRange (celsiusMax-celsiusMin)
#define celsiusMin 17
#define celsiusMax 34

//main loop
void loop()
{
  if (turnon){
    float humidity = getAverageHumidity(100);
    delay(100);
    float celsius= averageReadTempCelsius(100);

  
    int vKipas = (celsius - celsiusMin)*255/celsiusRange*humidity/100;
    if (vKipas>255)vKipas=255;
    if (vKipas<0) vKipas=0;
    
    analogWrite(11,vKipas);
        
    Serial.print(celsius);
    Serial.print(" degrees Celsius, ");

    Serial.print(humidity);
    Serial.print(" % RH, vKipas: ");
    
    Serial.println(vKipas);
    delay(1000);
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
