// This code assumes the control pins are sequentially ordered
// with the lowest pin being the first control line of the
// first controller, the second lowest pin being the second
// control line of the first controller, the third lowest pin
// being the first control line of the second controller,
// and so forth.

int  FirstControlPin = 2;
int  LastControlPin = 7;

// Time, in milliseconds, that a valve should be energized.
int  EnergizeDuration = 100;

// Data coming from wireless radio.
int  IncomingData = 0;

/// Number of milliseconds of no activity before going into demo mode
unsigned long DemoStartTime = 60000;
/// Indicates whether or not we're currently in demo mode.
int DemoMode = 0;

// The current valve state
int  ValveState[ 3 ] = { 0, 0, 0 };
// The new valve state that came in the last time there
// was data from the radio.
int  NewValveState[ 3 ] = { 0, 0, 0 };

unsigned long LastInputTime = 0;

void setup() {                
  int i;
  
  Serial.begin(9600);
  
  pinMode( 13, OUTPUT );
  digitalWrite( 13, LOW );
  
  for ( i = FirstControlPin; i <= LastControlPin; ++i )
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  
  createDemo();
}


// Looks at NewValveState and ValveState and energizes
// the appropriate valves if any states have changed.
// Returns 1 if any valve states were changed.
int setValves()
{
  int v;
  int energized(0);
  
  for ( v = 0; v < 3; ++v )
  {
    if ( NewValveState[v] != ValveState[v] )
    {
      ValveState[v] = NewValveState[v];
      if ( ValveState[v] )
        digitalWrite(2*v + FirstControlPin, HIGH);
      else
        digitalWrite(2*v + FirstControlPin + 1, HIGH); 
      energized = 1;
    }
  }
  
  // If we energized any valves, turn them all off after
  // the energize duration.
  if ( energized ) 
  {
    delay(EnergizeDuration);           
    for ( v = 0; v < 3; ++v )
    {
      digitalWrite(2*v + FirstControlPin, LOW);
      digitalWrite(2*v + FirstControlPin + 1, LOW); 
    }
  }
  
  return energized;
}


/// Main loop. Do this until the end of (Arduino) time.
void loop() 
{
  int v;
  
  // Run a simple test of all the controllers.
  // Each one should be extended and then contracted
  // for a number of times equal to the controller number.
  for ( v = 0; v < 3; ++v )
  {
    int j( v + 1 );
    
    while( j-- )
    {
      NewValveState[v] = 1;
      setValves();
      delay(500);
      NewValveState[v] = 0;
      setValves();
      delay(500);
    }
    delay(1000);
  }
  delay(1000);    

  // Read serial data and set valves.  Forever.
  while( 1 )
  {
    // The sensor will send data periodically, even if there was no change
    // in the sensors.  As long as the energize time + overhead is less
    // than the update rate of the sensor, we should be able to stay on
    // top of the incoming data.
    if ( Serial.available() > 0 )
    {
      IncomingData = Serial.read();
      
      NewValveState[0] = ( ( IncomingData & 1 ) == 1 );
      NewValveState[1] = ( ( IncomingData & 2 ) == 2 );
      NewValveState[2] = ( ( IncomingData & 4 ) == 4 );
        
      if ( setValves() )
        LastInputTime = millis();
    }
    
    // See if we should go into demo/attract mode.
    if ( millis() - LastInputTime > DemoStartTime )
    {
      digitalWrite( 13, HIGH );
      if ( ! DemoMode )
      {
        createDemo();
        DemoMode = 1;
      }
    }
    else
    {
      digitalWrite( 13, LOW );
      DemoMode = 0;
    }
    if ( DemoMode == 1 )
    {
      updateDemo();
    }
  }
}

// ms remaining for output pulses
long remainsPinA = 0;
long remainsPinB = 0;
long remainsPinC = 0;

// Number of ms water takes to reach another barrel
int periodA = 1250;
int periodB = 1250;
int periodC = 1250;

// Array of ms times, even index is "on" period, odd is "off" period
int onA[100];
int onB[100];
int onC[100];
// Number of ms remaining in current state
int remainsA = 0;
int remainsB = 0;
int remainsC = 0;
// The last index in the program
int lastA;
int lastB;
int lastC;
// The current index in the program
int indexA = -1;
int indexB = -1;
int indexC = -1;
// The state of the actuators
boolean stateA;
boolean stateB;
boolean stateC;
// Last time updateDemo() ran
long lastMillis;

