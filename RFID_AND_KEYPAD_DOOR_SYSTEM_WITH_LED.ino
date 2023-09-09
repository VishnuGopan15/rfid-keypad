#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Keypad.h>
#include <Servo.h>

#define SS_PIN    10
#define RST_PIN   9
#define ROW_NUM    4  // four rows
#define COLUMN_NUM 4  // four columns
#define SERVO_PIN  A0 // // the Arduino pin, which connects to the servo motor

MFRC522 rfid(SS_PIN, RST_PIN);


byte authorizedUID1[4] = {0x57, 0xA7, 0xB7, 0x89};
byte authorizedUID2[4] = {0x96, 0xD9, 0xC6, 0x5F};

Servo servo; // servo motor

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pin_rows[ROW_NUM] = {A4, A3, A2, A1}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

const String password_1 = "1234"; // change your password here
const String password_2 = "5642";   // change your password here
const String password_3 = "9765";    // change your password here
String input_password;

int angle = 0; // the current angle of servo motor
unsigned long lastTime;

void setup() {
  Serial.begin(9600);
  input_password.reserve(32); // maximum password size is 32, change if needed

  servo.attach(SERVO_PIN);
  servo.write(0); // rotate servo motor to 0°
  lastTime = millis();

//RFID
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522
  servo.attach(SERVO_PIN);
  servo.write(angle); // rotate servo motor to 0°

  Serial.println("Tap RFID/NFC Tag on reader");
  pinMode(6, OUTPUT);
 pinMode(7, OUTPUT);
}


void loop() {
  char key = keypad.getKey();

  if (key) {
    Serial.println(key);

    if (key == 'A') {
      input_password = ""; // reset the input password
    } else if (key == 'B') {
      if (input_password == password_1 || input_password == password_2 || input_password == password_3) {
        Serial.println("The password is correct, rotating Servo Motor to 90°");
        Serial.println("WELCOME VISHNU");
         digitalWrite(6, HIGH);
        angle = 90;
        servo.write(angle);
        lastTime = millis();
      } else {
        Serial.println("The password is incorrect, try again");
        digitalWrite(7, HIGH);
        delay(5000);
        digitalWrite(7, LOW);
      }

      input_password = ""; // reset the input password
    } else {
      input_password += key; // append new character to input password string
    }
  }

  if (angle == 90 && (millis() - lastTime) > 10000) { // 5 seconds
    angle = 0;
    servo.write(angle);
    Serial.println("Rotating Servo Motor to 0°");
    digitalWrite(6,LOW);
  }

//RFID

if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

      if (rfid.uid.uidByte[0] == authorizedUID1[0] &&
          rfid.uid.uidByte[1] == authorizedUID1[1] &&
          rfid.uid.uidByte[2] == authorizedUID1[2] &&
          rfid.uid.uidByte[3] == authorizedUID1[3] ) {
        Serial.println("Authorized Tag 1");
        Serial.println("WELCOME VISHNU");
        digitalWrite(6, HIGH);
        changeServo();
      } else if (rfid.uid.uidByte[0] == authorizedUID2[0] &&
                 rfid.uid.uidByte[1] == authorizedUID2[1] &&
                 rfid.uid.uidByte[2] == authorizedUID2[2] &&
                 rfid.uid.uidByte[3] == authorizedUID2[3] ) {
        Serial.println("Authorized Tag 2");
        Serial.println("WELCOME ALIN");
        digitalWrite(6, HIGH);
        changeServo();
      } else {
        Serial.print("Unauthorized Tag with UID:");
        digitalWrite(7, HIGH);
        for (int i = 0; i < rfid.uid.size; i++) {
          Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
          Serial.print(rfid.uid.uidByte[i], HEX);
        }
        Serial.println();
      }

      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }
}

void changeServo() {
  // change angle of servo motor
  if (angle == 0)
    angle = 90;
  else //if(angle == 90)
    angle = 0;

  // control servo motor arccoding to the angle
  servo.write(angle);
  Serial.print("Rotate Servo Motor to ");
  Serial.print(angle);
  Serial.println("°");
}
