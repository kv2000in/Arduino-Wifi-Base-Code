/*
OC0 A PD6 D6 Timer 0 980 Hz PWM with analogwrite(). Timer 0 is used for delay and millis().
OC0 B PD5 D5 Timer 0 980 Hz PWM with analogwrite(). Timer 0 is used for delay and millis().

OC1A  PB1 D9 490 Hz PWM with analogwrite(). Timer 1. Can't use Timer 1 if using Servo library
OC1B  PB2 D10 490 Hz PWM with analogwrite(). Timer 1. Can't use Timer 1 if using Servo library

OC2A PB3 D11 490 Hz PWM with analogwrite(). Timer 2.
OC2B PD3 D3 490 Hz PWM with analogwrite(). Timer 2. D3 is also INT 1 (hardware interrupt).

PD2 D2 INT0
PD3 D3 INT1 (and see above)

A4 SDA for i2c
A5 SCL for i2c

Can use analog pins as outputs to drive servos using  servo library.
When using  analog functions, the pins are numbered 0-5. but these same pins are numbered 14-19 when used with digital functions.

Servo 50 Hz PWM - Period = 1/50 S = 20 ms.
1 ms High, 19 ms Low - Full left, 1.5 ms High, 18.5 ms Low - middle, 2 ms High, 18 ms Low - full right.


*/



#include <Servo.h>

//*************************************i2c smbus smart battery***************

//i2c PIN Definitions - need to be placed before #include <SoftI2CMaster.h> for it to compile
#define SCL_PIN 5 //A5 but need to use 5 and not 19. It is pin 5 of PORTC
#define SCL_PORT PORTC  //Need for compiling
#define SDA_PIN 4 //A4 but need to use 4 and not 18. It is pin 4 of PORTC
#define SDA_PORT PORTC //Need for compiling
#include <SoftI2CMaster.h>

#define I2C_TIMEOUT 100
#define BATT_SMBUS_ADDR                     0x0B                ///< I2C address
#define BATT_SMBUS_ADDR_MIN                 0x08                ///< lowest possible address
#define BATT_SMBUS_ADDR_MAX                 0x7F                ///< highest possible address
//BUS MAPPINGS FROM DEV.3DR
#define BATT_SMBUS_TEMP                     0x08                ///< temperature register
#define BATT_SMBUS_VOLTAGE                  0x09                ///< voltage register
#define BATT_SMBUS_REMAINING_CAPACITY       0x0f                ///< predicted remaining battery capacity as a percentage
#define BATT_SMBUS_FULL_CHARGE_CAPACITY     0x10                ///< capacity when fully charged
#define BATT_SMBUS_DESIGN_CAPACITY          0x18                ///< design capacity register
#define BATT_SMBUS_DESIGN_VOLTAGE           0x19                ///< design voltage register
#define BATT_SMBUS_SERIALNUM                0x1c                ///< serial number register
#define BATT_SMBUS_MANUFACTURE_NAME         0x20                ///< manufacturer name
#define BATT_SMBUS_MANUFACTURE_DATA         0x23                ///< manufacturer data
#define BATT_SMBUS_MANUFACTURE_INFO         0x25                ///< cell voltage register
#define BATT_SMBUS_MANUFACTURE_ACCESS       0x00                ///< Manufacture access
#define BATT_SMBUS_MANUFACTURE_DEVICE_TYPE  0x01               ///< Manufacture access  
#define BATT_SMBUS_MANUFACTURE_F_VER        0x02               ///< Manufacture access  
#define BATT_SMBUS_MANUFACTURE_HW_VER       0x03               ///< Manufacture access  
#define BATT_SMBUS_MANUFACTURE_MANUF_STATUS 0x06               ///< Manufacture access  
#define BATT_SMBUS_MANUFACTURE_LED_ON       0x32               ///< Manufacture access
#define BATT_SMBUS_MANUFACTURE_LED_OFF      0x33               ///< Manufacture access 
#define BATT_SMBUS_MANUFACTURE_OP_STATUS    0x54               ///< Manufacture access 
#define BATT_SMBUS_MANUFACTURE_RESET        0x41               ///< Manufacture access
#define BATT_SMBUS_MANUFACTURE_SAFE_ACTIVE  0x30               ///< Manufacture access
#define BATT_SMBUS_MANUFACTURE_SAFE_CLEAR   0x31               ///< Manufacture access
#define BATT_SMBUS_CURRENT                  0x2a                ///< current register
#define BATT_SMBUS_MEASUREMENT_INTERVAL_US  (1000000 / 10)      ///< time in microseconds, measure at 10hz
#define BATT_SMBUS_TIMEOUT_US               10000000            ///< timeout looking for battery 10seconds after startup
#define BATT_SMBUS_BUTTON_DEBOUNCE_MS       300                 ///< button holds longer than this time will cause a power off event
 
