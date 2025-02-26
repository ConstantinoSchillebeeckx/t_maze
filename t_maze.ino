// Adapted from State Machine example at https://www.gammon.com.au/serial

#include <Servo.h>

Servo elevator; 

/*
 * Usage:
 * Use the serial monitor (Tools -> Serial monitor) to
 * send commands to the T-maze; make sure the monitor
 * is set to 115200 baud and that the line ending is
 * set to "Newline". Commands are entered in the
 * monitor and sent by pressing Return/Enter.
 * 
 * The following commands are available:
 * Exxx - move the elevator to position xxx
 * Zxxx - pulse the air solenoid attached to pin Z xxx times
 * Yxxx - pulse the air solenoid attached to pin Y xxx times
 * Cxxx - pulse the air solenoid attached to pin C xxx times
 * Axxx - open the solenoid on pin A for a duration of xxx ms
 * Bxxx - open the solenoid on pin B for a duration of xxx ms
 * Xxxx - open the solenoid on pin X for a duration of xxx ms
 * 
 * Commands can be chained together by delimiting
 * them with a semi-colon (;) e.g. E50;P100;E200
 */

/* 
 * Elevator limits:
 * min - 44
 * max - 143 
 * 
 * Elevator positions:
 * flow through end - 106
 * elevator testing - 140
 * elevator consolidation - 100
 * closed consolidation - 70
 * elevator loading - 66
 */


// the possible states of the state-machine
typedef enum { STATE_NONE, 
               STATE_E, 
               STATE_A,STATE_B,STATE_C,
               STATE_X,STATE_Y,STATE_Z, 
               STATE_W,
              } states;

// solenoid pins
const byte solenoidA = 2;
const byte solenoidB = 4;
const byte solenoidC = 5;
const byte solenoidX = 6;
const byte solenoidY = 7;
const byte solenoidZ = 8;

const byte elevatorPin = 3;

// time (in ms) solenoid stays open/closed to allow air flow
// solenoids are normally closed (NC)
int timeOpen = 20; // 10
int timeClosed = 200; // 300

// current state-machine state
states state = STATE_NONE;


void setup ()
  { 
  Serial.begin (115200);
  Serial.println (F("Ready to receive command"));
  Serial.println (F("e.g. E50;A100;E200"));
  elevator.attach(elevatorPin);
  
  pinMode(solenoidA, OUTPUT);
  digitalWrite(solenoidA, LOW);
  pinMode(solenoidB, OUTPUT);
  digitalWrite(solenoidB, LOW);
  pinMode(solenoidC, OUTPUT);
  digitalWrite(solenoidC, LOW);
  pinMode(solenoidX, OUTPUT);
  digitalWrite(solenoidX, LOW);
  pinMode(solenoidY, OUTPUT);
  digitalWrite(solenoidY, LOW);
  pinMode(solenoidZ, OUTPUT);
  digitalWrite(solenoidZ, LOW);
  
  } // end of setup

  
void doElevator (const long pos)
  {
  /*  
   * Move elevator (servo) to desired
   * position 'pos'.
   */
  // TODO: delay some time (?) until we reach end pos
  // rotate to desired angle 
  Serial.print (F("Moving elevator to "));
  Serial.println (pos); 
  elevator.write(pos);
  }  

void doWait (const long ms) {
  /*
   * Time (in ms) to wait or pause program.
   * This allows for things like waiting
   * until the elevator reaches a position
   * or for conditioning time
   */
   Serial.print("Waiting ");
   Serial.print(ms);
   Serial.print(" ms ... ");
   delay(ms);
   Serial.println("done!");
}

void openSolenoid(long openTime, int pin) {
  /*
   * Open the solenoid on given pin for
   * duration of `openTime
   */

  Serial.print("Opening solenoid (pin ");
  Serial.print(pin);
  Serial.print(") ");
  Serial.print(openTime);
  Serial.println(" ms.");

  // actuate solenoid
  digitalWrite(pin, HIGH);
  delay(openTime);
  digitalWrite(pin, LOW);
}


void doPuff (long numActuate, int pin) {
  /*
   * Modulate solenoid state found on
   * 'pin'; numActuate number of times
   */
  Serial.print (F("Puffing solenoid (pin "));
  Serial.print(pin);
  Serial.print(") ");
  Serial.print(numActuate);
  Serial.println(" times.");

  for (int i = 0; i < numActuate; i++) {
    digitalWrite(pin, HIGH);
    delay(timeOpen);
    digitalWrite(pin, LOW);
    delay(timeClosed);
  }
}  
  
  
void processNumber (char command, long n)
  {
  switch (command)
    {
    case 'E' : 
        doElevator (n); 
        break;
    case 'A' : 
        openSolenoid (n, solenoidA); 
        break;
    case 'B' : 
        openSolenoid (n, solenoidB); 
        break;
    case 'C' : 
        doPuff (n, solenoidC); 
        break;
    case 'X' : 
        openSolenoid (n, solenoidX); 
        break;
    case 'Y' : 
        doPuff (n, solenoidY); 
        break;
    case 'Z' : 
        doPuff (n, solenoidZ); 
        break;
    case 'W':
        doWait (n);
        break;
    }
  state = STATE_NONE;
  }

  
void unexpectedInput (char c)
  {
  Serial.print (F("Unexpected input: '"));
  Serial.print (char (c));
  Serial.println (F("'"));
  }  // end of unexpectedInput


// current number    
long receivedNumber = 0;

// what command we received
char commandType;
 

  
void gotNewline ()
  {
  switch (state)
    {
    case STATE_NONE: 
         break;  // ignore newline/carriage-return
          
    default:
         processNumber (commandType, receivedNumber);
         state = STATE_NONE;
         break; 

    }  // end of switch on state

  }  // end of gotNewline
  
  
void gotDigit (const char digit)
  {
  receivedNumber *= 10;
  receivedNumber += digit - '0';
  }  // end of gotDigit
  
 
  
void processInput ()
  {
  
  byte c = Serial.read ();

  // process according to what input was received
  switch (toupper (c))
    {
   
    case '\n':
    case '\r':
    case ';':
      gotNewline ();
      break;
      
    case 'E':
      commandType = 'E';
      receivedNumber = 0;
      state = STATE_E;
      break;

    case 'A':
      commandType = 'A';
      receivedNumber = 0;
      state = STATE_A;
      break;
    case 'B':
      commandType = 'B';
      receivedNumber = 0;
      state = STATE_B;
      break;
    case 'C':
      commandType = 'C';
      receivedNumber = 0;
      state = STATE_C;
      break;
    case 'X':
      commandType = 'X';
      receivedNumber = 0;
      state = STATE_X;
      break;
    case 'Y':
      commandType = 'Y';
      receivedNumber = 0;
      state = STATE_Y;
      break;
    case 'Z':
      commandType = 'Z';
      receivedNumber = 0;
      state = STATE_Z;
      break;
      
    case 'W':
      commandType = 'W';
      receivedNumber = 0;
      state = STATE_W;
      break;
      
    case '0' ... '9': 
      gotDigit (c);
      break;
      
    default:
      unexpectedInput (c);
      break;
    } // end of switch on event (input) type
  }  // end of processInput

  
void loop ()
  {
  
  if (Serial.available ())
    processInput ();
  }
