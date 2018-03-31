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
 * Pxxx - pulse the air solenoid
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
               STATE_P, 
               STATE_W,
              } states;

const byte solenoidPin = 8;
const byte elevatorPin = 9;

// time (in ms) solenoid stays open to allow air flow
int timeOpen = 500; 

// current state-machine state
states state = STATE_NONE;


void setup ()
  { 
  Serial.begin (115200);
  Serial.println (F("Ready to receive command"));
  Serial.println (F("e.g. E50;P100;E200"));
  elevator.attach(elevatorPin);
  
  pinMode(solenoidPin, OUTPUT);
  digitalWrite(solenoidPin, LOW);
  
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
   Serial.println(" ms ...");
   delay(ms);
}


void doPuff (const long timeClosed, int pin)
  {
  /*
   * Modulate solenoid state found on
   * 'pin'; time open equal to 'timeOpen',
   * time close equal to 'timeClosed'
   */
  Serial.print (F("Puffing solenoid at "));
  Serial.print (timeClosed);
  Serial.println (F("ms"));

  // do an open-close-open twice
  digitalWrite(pin, HIGH);
  delay(timeOpen);
  digitalWrite(pin, LOW);
  delay(timeClosed);
  digitalWrite(pin, HIGH);
  delay(timeOpen);
  digitalWrite(pin, LOW);
  delay(timeClosed);
  digitalWrite(pin, HIGH);
  delay(timeOpen);
  digitalWrite(pin, LOW);
  }  
  
  
void processNumber (char command, long n)
  {
  switch (command)
    {
    case 'E' : 
        doElevator (n); 
        break;
    case 'P' : 
        doPuff (n, solenoidPin); 
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

  // process according to what the input was  
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

    case 'P':
      commandType = 'P';
      receivedNumber = 0;
      state = STATE_P;
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