#define BATT_SMBUS_PEC_POLYNOMIAL           0x07                ///< Polynomial for calculating PEC
#define BATT_SMBUS_I2C_BUS                  PX4_I2C_BUS_EXPANSION
//BUS MAPPINGS FROM SMBus PROTOCOL DOCUMENTATION
#define BATTERY_MODE             0x03
#define CURRENT                  0x0A
#define RELATIVE_SOC             0x0D
#define ABSOLUTE_SOC             0x0E
#define TIME_TO_FULL             0x13
#define CHARGING_CURRENT         0x14
#define CHARGING_VOLTAGE         0x15
#define BATTERY_STATUS           0x16
#define CYCLE_COUNT              0x17
#define SPEC_INFO                0x1A
#define MFG_DATE                 0x1B
#define DEV_NAME                 0x21   // String
#define CELL_CHEM                0x22   // String
#define CELL4_VOLTAGE            0x3C   // Indidual cell voltages don't work on Lenovo and Dell Packs
#define CELL3_VOLTAGE            0x3D
#define CELL2_VOLTAGE            0x3E
#define CELL1_VOLTAGE            0x3F
#define STATE_OF_HEALTH          0x4F
#define EXTENDED_SBS_OP_STATUS   0x54
#define EXTENDED_SBS_FET_STATUS  0x46
#define EXTENDED_SBS_FET_CONTROL 0x02               ///0x02 to turn on the discharge FET. 0x00 to turn off the discharge FET
#define EXTENDED_SBS_AFE_DATA    0x45 // This read-block function returns the AFE memory map (9-bytes), beginning with AFE.0x00 as the MSB, and the internal AFE_Fail_Counter (2-bytes) returned with a maximum length of 11 data bytes + 1 length byte.
//END BUS MAPPINGS
#define SAFETY_ALERT             0x50
#define SAFETY_STATUS            0x51
#define PACK_VOLTAGE             0x5A
#define PERM_FAILURE_STATUS      0x53
#define RESET_DATA               0x57
#define WD_RESET_DATA            0x58
#define bufferLen 32
uint8_t i2cBuffer[bufferLen];
// standard I2C address for Smart Battery packs
byte deviceAddress = BATT_SMBUS_ADDR;
//*************************************i2c smbus smart battery***************


//interrupt pins
#define ENCODER0PINA         2    //Atmega pin PD2/INT0 // interrupt pin (2,3 on Atmega328)
#define ENCODER1PINA         3    //Atmega pin PD3/INT1, pin 3 also PWM paired with 11.

//L293D pins
#define pinENA               5    //Atmega pin PD5 //PWM pin, grouped with pin 6
#define pinINA1              7    //Atmega pin PD7
#define pinINA2              8    //Atmega pin PB0
//Will have to tie ENA and ENB together. - Can control only 1 motor with PWM in this config.
#define pinENB               9    //Atmega pin PB1 //PWM pin, grouped with pin 10
#define pinINB1              12   //Atmega pin PB4
#define pinINB2              13   //Atmega pin PB5

//HCSR04 definitions
#define pinTRIG   9
#define pinECHO   10
long duration,distance;


//Create Servo objects. myservo 1 - direction (Left.Right steering), myservo2 - ladder 1 up and down, myservo2 ladder 2 up and down
Servo myservo1,myservo2,myservo3,myservo4;
#define pinSERVO1 15 //A1
#define pinSERVO2 16 //A2
#define pinSERVO3 17 //A3
#define pinSERVO4 4  //D4

//Lights and Sounds Pins
#define pinLIGHTS 6
#define pinSIREN 11

//Analog ADC pin for checking battery voltage
#define pinBATTERY A0

//Misc definitions
//#define MAX_STRING_LEN  32

long encoder0Position = 0;
long encoder1Position = 0;
int PPR=8; //pulses per rotation
int PWMA = 255;
int PWMB = 255;
// volatile variables - modified by interrupt service routine (ISR)
volatile long counter0=0;
volatile long counter1=0;
bool motorARunning = false;
bool motorBRunning = false;
long stepsA;
long stepsB;
char dirA;
char dirB;
/*
Commands:
C-F (Forward)
D-89 (Steering servo to 89 degree angle)
S-90 (Speed 90% of Max)
B-V (Battery voltage)
W-180 (Aux servo 1 at 180 degree angle)

*/
//Serial Read stuff
const byte numChars = 8;
char receivedChars[numChars]; // an array to store the received data
boolean newData = false;
//char LEFTofDELIM[1] = {0};
//char RIGHTofDELIM[numChars-2]; //2 chars less - one for leftofdelim and one for delim.
//
//// Function to return a substring defined by a delimiter at an index
//char* subStr (char* str, char *delim, int index) {
//  char *act, *sub, *ptr;
//  static char copy[MAX_STRING_LEN];
//  int i;
//
//  // Since strtok consumes the first arg, make a copy
//  strcpy(copy, str);
//
//  for (i = 1, act = copy; i <= index; i++, act = NULL) {
//     sub = strtok_r(act, delim, &ptr);
//     if (sub == NULL) break;
//  }
//  return sub;
//
//}


