//Reads potentiometer output from analog stick on defined pins
//Sends the data over serial line in a custom format.

//pinouts
int x_pin = A1;
int y_pin = A2;
int button_pin = 2;

int INTERVAL = 100;

char buffer[3];

int x;
int y;

void setup() {
  Serial.begin(19200); 
  pinMode(x_pin, INPUT);
  pinMode(y_pin, INPUT);

  // interrupt for pressing a button
  pinMode(button_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(button_pin), button_press, HIGH);
}
 
void loop() {
  x = analogRead(x_pin);
  y = analogRead(y_pin);
  
  sprintf (buffer, "<%i,%i>", x, y);
  Serial.println(buffer);  
  delay(INTERVAL);
}

void button_press(){
  // arbitrary code to be recognized by receiver
  Serial.println("+");
  delay(100);
  loop();
}
