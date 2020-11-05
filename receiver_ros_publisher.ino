//#include <ArduinoHardware.h>
//#include <ros.h>
//#include <geometry_msgs/Twist.h>
//
//ros::NodeHandle nh;
//
//geometry_msgs::Twist msg;
//ros::Publisher pub("husky/cmd_vel", &msg);

const byte numChars = 8;
char receivedChars[numChars]; 
boolean newData = false;

int calib_x = 0;
int calib_y = 0;
bool calibrated = false;

int x;
int y;

void setup() {
    Serial.begin(19200);
//    nh.initNode();
//    nh.advertise(pub);
    delay(50);
}

void loop() {
    recv();
    publish_cmd_vel();
}

void recv() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
 
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (rc == '+'){
          button_press(x, y);
        }

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void publish_cmd_vel() {
    if (newData == true) {
      x = atoi(receivedChars);
      int offset = count_digits(x) + 1;
      y = atoi(receivedChars + offset);

      //calibrate if not done already
      if (calibrated == false){
        Serial.println("Calibrating, ensure the analog stick is in neutral position.");
        delay(1000);
        calib_x = x;
        calib_y = y;
        calibrated = true;
      }
      
      //scale values for velocity commands
      double lin_vel = (x - calib_x) / 200.0;
      double ang_vel = (y - calib_y) / 200.0;

      // deadzones, readings can be somewhat noisy
      if (lin_vel < 0.1 && lin_vel > -0.1){
        lin_vel = 0;
      }
      if (ang_vel < 0.1 && ang_vel > -0.1){
        ang_vel = 0;
      }
      
      // monitor output over serial
      Serial.print("Linear velocity: ");
      Serial.print(lin_vel);
      Serial.print("   Angular velocity: ");
      Serial.println(ang_vel);
      
      //compose and publish Twist msg
//      msg.linear.x=lin_vel;
//      msg.linear.y=ang_vel;
//
//      pub.publish(&msg);
//      nh.spinOnce();

      newData = false;
    }
}

// function called when button press is received
// planned to be utilized for a single sweep of the scanner
// currently just recalibrates due to no rover access
void button_press(int x, int y){
        Serial.println("Calibrating in 1 second, ensure the analog stick is in neutral position.");
        delay(1000);
        calib_x = x;
        calib_y = y;
        calibrated = true;
        loop();
}

int count_digits(int num){
    int count = 0;
      do
    {
        // Increment digit count
        count++;
        // Remove last digit
        num /= 10;
    } 
      while(num != 0);

    return count;
}
