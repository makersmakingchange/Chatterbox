// This file is to test an issues identified where calling the play function multiple times makes no file play.

#include <TMRpcm.h>
#include <SD.h>
#include <SPI.h>
#include <StateMachine.h>

#define DEBUG

TMRpcm audio;
File myFile;

// Add state maching information
StateMachine machine = StateMachine();


//Additional Arduino connections
const int mic {A0};
const int rec_LED {A1};
const int play_LED {A2};
const int select_switch_jack {6}; // switch jack for starting scanning and selecting options
const int rec_button {5};
const int play_button {4};
const int level_button {3};
//const int power_switch {10};
const int speaker_shutdown {8};
const int level_3 {A5};
const int level_2 {A4};
const int level_1 {A3};
const int level_ID {A6}; // analog input pin to read which level is selected
const int speed_ID {A7}; // analog input pin to read which playback speed is selected
//const int message_1
//const int message_2
//const int message_3
//const int message_4
const int switch_scan_button {7};
const long interval = 3000;
const long blinkInterval = 500;

//File properties
int file_number =0;
int file_level {1};
int old_level = {1};
int current_message {1};
const int sample_rate {16000};
const String file_extension {".wav"}; 
int old_file_level {1};
const int message_total = 4; // Total number of messages to be stored on any one level
char file_1 [50] = "rec_1_1.wav";
char file_2 [50] = "rec_1_2.wav";
char file_3 [50] = "rec_1_3.wav";
char file_4 [50] = "rec_1_4.wav";
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
int LEDState = LOW;


// LED blinking function

void blink_LED(){
  currentMillis = millis();

  if(currentMillis - previousMillis >= blinkInterval){
    previousMillis = currentMillis;

    if(LEDState == LOW){
      LEDState = HIGH;
    }
    else{
      LEDState = LOW;
    }
    digitalWrite(play_LED, LEDState);
  }
}

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);

  #ifdef DEBUG
  while (!Serial) {
    // wait for serial port to connect
  }
  #endif
  //Define pins
  pinMode(rec_LED, OUTPUT);
  pinMode(play_LED, OUTPUT);
  pinMode(level_1, OUTPUT);
  pinMode(level_2, OUTPUT);
  pinMode(level_3, OUTPUT);
  pinMode(rec_button, INPUT_PULLUP);
  pinMode(play_button, INPUT_PULLUP);
  pinMode(level_button, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(level_button),level_int,FALLING);
  //pinMode(power_switch, INPUT_PULLUP);
  pinMode(speaker_shutdown, OUTPUT);
  pinMode(select_switch_jack,INPUT_PULLUP);
  //audio.CSPin = 10;
  audio.speakerPin = 9;
  audio.volume(5);
  audio.quality(1);

  //Ensure access to Micro SD Card

  #ifdef DEBUG
  Serial.println("Initializing SD card...");
  #endif
  
  if (!SD.begin(10)) {
    #ifdef DEBUG
    Serial.println("Initialization failed!");
    #endif
    
    while (1){
      digitalWrite(rec_LED, HIGH);
      digitalWrite(play_LED, HIGH);
      delay(500);
      digitalWrite(rec_LED, LOW);
      digitalWrite(play_LED, LOW);
      delay(500);
    }
    
  }

}

void loop() {
  // put your main code here, to run repeatedly:

  if(digitalRead(play_button) == LOW){
  digitalWrite(speaker_shutdown, HIGH);
  audio.play(file_1);

  while(audio.isPlaying()){
    blink_LED();
    if(digitalRead(level_button) == LOW){
      audio.disable();
    }
  //previousMillis = millis();
  }

  digitalWrite(speaker_shutdown, LOW);
  digitalWrite(play_LED, LOW);
  #ifdef DEBUG
  Serial.println(file_1);
  #endif
 // The current millis stuff doesn't work for timing. I think it needs to be outside the if statement for when the button is pressed.
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
    digitalWrite(speaker_shutdown, HIGH);
    audio.play(file_2);
    while(audio.isPlaying()){
      blink_LED();
    }
    digitalWrite(speaker_shutdown, LOW);
  }
  }

  
    if(digitalRead(level_button) == LOW){
    digitalWrite(play_LED, HIGH);
  digitalWrite(speaker_shutdown, HIGH);
  audio.play(file_2);

  while(audio.isPlaying()){
    blink_LED();
  }
  digitalWrite(speaker_shutdown, LOW);
  digitalWrite(play_LED, LOW);
  #ifdef DEBUG
  Serial.println(file_2);
  #endif
  }

}
