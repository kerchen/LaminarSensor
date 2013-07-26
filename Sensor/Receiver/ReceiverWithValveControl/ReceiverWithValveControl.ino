int  FirstControlPin = 2;
int  LastControlPin = 7;
int  EnergizeDuration = 100;
int  IncomingData = 0;

int  ValveState[ 3 ] = { 0, 0, 0 };
int  NewValveState[ 3 ] = { 0, 0, 0 };

void setup() {                
  int i;
  
  Serial.begin(9600);
  
  for ( i = FirstControlPin; i <= LastControlPin; ++i )
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
}

void setValves()
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
}


void loop() {
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

  while( 1 )
  {
    if ( Serial.available() > 0 )
    {
      IncomingData = Serial.read();
      
      NewValveState[0] = ( ( IncomingData & 1 ) == 1 );
      NewValveState[1] = ( ( IncomingData & 2 ) == 2 );
      NewValveState[2] = ( ( IncomingData & 4 ) == 4 );
        
      setValves();
    }  
  }
}
