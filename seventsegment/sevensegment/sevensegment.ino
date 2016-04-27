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




void setup()   {



  pinMode(clk, OUTPUT);
  pinMode(data_bit, OUTPUT);
  pinMode(data_bit2, OUTPUT);


  digitalWrite(clk, 0);
  digitalWrite(data_bit, 0);
  digitalWrite(data_bit2, 0);


} // end setup


void loop()   {
  int i, j;
  delay(500);
  for (i = 0; i <= 99; i++)   {
    displayBilangan(i);
    delay(100);
  }
}

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
