#include <Servo.h>
#define BUTTON_PIN1 7
#define BUTTON_PIN2 8
Servo myservo;  // create servo object to control a servo

int val;    // variable to read the value from the analog pin
int pos = 30; 

int photocellPin = 0;     // the cell and 10K pulldown are connected to a0
int photocellReading;     // the analog reading from the analog resistor divider
int count = 0;
int light_threshold = 0; //Used later for calibrating light sensor
bool in_withdrawal_mode = false; //Used for loop later

#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
// taken from arduino website https://docs.arduino.cc/learn/electronics/lcd-displays
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// This was taken and adjusted from the sweep program on https://docs.arduino.cc/learn/electronics/servo-motors
void eject_coin(){
  for (pos = 0; pos <=40; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }

  for (pos = 40; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  count--;
}

//Prints the current balance

void print_balance(){
  lcd.clear();
  lcd.print("Balance: ");
  lcd.print(count);
}

void setup() {
  
//calibrating photocell

  for(int i = 0; i < 10; i++){
    light_threshold += analogRead(photocellPin);
  }

  light_threshold = light_threshold/10;
  light_threshold = light_threshold/2;

//output the light threshold briefly

  lcd.clear();
  lcd.print(light_threshold);
  delay(1000);


  myservo.attach(6);  // attaches the servo on pin 6 to the servo object

  //set up buttons
  pinMode(BUTTON_PIN1, INPUT_PULLUP);
  pinMode(BUTTON_PIN2, INPUT_PULLUP);

  
  print_balance(); //print initial balance of 0
}

void loop() {

  //adjusts balance when a coin is deposited. Sense when the photocell is blocked and delays so as to not run up the count.
  //This line taken from https://learn.adafruit.com/photocells/arduino-code

  photocellReading = analogRead(photocellPin);  

  if(photocellReading < light_threshold){
    count++;
    print_balance();
    delay(1000);
  }

  //Entering withdrawal mode

  if(digitalRead(BUTTON_PIN2) == LOW){

    //prevents entering  loop when no coins are present

    if(count <= 0){
      lcd.clear();
      lcd.print("InsufficentFunds");
      delay(2000);
    }else{

      //set up withdrawal mode

      in_withdrawal_mode = true;
      int withdrawal_count = 0;
      lcd.clear();
      lcd.print("Withdrawal Mode");
      delay(1000);
      lcd.clear();
      lcd.print("Withdrawing: ");
      lcd.print(withdrawal_count);

      //entering while loop

      while(in_withdrawal_mode){
        if(digitalRead(BUTTON_PIN1) == LOW){
          delay(500);

          if(withdrawal_count >= count){
            //Signal overdraw and reset screen

            lcd.clear();
            lcd.print("OverDrawn");
            delay(1000);
            lcd.clear();
            lcd.print("Withdrawing: ");
            lcd.print(withdrawal_count);  

          }else{
            //Adjust the withdraw amount and adjust screen

            withdrawal_count++;
            lcd.clear();
            lcd.print("Withdrawing: ");
            lcd.print(withdrawal_count);  

          }
        }else if(digitalRead(BUTTON_PIN2) == LOW){
          //When right button is clicked again, deposit coins and through flag to exit loop

          delay(500);
          for(int i = 0; i < withdrawal_count; i++){
            eject_coin();
            print_balance();
          }
          in_withdrawal_mode = false;        
        }
      }
    }

    //Signify end of withdrawal mode
    
    lcd.clear();
    lcd.print("Withdrawal Ended");
    delay(1000);
    print_balance();
  }

}


