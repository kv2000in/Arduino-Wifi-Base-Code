/*
OC0 A PD6 D6 Timer 0 980 Hz PWM with analogwrite(). Timer 0 is used for delay and millis().
OC0 B PD5 D5 Timer 0 980 Hz PWM with analogwrite(). Timer 0 is used for delay and millis().

OC1A  PB1 D9 490 Hz PWM with analogwrite(). Timer 1. Can't use Timer 1 if using Servo library
OC1B  PB2 D10 490 Hz PWM with analogwrite(). Timer 1. Can't use Timer 1 if using Servo library

OC2A PB3 D11 490 Hz PWM with analogwrite(). Timer 2.
OC2B PD3 D3 490 Hz PWM with analogwrite(). Timer 2. D3 is also INT 1 (hardware interrupt).

PD2 D2 INT0
PD3 D3 INT1 (and see above)

A4 SDA for i2c NEED TO BE PULLED UP BY EXTERNAL 4.7K RESISTORS TO VCC
A5 SCL for i2c NEED TO BE PULLED UP BY EXTERNAL 4.7K RESISTORS TO VCC

Can use analog pins as outputs to drive servos using  servo library.
When using  analog functions, the pins are numbered 0-5. but these same pins are numbered 14-19 when used with digital functions.

Servo 50 Hz PWM - Period = 1/50 S = 20 ms.
1 ms High, 19 ms Low - Full left, 1.5 ms High, 18.5 ms Low - middle, 2 ms High, 18 ms Low - full right.


*/



#include <Servo.h>

//*************************************i2c smbus smart battery***************

//DEFINE SDA AND SCL PINS
  #define SCL_PIN 5 //A5                //COMMUNICATION PIN 20 ON MEGA
  #define SCL_PORT PORTC

  #define SDA_PIN 4 //A4                 //COMMUNICATION PIN 21 ON MEGA
  #define SDA_PORT PORTC

//CONFIGURE I2C MODES
  #define I2C_TIMEOUT 100           //PREVENT SLAVE DEVICES FROM STRETCHING LOW PERIOD OF THE CLOCK INDEFINITELY AND LOCKING UP MCU BY DEFINING TIMEOUT
  //#define I2C_NOINTERRUPT 1       //SET TO 1 IF SMBus DEVICE CAN TIMEOUT
  //#define I2C_FASTMODE 1          //THE STANDARD I2C FREQ IS 100kHz.  USE THIS TO PERMIT FASTER UP TO 400kHz.
  //#define I2C_SLOWMODE 1            //THE STANDARD I2C FREQ IS 100kHz.  USE THIS TO PERMIT SLOWER, DOWN TO 25kHz.
  #include <SoftI2CMaster.h>

/**********************************
 * CONFIGURE SERIAL LIBRARY
 **********************************/
  //#include <SoftwareSerial.h>
  //#include <Serial.h>
  //#include <Wire.h>


/**********************************
 * DEFINE VARIABLES AND SMBus MAPPINGS
 **********************************/
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


//Create Servo objects. myservoW - direction (Left.Right steering), myservoX - ladder 1 up and down, myservoY ladder 2 up and down
Servo myservoW,myservoX,myservoY,myservoZ;
#define pinSERVOW 15 //A1
#define pinSERVOX 16 //A2
#define pinSERVOY 17 //A3
#define pinSERVOZ 4  //D4

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
int PWMAMAX=0;
int PWMAMIN=0;
int PWMBMAX=0;
int PWMBMIN=0;
int MOTORAMAXVOLTAGE =6000.0; //6000 mV for yellow motor
int MOTORAMINVOLTAGE =4000.0; //4000 mV for yellow motor
int MOTORBMAXVOLTAGE =6000.0; //6000 mV for yellow motor
int MOTORBMINVOLTAGE =4000.0; //4000 mV for yellow motor
int PWMA ;
int PWMB ;

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
    digitalWrite(pinINA1,HIGH);
    digitalWrite(pinINA2,LOW);
    motorARunning=true;
}
void rotateAR()
{
    digitalWrite(pinINA1,LOW);
    digitalWrite(pinINA2,HIGH);
    motorARunning=true;
}
void rotateAStop()
{
  digitalWrite(pinINA1,LOW);
  digitalWrite(pinINA2,LOW);
  motorARunning=false;
}

void rotateBF()
{
    digitalWrite(pinINB1,HIGH);
    digitalWrite(pinINB2,LOW);
    motorBRunning=true;
}
void rotateBR()
{
    digitalWrite(pinINB1,LOW);
    digitalWrite(pinINB2,HIGH);
    motorBRunning=true;
}
void rotateBStop()
{
  digitalWrite(pinINB1,LOW);
  digitalWrite(pinINB2,LOW);
  motorBRunning=false; 

}

