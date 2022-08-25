#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Keypad.h>
#include <SPI.h>
#include <SoftwareSerial.h>

//relay setup
#define RELAY_PIN 2
void Door_Open();

//Keypad Setup
const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {36, 34, 32, 30};
byte colPins[COLS] = {28, 26, 24, 22}; 
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
int Keypad_Input(void);


//OLED i2C display setup
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//RFID setup
#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above
MFRC522 mfrc522(SS_PIN, RST_PIN);


//SIM900A setup
SoftwareSerial SIM900A(A8, A9);
void SendMessage();


//initial variable
int Secret_Code = 1234;
bool rfid_flag = 1;
const char* NAME;
String ID_e;
int value = 0;
bool done_flag = 0;
int wrong=0;

void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  SIM900A.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  Serial.println("SIM900A Ready");
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(1, 1);
  display.print("RFID Door Lock");
  display.display();
  
  SPI.begin();
  mfrc522.PCD_Init();
  delay(1000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 10);
  display.print("Scan tag");
  display.display();
  delay(1000);

}

void loop() {
  // put your main code here, to run repeatedly:
  char key=keypad.getKey();
  if (wrong >=2)
  {
    SendMessage();
    wrong=0;
   }
   if (key == 'A') // If 'A' is pressed, goto pincode mode
  {
    Serial.println("A");
    Serial.println("Pin Code");
    display.clearDisplay();
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(10, 0);            // Start at top-left corner
    display.println(F("Pin Code"));
    display.setCursor(25, 20);
    display.println(F("Mode"));
    display.display();
    delay(1000);

    Serial.println("Enter the \nFour Digit \nCode");

    rfid_flag = 0;
  }
  if (rfid_flag == 1)
    {
      
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(10, 10);
      display.print("Scan Tag");
      display.display();

      if ( ! mfrc522.PICC_IsNewCardPresent())
      {
        goto reset;
      }
      // Select one of the cards
      if ( ! mfrc522.PICC_ReadCardSerial())
      {
        goto reset;
      }

      String content = "";
      byte letter;
      for (byte i = 0; i < mfrc522.uid.size; i++)
      {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        content.concat(String(mfrc522.uid.uidByte[i], HEX));
      }
       if (content.substring(1) == "c3 c8 a0 1a") //change here the UID of the card that you want to give access
      {
        Serial.print("!!--");
        Serial.println(content.substring(1));
        NAME = "Hafid";
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(1, 1);
        display.println("Welcome ");
        display.println(NAME);
        display.setCursor(1, 20);
        display.print("Door Open");
        display.display();
        Door_Open();
        wrong=0;
        delay(1000);
      }
      else if (content.substring(1)=="05 8f a0 4d 40 a1 00") 
      {        
        Serial.print("!!--");
        Serial.println(content.substring(1));
        NAME = "KTP Hafid";
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(1, 1);
        display.println("Welcome ");
        display.println(NAME);
        display.setCursor(1, 20);
        display.print("Door Open");
        display.display();
        Door_Open();
        wrong=0;
        delay(1000);
        
        }
      else
      {
        Serial.println("Not Registered");
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(38, 1);
        display.print("Not");
        display.setCursor(5, 10);
        display.print("Registered");
        display.display();
        Serial.println("You can't enter Studio.");
        Serial.println(content.substring(1));
        wrong++;
        delay(1000 );
      }
      content.substring(1) = "";


    }
     if (rfid_flag == 0)
    {

      if (Keypad_Input() == Secret_Code) // Checking the code entered by user
      {
        Serial.println("Correct Code");
        display.clearDisplay();
        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);        // Draw white text
        display.setCursor(10, 10);            // Start at top-left corner
        display.println("Correct");
        display.setCursor(10, 20);            // Start at top-left corner
        display.println("Code");
        display.display();
        Door_Open();
        rfid_flag = 1;
        wrong=0;
        delay(1000);
        
      }
      else
      {
        Serial.println("Wrong Code");
        display.clearDisplay();
        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);        // Draw white text
        display.setCursor(10, 10);            // Start at top-left corner
        display.println(("Wrong Code"));
        display.display();
        delay(1000);
        display.clearDisplay();
        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);        // Draw white text
        display.setCursor(35, 20);            // Start at top-left corner
        display.println(("Code"));
        display.display();
        wrong+=1;
        delay(1000);
      }
      ID_e = "";
    }

reset:{}

  }
int Keypad_Input(void)
  {
    int i = 0;
    display.clearDisplay();
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0, 0);            // Start at top-left corner
    display.println(("Enter Code"));
    display.display();
    i = 0;
    ID_e = "";
    value = 0;
    while (1)
    {
      char key = keypad.getKey();

      if (key && i < 4)
      {
        ID_e = ID_e + key;
        value = ID_e.toInt();
        Serial.println(value);
        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);        // Draw white text
        display.setCursor(i * 35, 50);            // Start at top-left corner
        display.println(("*"));
        display.display();
        i++;
      }
      if (key == '#')
      {
        done_flag = 1;
        Serial.println("DONE");
        i = 0;
        ID_e = "";
        return (value);
      }
      if (key == 'B')
      {
        rfid_flag = 1;
        ID_e = "";
        return (0);
      }
    }
  }
void SendMessage()
{
  Serial.println("Sending Message");
  SIM900A.println("AT+CMGF=1");
  delay(1000);
  Serial.println("Set SMS Number");
  SIM900A.println("AT+CMGS=\"+6281649641087\"\r");
  delay(1000);
  Serial.println("Set SMS Content");
  SIM900A.println("Kesalahan password/RFID lebih dari 3x! Mohon Berhati-hati");
  delay(100);
  Serial.println("Finish");
  SIM900A.println((char)26);
  delay(1000);
  Serial.println ("Message has been sent");
  wrong=0;
}
void Door_Open() 
{
  digitalWrite(RELAY_PIN, HIGH);
  Serial.println("DOOR OPENED");
  delay(5000);
  digitalWrite(RELAY_PIN, LOW);
  Serial.println("DOOR CLOSED");
  mfrc522.PCD_Init();
  }