void servoposition(int angle){
  myservo1.write(angle);
  Serial.print("D-L:OK"); // todo - make it dynamic so that it responds based on the command received.
}
void setup()
{
  //Initialize Serial
 Serial.begin (57600);
 //Initialize i2C
 if(i2c_init()){                                             //i2c_start initialized the I2C system.  will return false if bus is locked.
    Serial.print("<I2C Inialized>");
 } else
    Serial.print("<I2C Failed>");


//Attach servos to A1, A2 , A3 & D4
myservo1.attach(pinSERVO1);
myservo2.attach(pinSERVO2);
myservo3.attach(pinSERVO3);
myservo4.attach(pinSERVO4);

// HR-SC04
//Define inputs and outputs
pinMode(pinTRIG, OUTPUT);
pinMode(pinECHO, INPUT);
 
//Interrupt pins
pinMode(ENCODER0PINA, INPUT_PULLUP);
pinMode(ENCODER1PINA, INPUT_PULLUP);
//attach interrupts 
attachInterrupt(digitalPinToInterrupt(ENCODER0PINA),onInterrupt0, RISING);
attachInterrupt(digitalPinToInterrupt(ENCODER1PINA),onInterrupt1, RISING); 
  

//L293D pins
pinMode(pinENA,OUTPUT);
pinMode(pinINA1,OUTPUT);
pinMode(pinINA2,OUTPUT);
pinMode(pinENB,OUTPUT);
pinMode(pinINB1,OUTPUT);
pinMode(pinINB2,OUTPUT);
  
analogWrite(pinENA, PWMA);
digitalWrite(pinINA1,LOW);
digitalWrite(pinINA2,LOW);
  
analogWrite(pinENB, PWMB);
digitalWrite(pinINB1,LOW);
digitalWrite(pinINB2,LOW);



}



//i2c fetchword function
int fetchWord(byte func)
{
    i2c_start(deviceAddress<<1 | I2C_WRITE);                //Initiates a transfer to the slave device with the (8-bit) I2C address addr.
                                                            //Alternatively, use i2c_start_wait which tries repeatedly to start transfer until acknowledgment received
    //i2c_start_wait(deviceAddress<<1 | I2C_WRITE);
    i2c_write(func);                                        //Sends a byte to the previously addressed device. Returns true if the device replies with an ACK.
    i2c_rep_start(deviceAddress<<1 | I2C_READ);             //Sends a repeated start condition, i.e., it starts a new transfer without sending first a stop condition.
    byte b1 = i2c_read(false);                              //i2c_read Requests to receive a byte from the slave device. If last is true,
                                                            //then a NAK is sent after receiving the byte finishing the read transfer sequence.
    byte b2 = i2c_read(true);
    i2c_stop();                                             //Sends a stop condition and thereby releases the bus.
    return (int)b1|((( int)b2)<<8);
}
//Call the function to get i2c batt info
void batteryinfo(){
//    Serial.print("Relative Charge(%): ");
//    Serial.println(fetchWord(RELATIVE_SOC));
//    Serial.print("Voltage: ");
//    Serial.println((float)fetchWord(BATT_SMBUS_VOLTAGE)/1000);
// 
//    Serial.print("Full Charge Capacity: " );
//    Serial.println(fetchWord(BATT_SMBUS_FULL_CHARGE_CAPACITY));
// 
    Serial.print("<Remaining Capacity:" );
    Serial.print(fetchWord(BATT_SMBUS_REMAINING_CAPACITY));
    Serial.print(">");
// 
//    Serial.print("Temp: ");
//    unsigned int tempk = fetchWord(BATT_SMBUS_TEMP);
//    Serial.println((float)tempk/10.0-273.15);
// 
//    Serial.print("Current (mA): " );
//    Serial.println(fetchWord(BATT_SMBUS_CURRENT));
  }

//HCSR04 distance function
void Obstacle(){
  digitalWrite(pinTRIG, LOW);
  delayMicroseconds(20);
  digitalWrite(pinTRIG, HIGH);
  delayMicroseconds(20);
  digitalWrite(pinTRIG, LOW);
  
  duration = pulseIn(pinECHO, HIGH);
  distance = (duration/2) / 29.1;
  Serial.print("<");
  Serial.print(distance);
  Serial.print(">");
  }  