void moveauxservoW(char *angle){
  int myServoAngle=atoi(angle);
    myservoW.write(myServoAngle);
    Serial.print("<W-");
    Serial.print(myServoAngle);
    Serial.print(":OK>");
}

void moveauxservoX(char *angle){
    int myServoAngle=atoi(angle);
    myservoX.write(myServoAngle);
    Serial.print("<X-");
    Serial.print(myServoAngle);
    Serial.print(":OK>");
  
  }
void moveauxservoY(char *angle){
  int myServoAngle=atoi(angle);
    myservoY.write(myServoAngle);
    Serial.print("<Y-");
    Serial.print(myServoAngle);
    Serial.print(":OK>");
}
void moveauxservoZ(char *angle){
    int myServoAngle=atoi(angle);
    myservoZ.write(myServoAngle);
    Serial.print("<Z-");
    Serial.print(myServoAngle);
    Serial.print(":OK>");
  }

//Toggle LED LIGHTS
void toggleledlights(char onoroff)
{
  if(onoroff=='O'){
    digitalWrite(pinLIGHTS,LOW);
    Serial.print("<L-O:OK>");
    }
  if(onoroff=='N'){
    digitalWrite(pinLIGHTS,HIGH);
    Serial.print("<L-N:OK>");
    }
 
} 
//Toggle Siren
void togglesiren(char onoroff)
{
  if(onoroff=='O'){
    digitalWrite(pinSIREN,LOW);
    Serial.print("<N-O:OK>");
    }
  if(onoroff=='N'){
    digitalWrite(pinSIREN,HIGH);
    Serial.print("<N-N:OK>");
    }
 
} 


/*******************i2c smbus functions*********************/

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

uint8_t i2c_smbus_read_block ( uint8_t command, uint8_t* blockBuffer, uint8_t blockBufferLen )
{
    uint8_t x, num_bytes;
    i2c_start((deviceAddress<<1) + I2C_WRITE);
    i2c_write(command);
    i2c_rep_start((deviceAddress<<1) + I2C_READ);     
    num_bytes = i2c_read(false);                              //num of bytes; 1 byte will be index 0
    num_bytes = constrain(num_bytes,0,blockBufferLen-2);      //room for null at the end
    for (x=0; x<num_bytes-1; x++) {                           //-1 because x=num_bytes-1 if x<y; last byte needs to be "nack"'d, x<y-1
      blockBuffer[x] = i2c_read(false);
    }
    blockBuffer[x++] = i2c_read(true);                        //this will nack the last byte and store it in x's num_bytes-1 address.
    blockBuffer[x] = 0;                                       // and null it at last_byte+1
    i2c_stop();
    return num_bytes;
}

void i2c_smbus_manf_access ( uint8_t command, uint8_t myword)
{

    i2c_start((deviceAddress<<1) + I2C_WRITE);
    i2c_write(command);
    i2c_write(myword);
    i2c_write(0x00);
    i2c_stop();
}

void i2c_smbus_PF_Clear ()
{
/*
 * Permanent Fail Clear: (1) 0x2673 then (2) 0x1712, 
 * Instructs the bq20z80 to clear the Permanent Failure Status, 
 * clear the Permanent Failure Flag, clear the SAFE and SAFE pins, and unlock the data flash for writes. 
 * This function is only available when the bq20z80 is Unsealed
 * NOTE:
    (1) must be followed by (2). 
    If the clear fails then (1) can only be re-sent after 4 seconds since the last communication in the failed attempt. 
  If communication other than the second code occurs after the first code is sent, the clear fails.
 * 
 */
    i2c_start((deviceAddress<<1) + I2C_WRITE);
    i2c_write(0x00); //writing to manufacturer access
    i2c_write(0x73); //Little Endian 0x1234 to be sent as 0x34 +0x12
    i2c_write(0x26);
    i2c_stop();
    delay(20);
    i2c_start((deviceAddress<<1) + I2C_WRITE);
    i2c_write(0x00); //writing to manufacturer access
    i2c_write(0x12); //Little Endian 0x1234 to be sent as 0x34 +0x12
    i2c_write(0x17);
    i2c_stop();
    
}
void i2c_smbus_process_call(uint8_t command, uint8_t myword)
{
   i2c_start((deviceAddress<<1) + I2C_WRITE); 
   i2c_write(command);
   i2c_write(myword);
   i2c_write(0x00);
   i2c_rep_start(deviceAddress<<1 | I2C_READ);             //Sends a repeated start condition, i.e., it starts a new transfer without sending first a stop condition.
    byte b1 = i2c_read(false);                              //i2c_read Requests to receive a byte from the slave device. If last is true,
                                                            //then a NAK is sent after receiving the byte finishing the read transfer sequence.
    byte b2 = i2c_read(true);
    i2c_stop();
   
}
/****************************i2c smbus functions***************************/


