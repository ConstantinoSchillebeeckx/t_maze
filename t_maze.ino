// Example of state machine
// Author: Nick Gammon
// Date: 5 December 2013

#include <Servo.h>

Servo elevator; 

/*
 * Move elevator with command R45 (for position 45)
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
              } states;

const byte solenoidPin = 8;
int timeOpen = 500; // time (in ms) solenoid stays open to allow air flow

// current state-machine state
states state = STATE_NONE;

void setup ()
  { 
  Serial.begin (115200);
  Serial.println (F("Starting ..."));
  elevator.attach(9);
  
  pinMode(solenoidPin, OUTPUT);
  digitalWrite(solenoidPin, LOW);
  
  } // end of setup
  
void doElevator (const long pos)
  {
  // rotate to desired angle 
  Serial.print (F("Moving elevator to "));
  Serial.println (pos); 
  elevator.write(pos);
  }  

void doPuff (const long pos)
  {
  // rotate to desired angle 
  Serial.print (F("Puffing at "));
  Serial.println (pos);

  digitalWrite(solenoidPin, HIGH);
  delay(timeOpen);
  digitalWrite(solenoidPin, LOW);
  delay(pos);
  digitalWrite(solenoidPin, HIGH);
  delay(timeOpen);
  digitalWrite(solenoidPin, LOW);
  delay(pos);
  digitalWrite(solenoidPin, HIGH);
  delay(timeOpen);
  digitalWrite(solenoidPin, LOW);
   
//  elevator.write(pos);
  }  
  
  
void processNumber (char command, long n)
  {
  switch (command)
    {
    case 'E' : 
        doElevator (n); 
        break;
    case 'P' : 
        doPuff (n); 
        break;
    }  // end of switch
  state = STATE_NONE;
  }  // end of processNumber

  
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

  Serial.println(c);

  // process according to what the input was  
  switch (toupper (c))
    {
   
    case '\n':
    case '\r':
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
    
  
  } // end of loop
