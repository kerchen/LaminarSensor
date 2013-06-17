
// low_thresh determines the maximum analogRead() value that is
// considered a low value.  In the absence of a connection,
// the analog pins should be pulled high by largish (1 MOhm) resistors.
int low_thresh = 60;
int send_word = 0;

void setup() {            
  Serial.begin(9600);
  pinMode(5, OUTPUT);     
  pinMode(6, OUTPUT);     
  pinMode(7, OUTPUT);     

}

void loop() {
  send_word = 0;
  
  // Determine if each analog input has a path to
  // ground via one of the other two analog inputs
  // (which is temporarily set as an output so that
  // it is at ground potential, allowing the pin that
  // is being tested to be driven low-ish).
  pinMode( A0, INPUT );
  pinMode( A1, OUTPUT );
  pinMode( A2, OUTPUT );
  
  if ( analogRead( 0 ) < low_thresh )
  {
    send_word = send_word | 1;
    digitalWrite( 5, HIGH );
  }
  else
    digitalWrite( 5, LOW );
    

  pinMode( A0, OUTPUT );
  pinMode( A1, INPUT );
  pinMode( A2, OUTPUT );
  if ( analogRead( 1 ) < low_thresh )
  {
    send_word = send_word | 2;
    digitalWrite( 6, HIGH );
  }
  else
    digitalWrite( 6, LOW );

  pinMode( A0, OUTPUT );
  pinMode( A1, OUTPUT );
  pinMode( A2, INPUT );
  if ( analogRead( 2 ) < low_thresh )
  {
    send_word = send_word | 4;
    digitalWrite( 7, HIGH );
  }
  else
    digitalWrite( 7, LOW );
    
  Serial.write( send_word );
  
  // Receiver should use the same delay so that it doesn't stall
  // and cause all of the outputs to shut off.
  delay(100);  
}