//Call the function to get i2c batt info
int batteryinfo(char whichinfo){
    unsigned int myvoltage;
    if(whichinfo=='C'){
    Serial.print("<B-C:");
    Serial.print(fetchWord(RELATIVE_SOC));
    Serial.print(">");
    }
    if(whichinfo=='V'){
    Serial.print("<B-V:");
    myvoltage = (int)fetchWord(BATT_SMBUS_VOLTAGE);
    Serial.print(myvoltage);
    Serial.print(">");
    }
    if(whichinfo=='T'){
    unsigned int tempk = fetchWord(BATT_SMBUS_TEMP);
    Serial.print("<B-T:");
    Serial.print((float)tempk/10.0-273.15);
    Serial.print(">");
    }
    if(whichinfo=='I'){
    Serial.print("<B-I:");
    Serial.print(fetchWord(CURRENT));
    Serial.print(">");
    }
    if(whichinfo=='R'){
    Serial.print("<B-R:");
    Serial.print(fetchWord(BATT_SMBUS_REMAINING_CAPACITY));
    Serial.print(">");
    }
    if(whichinfo=='F'){
    Serial.print("<B-F:");
    Serial.print(fetchWord(BATT_SMBUS_FULL_CHARGE_CAPACITY));
    Serial.print(">");
    }
    if(whichinfo=='O'){
    i2c_smbus_manf_access(BATT_SMBUS_MANUFACTURE_ACCESS,BATT_SMBUS_MANUFACTURE_LED_OFF);
    Serial.print("<B-O:");
    Serial.print("OFF");
    Serial.print(">");
    }
    if(whichinfo=='N'){
    i2c_smbus_manf_access(BATT_SMBUS_MANUFACTURE_ACCESS,BATT_SMBUS_MANUFACTURE_LED_ON);
    Serial.print("<B-N:");
    Serial.print("ON");
    Serial.print(">");
    }
    return myvoltage;
  }

