void setup() {
  Serial.begin(9600);
  pinMode(5, OUTPUT);     
  pinMode(6, OUTPUT);     
  pinMode(7, OUTPUT);     

}

int incomingData = 0;

void loop() {
  if ( Serial.available() > 0 )
  {
    incomingData = Serial.read();
    
      if ( ( incomingData & 1 ) == 1 )
        digitalWrite( 5, HIGH );
      else
        digitalWrite( 5, LOW );
      
       if ( ( incomingData & 2 ) == 2 )
        digitalWrite( 6, HIGH );
      else
        digitalWrite( 6, LOW );
        
      if ( ( incomingData & 4 ) == 4 )
        digitalWrite( 7, HIGH );
      else
        digitalWrite( 7, LOW );
      delay(100);  // Should match delay in sensor sketch
  }
  else
  {
    digitalWrite( 5, LOW );
    digitalWrite( 6, LOW );
    digitalWrite( 7, LOW );
  }
}
