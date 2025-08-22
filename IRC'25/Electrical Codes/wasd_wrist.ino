// Pin Definitions
#define encA 36   // Hall sensor A
#define encB 39   // Hall sensor B
#define PWM1 32  // Motor2 PWM output
#define dir1 33  // Motor2 direction control
#define PWM2 25  // Motor1 PWM output
#define dir2 26  // Motor1 direction control

volatile long posi = 0;           // Current position (encoder pulses)
int maxPwm = 0;                 // Max PWM value for the motor
int mdir1 = 0;
int mdir2 = 0;
char input = 'x';

void setup() {
  Serial.begin(115200);           // Use a faster baud rate for communication
  pinMode(encA, INPUT_PULLUP);
  pinMode(encB, INPUT_PULLUP);
  pinMode(PWM1, OUTPUT);
  pinMode(dir1, OUTPUT);
  pinMode(PWM2, OUTPUT);
  pinMode(dir2, OUTPUT);

  // Attach interrupt to encoder channel A
  attachInterrupt(digitalPinToInterrupt(encA), readEncoder, RISING);
}

void loop() {
  // Read Serial input
  if (Serial.available()) {
    input = Serial.read();

    if (input == 'x') {
      // Stop the motor immediately
      maxPwm = 0;
      motorControl(0, 0, 0);
      Serial.println("Motor stopped.");
      return; // Skip further processing in this iteration
    }
    else {
    // Update motor direction based on other inputs
    switch (input) {
      case 'w':
        mdir1 = 0;
        mdir2 = 1;
        maxPwm = 100;
        break;
      case 'a':
        mdir1 = 1;
        mdir2 = 1;
        maxPwm = 120;
        break;
      case 's':
        mdir1 = 1;
        mdir2 = 0;
        maxPwm = 100;
        break;
      case 'd':
        mdir1 = 0;
        mdir2 = 0;
        maxPwm = 120;
        break;
      case 'x':
        mdir1 = 0;
        mdir2 = 0;
        maxPwm = 0;
        break;
      }
    }
  }

  // Control motor based on inputs
  motorControl(mdir1, mdir2, maxPwm);

  // Optional: Print status
  Serial.print("Input: ");
  Serial.print(input);
  Serial.print("   |  ");
  Serial.print("Position: ");
  Serial.println(posi);
  delay(50);  // Add a small delay to reduce Serial communication frequency
}

// Function to control motor direction and speed
void motorControl(int mdir1, int mdir2, int pwmVal) {
  analogWrite(PWM1, pwmVal);
  digitalWrite(dir1, mdir1);
  analogWrite(PWM2, pwmVal);
  digitalWrite(dir2, mdir2);
}

// Interrupt Service Routine (ISR) for encoder
void readEncoder() {
  int stateB = digitalRead(encB);
  posi += (stateB == HIGH) ? 1 : -1;  // Increment or decrement based on encoder direction
}
