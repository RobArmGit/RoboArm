#define pot3 4
#define dir3 16
#define p3 17

#define pot2 35
#define dir2 22
#define p2 23

#define pot1 34
#define dir1 14
#define p1 27

// Pin Definitions
#define encA 2   // Hall sensor A (connected to interrupt-capable pin)
#define encB 3   // Hall sensor B
#define PWM  8   // Motor PWM output
#define dir1 9   // Motor direction control

volatile long posi = 0;           // Current position (encoder pulses)
long target = 0;                  // Target position (calculated in pulses)
const int pulsesPerRev = 20000;   // Pulses per revolution for the motor encoder
int maxPwm = 255;                 // Max PWM value for the motor (Arduino Mega supports 0-255)
bool stopMotor = false; 

const int freq = 20000;
const int resolution = 8;
const int pwm = 200;

int angle1;
int angle2;
int angle3;

float w2 = 0;
float b2; 
int a2 = 0;

float w3 = 0;
float b3; 
int a3 = 0;

float w1 = 0;
float b1; 
int a1 = 0;

float b4;

int q1,q2,q3;
int c1,c2,c3;
int b11,b22,b33;

String inputString;

int values[4];
float predefinedAngles[][3] = {
  // limits
  // pot1 -> 55 deg : 216 to 05 deg : 290
  // pot2 -> 00 deg : 300 to 90 deg : 220
  // pot3 -> 90 deg : 240 to 00 deg : 155
  {216, 220, 155, -45}, // Predefined angle set 1
  {230, 250, 180, 30}, // Predefined angle set 2
  {290, 300, 240, 0}  // Predefined angle set 3
};

void setup() {
  pinMode(pot3, INPUT);
  pinMode(dir3, OUTPUT);
  pinMode(p3, OUTPUT);
  ledcAttachChannel(p3, freq, resolution, 1);

  pinMode(pot2, INPUT);
  pinMode(dir2, OUTPUT); 
  pinMode(p2, OUTPUT);
  ledcAttachChannel(p2, freq, resolution, 2);

  pinMode(pot1, INPUT);
  pinMode(dir1, OUTPUT);
  pinMode(p1, OUTPUT);
  ledcAttachChannel(p1, freq, resolution, 3);

  Serial.begin(115200);           // Use a faster baud rate for communication
  pinMode(encA, INPUT_PULLUP);
  pinMode(encB, INPUT_PULLUP);
  pinMode(PWM, OUTPUT);
  pinMode(dir1, OUTPUT);

  // Attach interrupt to encoder channel A
  attachInterrupt(digitalPinToInterrupt(encA), readEncoder, RISING);

  Serial.begin(115200);
}

void moveJoints(float b1, float b2, float b3, float b4) {
  int c1 = analogRead(pot1);
  int c2 = analogRead(pot2);
  int c3 = analogRead(pot3);

  int q1 = map(c1, 0, 4095, 0, 300);
  int q2 = map(c2, 0, 4095, 0, 300);
  int q3 = map(c3, 0, 4095, 0, 300);

  digitalWrite(dir1, b1 > q1 ? HIGH : LOW);
  digitalWrite(dir2, b2 > q2 ? HIGH : LOW);
  digitalWrite(dir3, b3 > q3 ? HIGH : LOW);

  while (abs(q1 - b1) > 2 || abs(q2 - b2) > 2 || abs(q3 - b3) > 2) {
    if (Serial.available()>0){
      break;
    }
    if (abs(q1 - b1) > 2) {
      ledcWrite(p1, pwm);
    } else {
      ledcWrite(p1, 0);
    }

    if (abs(q2 - b2) > 2) {
      ledcWrite(p2, pwm);
    } else {
      ledcWrite(p2, 0);
    }

    if (abs(q3 - b3) > 2) {
      ledcWrite(p3, pwm);
    } else {
      ledcWrite(p3, 0);
    }

    c1 = analogRead(pot1);
    c2 = analogRead(pot2);
    c3 = analogRead(pot3);

    q1 = map(c1, 0, 4095, 0, 300);
    q2 = map(c2, 0, 4095, 0, 300);
    q3 = map(c3, 0, 4095, 0, 300);
  }

  ledcWrite(p1, 0);
  ledcWrite(p2, 0);
  ledcWrite(p3, 0);
}

void loop() {
  if (Serial.available() > 0) {
    char inputChar = Serial.read();

    if (inputChar == 'x') {
      ledcWrite(p1, 0);
      ledcWrite(p2, 0);
      ledcWrite(p3, 0);
      Serial.println("Stopping motion");
    }else if (inputChar >= '1' && inputChar <= '3') {
      int index = inputChar - '1';
      
      b1 = predefinedAngles[index][0];
      b2 = predefinedAngles[index][1];
      b3 = predefinedAngles[index][2];
      
      angle = predefinedAngles[index][3];
      float angle = input.toFloat();  // Parse the number of revolutions (supports fractional values)
      b4 = (long)(revolutions * pulsesPerRev / 360);

      long error = target - posi;
      int pwmVal = constrain(abs(error), 0, maxPwm); // Constrain PWM value to the allowed range

      // Determine motor direction
      int dir = (error < 0) ? -1 : 1;

      if (b1 <= 290 && b1 >= 216){
        if (b2 <= 300 && b2 >= 220){
          if (b3 <= 240 && b3 >= 155) moveJoints(b1, b2, b3);
          else Serial.println("unreachable array");
        }
      }

      Serial.print("Moved to predefined angle set ");
      Serial.println(inputChar);
    }
  }
}
