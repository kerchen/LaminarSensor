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
    }
    else
    {
      digitalWrite( 13, LOW );
    }
  }
}
