int led = 13;
int thresh = 60;
int send_word = 0;

// the setup routine runs once when you press reset:
void setup() {            
  Serial.begin(9600);
  pinMode(5, OUTPUT);     
  pinMode(6, OUTPUT);     
  pinMode(7, OUTPUT);     

}

// the loop routine runs over and over again forever:
void loop() {
  send_word = 0;
  
  pinMode( A0, INPUT );
  pinMode( A1, OUTPUT );
  pinMode( A2, OUTPUT );
  if ( analogRead( 0 ) < thresh )
  {
    send_word = send_word | 1;
    digitalWrite( 5, HIGH );
  }
  else
    digitalWrite( 5, LOW );
    

  pinMode( A0, OUTPUT );
  pinMode( A1, INPUT );
  pinMode( A2, OUTPUT );
  if ( analogRead( 1 ) < thresh )
  {
    send_word = send_word | 2;
    digitalWrite( 6, HIGH );
  }
  else
    digitalWrite( 6, LOW );

  pinMode( A0, OUTPUT );
  pinMode( A1, OUTPUT );
  pinMode( A2, INPUT );
  if ( analogRead( 2 ) < thresh )
  {
    send_word = send_word | 4;
    digitalWrite( 7, HIGH );
  }
  else
    digitalWrite( 7, LOW );
    
  Serial.write( send_word );
  delay(100);
}