//HCSR04 distance function
void obstacle(){
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

//Analog read battery pin voltage using ADC, returns battery voltage in mVs.
int readbatteryvoltage(char isWSrequestingthis){
  //Full scale 3-18V = ADC 0-1024
  //With voltage divider 18V = Atmega VCC (5V) and 3V = Atmega 0.
  //with 680k-300K 17V = 5.2V. Will work.
  //ADC is optimized for signals with output impedance of 10k so will have to use 22K-10k voltage divider. which will give 17V = 5.3V
  
  float batteryvoltage;
  int adcreadvalue;
  float adcvoltage;
  adcreadvalue =analogRead(pinBATTERY);
  adcvoltage= (adcreadvalue/1024.0)*4.91; //Measured by multimeter vcc
  batteryvoltage=(adcvoltage+(adcvoltage*(2.2)))*1000;
  if (isWSrequestingthis=='V'){
    Serial.print("<b-V:");
    Serial.print((int)batteryvoltage);
    Serial.print(">");  
    }
  
  return (int)batteryvoltage;
  }

void adjustPWM(char analogordigital){
  if (analogordigital == 'A'){
  int mybatteryvoltage=readbatteryvoltage('A'); //readbatteryvoltage('V') results in unnecessary serial printout;
  PWMAMAX = (255.0/mybatteryvoltage)*MOTORAMAXVOLTAGE; // 'M' - could be anything other than 'V' to avoid unnecessary serial print
  PWMAMIN = (255.0/mybatteryvoltage)*MOTORAMINVOLTAGE; 
  PWMBMAX = (255.0/mybatteryvoltage)*MOTORBMAXVOLTAGE; // 'M' - could be anything other than 'V' to avoid unnecessary serial print
  PWMBMIN = (255.0/mybatteryvoltage)*MOTORBMINVOLTAGE; 
  PWMA=PWMAMIN;
  PWMB=PWMBMIN;
  
  }
  else if (analogordigital == 'D'){
   int mybatteryvoltage=batteryinfo('V');
  PWMAMAX = (255.0/mybatteryvoltage)*MOTORAMAXVOLTAGE; //Messes up smbus if everycall to batteryinfo reads the voltage by default and returns it (which is what will happen if it were M.
  PWMAMIN = (255.0/mybatteryvoltage)*MOTORAMINVOLTAGE;
  PWMBMAX = (255.0/mybatteryvoltage)*MOTORBMAXVOLTAGE; //Messes up smbus if everycall to batteryinfo reads the voltage by default and returns it (which is what will happen if it were M.
  PWMBMIN = (255.0/mybatteryvoltage)*MOTORBMINVOLTAGE;
  PWMA=PWMAMIN;
  PWMB=PWMBMIN;  
  }
  //send max-min PWMs to the client. So, when the client connects - it will send a request to adjustPWM and expect a result
  Serial.print("<H-");
  Serial.print(PWMAMAX);
  Serial.print(":OK");
  Serial.print("<h-");
  Serial.print(PWMBMAX);
  Serial.print(":OK");
  Serial.print("<L-");
  Serial.print(PWMAMIN);
  Serial.print(":OK");
  Serial.print("<l-");
  Serial.print(PWMBMIN);
  Serial.print(":OK");
  }

void motorforwardreversestop(char whichoneisit){
  if(whichoneisit=='F'){
  rotateAF();
  Serial.print("<C-F:OK>"); //'F' for motor A and 'f' for motor B
  }
  if(whichoneisit=='f'){
  rotateBF();
  Serial.print("<C-f:OK>"); //'F' for motor A and 'f' for motor B
  }
  if(whichoneisit=='R'){
  rotateAR();
  Serial.print("<C-R:OK>"); //'R' for motor A and 'r' for motor B
  }
  if(whichoneisit=='r'){
  rotateBR();
  Serial.print("<C-r:OK>"); //'R' for motor A and 'r' for motor B
  }
  if(whichoneisit=='X'){
  rotateAStop();
  Serial.print("<C-X:OK>"); //'X' for motor A and 'x' for motor B
  }
  if(whichoneisit=='x'){
  rotateBStop();
  Serial.print("<C-x:OK>"); //'X' for motor A and 'x' for motor B
  }
}
//Trying to change the format for speed and angles from S-U, S-D to S-100, S-90 etc.
void motorAspeed(char *pwmvalue){
  int myPWM=atoi(pwmvalue);
  analogWrite(pinENA,myPWM);
  Serial.print("<S-"); //'U' for motor A and 'u' for motor B
  Serial.print(myPWM);
  Serial.print(":OK>");
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
        if(receivedChars[0]=='A'){adjustPWM(receivedChars[2]);}
        if(receivedChars[0]=='B'){batteryinfo(receivedChars[2]);}
        if(receivedChars[0]=='b'){readbatteryvoltage(receivedChars[2]);}
        if(receivedChars[0]=='C'){motorforwardreversestop(receivedChars[2]);}
        if(receivedChars[0]=='S'){motorAspeed(receivedChars+2);}
        if(receivedChars[0]=='W'){moveauxservoW(receivedChars+2);}
        if(receivedChars[0]=='X'){moveauxservoX(receivedChars+2);}
        if(receivedChars[0]=='Y'){moveauxservoY(receivedChars+2);}
        if(receivedChars[0]=='Z'){moveauxservoZ(receivedChars+2);}
        if(receivedChars[0]=='L'){toggleledlights(receivedChars[2]);}
        if(receivedChars[0]=='N'){togglesiren(receivedChars[2]);}
        
        newData = false;
    }
}
void setup()
{
  //Initialize Serial
 Serial.begin (57600);
 //Initialize i2C
 if(i2c_init()){                                             //i2c_start initialized the I2C system.  will return false if bus is locked.
    Serial.print("<I2C Initialized>");
 } else
    Serial.print("<I2C Failed>");


//Attach servos to A1, A2 , A3 & D4
myservoW.attach(pinSERVOW);
myservoX.attach(pinSERVOX);
myservoY.attach(pinSERVOY);
myservoZ.attach(pinSERVOZ);

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
  

digitalWrite(pinINA1,LOW);
digitalWrite(pinINA2,LOW);
  

digitalWrite(pinINB1,LOW);
digitalWrite(pinINB2,LOW);

//Lights and Siren GPIO pins
pinMode(pinLIGHTS,OUTPUT);
pinMode(pinSIREN,OUTPUT);

//Check battery voltage and set the max and min PWM (to protect the 6 V motor. As battery gets used up and voltage drops, JS client will adjust this value
adjustPWM('A'); // A for analog, read the ADC. D for digital - get it from the smbus.

}
  
void loop()
{
  //Read the Serial and move accordingly
 recvWithStartEndMarkers();
  
 showNewData();

    //  setPosition(DIR[0],VALUE);

  
  
  
}