// Initialize state for demo
void createDemo()
{
  // On 1/2, off 1/2
  onA[0] = periodA/2;
  onA[1] = periodA/2;
  lastA = 1;
  indexA = -1;
  // Wait for pulse from A to reach B
  onB[0] = 0;
  onB[1] = periodA;
  // On 1/4, off 1/4, twice
  onB[2] = periodB/4;
  onB[3] = periodB/4;
  onB[4] = periodB/4;
  onB[5] = periodB/4;
  lastB = 5;
  indexB = -1;
  // Wait for pulse from A to reach B
  onC[0] = 0;
  onC[1] = periodA;
  // Wait for pulse from B to reach C
  onC[2] = 0;
  onC[3] = periodB;
  // On 1/8, off 1/8, four times
  onC[4] = periodC/8;
  onC[5] = periodC/8;
  onC[6] = periodC/8;
  onC[7] = periodC/8;
  onC[8] = periodC/8;
  onC[9] = periodC/8;
  onC[10] = periodC/8;
  onC[11] = periodC/8;
  // Wait for last pulse to finish
  onC[11] = 0;
  onC[11] = periodC*7/8;
  lastC = 13;
  indexC = -1;
  
  // Init state
  stateA = false;
  stateB = false;
  stateC = false;
  remainsA = 0;
  remainsB = 0;
  remainsC = 0;
  lastMillis = millis();
}

// Update the state of demo variables
void updateDemo()
{
  // Go back to the beginning if we've finished
  if (
    indexA == lastA && indexB == lastB && indexC == lastC
    && remainsA == 0 && remainsB == 0 && remainsC == 0
  ) {
    createDemo();
  }
  
  // Find out how much time has ellapsed since the last draw()
  long curMillis = millis();
  long elapsed = curMillis - lastMillis;
  lastMillis = curMillis;

  updateActuators(elapsed);

  // Update A state
  long toAdvance = elapsed;
  while (toAdvance > 0) {
    // Check whether we've finished the current state
    if (remainsA <= toAdvance) {
      // Subtract time left on current state, and move to next state
      toAdvance -= remainsA;
      remainsA = 0;
      if (indexA < lastA) {
        indexA++;
        remainsA = onA[indexA];
        stateA = !stateA;
        digitalWrite(13, stateA ? HIGH : LOW);
        setActuatorA(stateA);
      } else {
        break;
      }
    } else {
      // Still time remaining, just subtact it
      remainsA -= toAdvance;
      toAdvance = 0;
    }
  }
  
  // Update B state
  toAdvance = elapsed;
  while (toAdvance > 0) {
    // Check whether we've finished the current state
    if (remainsB <= toAdvance) {
      // Subtract time left on current state, and move to next state
      toAdvance -= remainsB;
      remainsB = 0;
      if (indexB < lastB) {
        indexB++;
        remainsB = onB[indexB];
        stateB = !stateB;
        setActuatorB(stateB);
      } else {
        break;
      }
    } else {
      // Still time remaining, just subtact it
      remainsB -= toAdvance;
      toAdvance = 0;
    }
  }
  
  // Update C state
  toAdvance = elapsed;
  while (toAdvance > 0) {
    // Check whether we've finished the current state
    if (remainsC <= toAdvance) {
      // Subtract time left on current state, and move to next state
      toAdvance -= remainsC;
      remainsC = 0;
      if (indexC < lastC) {
        indexC++;
        remainsC = onC[indexC];
        stateC = !stateC;
        setActuatorC(stateC);
      } else {
        break;
      }
    } else {
      // Still time remaining, just subtact it
      remainsC -= toAdvance;
      toAdvance = 0;
    }
  }
}

void setActuatorA(boolean state) {
  if (state) {
    digitalWrite(FirstControlPin, HIGH);
    digitalWrite(FirstControlPin+1, LOW);
    remainsPinA = EnergizeDuration;
  } else {
    digitalWrite(FirstControlPin, LOW);
    digitalWrite(FirstControlPin+1, HIGH);
    remainsPinA = EnergizeDuration;
  }
}

void setActuatorB(boolean state) {
  if (state) {
    digitalWrite(FirstControlPin+2, HIGH);
    digitalWrite(FirstControlPin+3, LOW);
    remainsPinB = EnergizeDuration;
  } else {
    digitalWrite(FirstControlPin+2, LOW);
    digitalWrite(FirstControlPin+3, HIGH);
    remainsPinB = EnergizeDuration;
  }
}

void setActuatorC(boolean state) {
  if (state) {
    digitalWrite(FirstControlPin+4, HIGH);
    digitalWrite(FirstControlPin+5, LOW);
    remainsPinC = EnergizeDuration;
  } else {
    digitalWrite(FirstControlPin+4, LOW);
    digitalWrite(FirstControlPin+5, HIGH);
    remainsPinC = EnergizeDuration;
  }
}

void updateActuators(long elapsed) {
  if (remainsPinA < elapsed) {
    digitalWrite(FirstControlPin, LOW);
    digitalWrite(FirstControlPin+1, LOW);
  }
  if (remainsPinB < elapsed) {
    digitalWrite(FirstControlPin+2, LOW);
    digitalWrite(FirstControlPin+3, LOW);
  }
  if (remainsPinC < elapsed) {
    digitalWrite(FirstControlPin+4, LOW);
    digitalWrite(FirstControlPin+5, LOW);
  }
  remainsPinA = max(0, remainsPinA - elapsed);
  remainsPinB = max(0, remainsPinB - elapsed);
  remainsPinC = max(0, remainsPinC - elapsed);
}


