#include <Bluepad32.h>
#include "esp_system.h"

// Motor A 
const int IN1 = 13;
const int IN2 = 26; 
const int ENA = 32; 

// Motor B 
const int IN3 = 14;
const int IN4 = 27;
const int ENB = 33; 

const int DEADZONE = 15;
const int MIN_PWM = 60;  // Lowered slightly to reduce heat if stalled at low speeds
const int MAX_PWM = 120; // Lowered from 180 to drastically reduce L298N overheating

unsigned long lastCommandTime = 0;
const unsigned long COMMAND_TIMEOUT = 300; // Stop motors if no signal for 300ms

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void setup() {
  Serial.begin(115200);
  
  Serial.println("\n=======================================================");
  Serial.printf("ESP32 BOOTED. Reset reason: %d\n", esp_reset_reason());
  Serial.println("=======================================================\n");

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
  
  stopMotors();

  BP32.setup(&onConnectedController, &onDisconnectedController);
  
  Serial.println("System ready. Waiting for PS5 Controller...");
}

void loop() {
  BP32.update();
  processController();

  // Safety failsafe: If connection is lost or frozen for 300ms, halt.
  if (millis() - lastCommandTime > COMMAND_TIMEOUT) {
    stopMotors();
  }

  delay(5);
}

void onConnectedController(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.printf("CALLBACK: Controller connected, index=%d\n", i);
      myControllers[i] = ctl;
      break;
    }
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      Serial.printf("CALLBACK: Controller disconnected, index=%d\n", i);
      myControllers[i] = nullptr;
      stopMotors();
      break;
    }
  }
}

void setMotor(int in1, int in2, int en, int value) {
  value = constrain(value, -512, 512);

  if (abs(value) <= DEADZONE) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(en, 0);
    return;
  }

  // Map input to a guaranteed minimum starting torque
  int pwm = map(abs(value), DEADZONE, 512, MIN_PWM, MAX_PWM);
  pwm = constrain(pwm, MIN_PWM, MAX_PWM);

  if (value > 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }

  analogWrite(en, pwm);
}

void processController() {
  ControllerPtr ctl = myControllers[0];
  
  if (!ctl || !ctl->isConnected()) {
    return; 
  }

  int leftStick = constrain(ctl->axisY(), -512, 512);
  int rightStick = constrain(ctl->axisRY(), -512, 512);

  lastCommandTime = millis();

  // SWAPPED: Right joystick now controls Motor A (ENA), Left joystick controls Motor B (ENB)
  setMotor(IN1, IN2, ENA, rightStick);
  setMotor(IN3, IN4, ENB, leftStick);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
  
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 0);
}