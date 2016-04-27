//initializes/defines the output pin of the LM35 temperature sensor
int temperaturepin= 0;
bool turnon = true;
//this sets the ground pin to LOW and the input voltage pin to high

#define switchButtonPin 2

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
attachInterrupt(digitalPinToInterrupt(switchButtonPin), switchTurnOnOffButton, RISING);  
pinMode(fanPwmPin ,OUTPUT);
}

//main loop
void loop()
{
  if (Serial.available() >= CMDLENGTH){
    handleSerialCommand();
  }
  if (turnon){
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
    delay(1000);
  }else{
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
    }else{
      Serial.println("switch on");
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