void loop()
{
  //Read the Serial and move accordingly
  recvWithStartEndMarkers();
  
  showNewData();

    //  setPosition(DIR[0],VALUE);
      
  
  
  
  
}

// Minimalistic ISR

void onInterrupt0()
{

  counter0++;

}
void onInterrupt1()
{
  counter1++;

}
void rotateAF()
{
    analogWrite(pinENA, PWMA);
    digitalWrite(pinINA1,HIGH);
    digitalWrite(pinINA2,LOW);
    motorARunning=true;
}
void rotateAR()
{
    analogWrite(pinENA, PWMA);
    digitalWrite(pinINA1,LOW);
    digitalWrite(pinINA2,HIGH);
    motorARunning=true;
}
void rotateAStop()
{
  analogWrite(pinENA, 0);
  digitalWrite(pinINA1,LOW);
  digitalWrite(pinINA2,LOW);
  motorARunning=false;
}

void rotateBF()
{
    analogWrite(pinENB, PWMB);
    digitalWrite(pinINB1,HIGH);
    digitalWrite(pinINB2,LOW);
    motorBRunning=true;
}
void rotateBR()
{
    analogWrite(pinENB, PWMB);
    digitalWrite(pinINB1,LOW);
    digitalWrite(pinINB2,HIGH);
    motorBRunning=true;
}
void rotateBStop()
{
  analogWrite(pinENB, 0);
  digitalWrite(pinINB1,LOW);
  digitalWrite(pinINB2,LOW);
  motorBRunning=false; 

}

//Serial Read Functions
void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
 
 // if (Serial.available() > 0) {
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                //parseData();
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void showNewData() {
    if (newData == true) {
        Serial.print("<");
        Serial.print(receivedChars[0]); //C
        Serial.print(receivedChars[1]);
        Serial.print(receivedChars[2]);
        Serial.print(receivedChars[3]);
        Serial.print(receivedChars[4]);
        Serial.print(">");
        newData = false;
    }
}

/*
 Probably not needed if data coming in with fixed delims and length left of delim
void parseData() {

    // split the data into its parts
    
  char * strtokIndx; // this is used by strtok() as an index
  
  strtokIndx = strtok(receivedChars,"-");      // get the first part - the string
  strcpy(DIR, strtokIndx); // copy it to DIR
  //strcpy(DIR,0);
  //strtokIndx = strtok(NULL, "-"); // this continues where the previous call left off
  //VALUE = atoi(strtokIndx); // convert this part to an integer
  VALUE = atol(strtokIndx+2);   
 }
*/
void setPosition(char dir, long steps)
{


  if (newData)
  {
    if(dir == 'S'){rotateAStop();}
    if(dir == 's'){rotateBStop();}
    // It is new data from Serial - it wants to move the motors one way or the other
    // if stepsA<offshootA - no point turning the motor. Repeat for stepsB     
         if (dir=='F') //Single quotes - Double quotes don't work.
            {
                if (not (motorARunning)) {rotateAF(); stepsA=steps; dirA=dir;}
             }
         else if (dir=='R')
            {
               if (not (motorARunning)) {rotateAR();stepsA=steps;dirA=dir;}
            }
         else if (dir=='f') //Single quotes - Double quotes don't work.
            {
               rotateBF();
               Serial.println("<OK-BF>");
               // if (not (motorBRunning)) {rotateBF();stepsB=steps;dirB=dir;}
             }
         else if (dir=='r')
            {
               rotateBR();
               Serial.println("<OK-BR>");
               //if (not (motorBRunning)) {rotateBR();stepsB=steps;dirB=dir;}
            }
    newData=false;
    }

    //Now motors are running - count steps
    if ((motorARunning) and (stepsA<counter0))
            {
             rotateAStop();
             
             
             if (dirA=='F')
            {
                encoder0Position = encoder0Position+counter0;

             }
             else if (dirA=='R')
            {
               encoder0Position = encoder0Position-counter0;
               
            }
            
        counter0 =0;
        Serial.print("<0-");
        Serial.print(encoder0Position);
        Serial.println(">"); 
            }
     
    if ((motorBRunning) and (stepsB<counter1))
    {
              rotateBStop();
              
              
             if (dirB=='f')
            {
                
                encoder1Position = encoder1Position+counter1;
             }
         else if (dirB=='r')
            {
               
               encoder1Position = encoder1Position-counter1;
            }
        counter1 =0;
        Serial.print("<1-");
        Serial.print(encoder1Position);
        Serial.println(">");
            }
     


}
