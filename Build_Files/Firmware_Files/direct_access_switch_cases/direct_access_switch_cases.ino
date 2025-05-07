// This file is to test an issues identified where calling the play function multiple times makes no file play.

#include <TMRpcm.h>
#include <SD.h>
#include <SPI.h>
#include <StateMachine.h>
#include <neotimer.h>

#define DEBUG

TMRpcm audio;
File myFile;

// Add state maching information
StateMachine play_machine = StateMachine();

State* S0 = play_machine.addState(&waiting);
State* S1 = play_machine.addState(&play_message);
// State* S2 = play_machine.addState(&scan_delay);
Neotimer mytimer; // Set one second timer, length of time can change later

//Additional Arduino connections
const int mic {A0};
const int rec_LED {A1};
const int play_LED {A2};
const int message_button_3 {5};
const int message_button_2 {4};
const int message_button_1 {3};
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
const int message_button_4 {6};
const int switch_scan_button {7}; // Button to start switch scanning
const int switch_advance_button {2}; // Button to select a message

// Variables related to timing
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
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
int LEDState = LOW;
int which_switch = -1;
int previous_file_number = 1; // Store previous file number to check if it's the same switch being pressed again
bool play_transition[2] = {false, false};
bool switch_scanning = false; // Variable to track if we're in switch scanning or direct press
// Hard code in the file variable
char file_name[][12]{"rec_1_1.wav", "rec_1_2.wav", "rec_1_3.wav", "rec_1_4.wav"};
char file [12];
bool advance_message = false; // variable to advance to next message
//------------------------------------------------------------------------------------------------------
// Functions

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

//------------------------------------------------------------------------------------------------------
// States of state machine
void waiting(){
// Read inputs, blink LEDs. Make the transition here: check if I have an input, if the audio is playing, etc.

  // #ifdef DEBUG
  //   Serial.print(digitalRead(switch_scan_button));
  //   Serial.println("Waiting State");
  // #endif
  
  // Add statement to handle switch scanning in this waiting state.
  if(switch_scanning){

    // Will need to read the interval later, rather than hard coded.
    // If the timer ends without an input, advance to the next message if it's within the message count total
    if(mytimer.done()){
      if(file_number < message_total){
      file_number += 1;
      strcpy(file,file_name[file_number-1]);
      advance_message = true;
      mytimer.reset();
      }

      // If the message is going past the total message count, stop switch scanning
      else{
        switch_scanning = false;
        advance_message = false;
        mytimer.reset();
      }
    }

    // If the advance switch is pressed, then advance to the next message if it's within the message total
    if(digitalRead(switch_advance_button) == LOW){
      while(digitalRead(switch_advance_button) == LOW){
        // wait for the switch to be released
      }
      if(file_number < message_total){
      file_number += 1;
      strcpy(file,file_name[file_number-1]);
      advance_message = true;
      mytimer.reset();
      }

      // If the message is going past the total message count, stop switch scanning
      else{
        switch_scanning = false;
        advance_message = false;
        mytimer.reset();
      }
    }

    else if(digitalRead(switch_scan_button) == LOW){
      while(digitalRead(switch_scan_button) == LOW){
        // wait for the switch to be released
      }
      // This part works if timing works
      #ifdef DEBUG
        Serial.println("Switch scan button pressed during switch scan pause");
      #endif
      switch_scanning = false;
      advance_message = true;
      mytimer.reset();
    }

  }
}

void play_message(){
  while(digitalRead(which_switch) == LOW){
    // Wait for switch to be released
  }
  // #ifdef DEBUG
  // Serial.print("Switch pressed: ");
  // Serial.print(which_switch);
  // Serial.print(" Playing Message State");
  // Serial.print(" Playing message? ");
  // Serial.println(audio.isPlaying());
  // #endif

  // String file_name = "rec_";
  // file_name += file_level;
  // file_name += "_";
  // file_name += file_number;
  // file_name += file_extension;

  // char file [50];
  // file_name.toCharArray(file,50);

  digitalWrite(play_LED, HIGH);
  digitalWrite(speaker_shutdown, HIGH);

  #ifdef DEBUG
    Serial.print("File: ");
    Serial.println(file);
    // Serial.print(" File Number: ");
    // Serial.print(file_number);
    // Serial.print(" Previous: ");
    // Serial.println(previous_file_number);
    Serial.println(switch_scanning);
    // Serial.println(switch_scan_button);
  #endif

  audio.play(file);

  while(audio.isPlaying()){
    blink_LED();
    play_transition[1] = play_transition[2];
    play_transition[2] = audio.isPlaying();
    // #ifdef DEBUG
    // Serial.print("Play transition: ");
    // Serial.print(play_transition[1]);
    // Serial.print(": ");
    // Serial.println(play_transition[2]);
    // #endif
    // *** NOTE: *** Need to add option for pressing the switch advance or select button to stop playing.
    if((digitalRead(message_button_1) == LOW) || (digitalRead(message_button_2) == LOW) || (digitalRead(message_button_3) == LOW) || (digitalRead(message_button_4) == LOW)){
      play_transition[1] = play_transition[2];
      play_transition[2] = audio.isPlaying();
      audio.stopPlayback();
    }
    else if((digitalRead(switch_advance_button) == LOW) && switch_scanning){
      play_transition[1] = play_transition[2];
      play_transition[2] = audio.isPlaying();
      audio.stopPlayback();
      while(digitalRead(switch_advance_button) == LOW){
        // wait for the switch to be released
      }
      if(file_number < message_total){
      file_number += 1;
      strcpy(file,file_name[file_number-1]);
      advance_message = true;
      mytimer.reset();
      }

      // If the message is going past the total message count, stop switch scanning
      else{
        switch_scanning = false;
        advance_message = false;
        mytimer.reset();
      }
    }
    // If the switch scan button is pressed, replay the message
    else if((digitalRead(switch_scan_button) == LOW) && switch_scanning){
            while(digitalRead(switch_scan_button) == LOW){
        // wait for the switch to be released
      }
      play_transition[1] = play_transition[2];
      play_transition[2] = audio.isPlaying();
      audio.stopPlayback();
      advance_message = true;
      switch_scanning = false;
      mytimer.reset();

    }
  }
  digitalWrite(speaker_shutdown, LOW);
  digitalWrite(play_LED, LOW);
  play_transition[1] = true;
  play_transition[2] = audio.isPlaying();
  #ifdef DEBUG
    Serial.print(play_transition[1]);
    Serial.print(play_transition[2]);
    Serial.println("Exited playing");
  #endif
  if(switch_scanning){
    // Will need to make thing to read timer interval later
    mytimer = Neotimer(interval);
    mytimer.start();
  }

}

//-----------------------------------------------------------------------------------------------------------------------
// Transitions

// Create transition from waiting state to direct message playing
bool transitionS0S1(){
  // If any of the direct message buttons are pressed, find out which one
  if((digitalRead(message_button_1) == LOW) || (digitalRead(message_button_2) == LOW) || (digitalRead(message_button_3) == LOW) || (digitalRead(message_button_4) == LOW) || (digitalRead(switch_scan_button) == LOW)){
    which_switch = -1;
    previous_file_number = file_number;

    // Step through the buttons to find out which one was pressed
      for(int i = 3;i<8;i++){
        if (digitalRead(i) == LOW){
          which_switch = i;
        }
      }
      // Write the file number to be passed to the playing function to write the file
      if (which_switch == 3){
        file_number = 1;
        strcpy(file,file_name[file_number-1]);
        switch_scanning = false;
      }
      else if (which_switch == 4){
        file_number = 2;
        strcpy(file,file_name[file_number-1]);
        switch_scanning = false;
      }
      else if (which_switch == 5){
        file_number = 3;
        strcpy(file,file_name[file_number-1]);
        switch_scanning = false;
      }
      else if (which_switch == 6){
        file_number = 4;
        strcpy(file,file_name[file_number-1]);
        switch_scanning = false;
      }
      else if (which_switch == 7){
        if(switch_scanning == false){
          file_number = 1;
          switch_scanning = true; // Turn on the switch scanning variable to track that we are switch scanning when we go into the play button transitions.
          strcpy(file,file_name[file_number-1]);
          #ifdef DEBUG
            Serial.println("Start switch scanning");
          #endif
        }
        else if (switch_scanning){
          switch_scanning = false;
          #ifdef DEBUG
            Serial.println("Repeat message");
          #endif
        }
      }
  
  #ifdef DEBUG
    Serial.println("Waiting to playing transition");
  #endif

  return true;
  }
  
  else if(advance_message){
    advance_message = false;
    return true;
  }

  else{
  return false;
  }
}

// Create transition to interupt playing where if a different button is pressed it moves to that message.
bool transitionS1S1(){
  // If any of the direct message buttons are pressed, find out which one
  if((digitalRead(message_button_1) == LOW) || (digitalRead(message_button_2) == LOW) || (digitalRead(message_button_3) == LOW) || (digitalRead(message_button_4) == LOW) && (play_transition[1] == true)){
    which_switch = -1;
    previous_file_number = file_number;
    switch_scanning = false;

    // Add something to stop playing audio.
    // Step through the buttons to find out which one was pressed
      for (int i = 3;i<7;i++){
        if (digitalRead(i) == LOW){
          which_switch = i;
        }
      }
      // Write the file number to be passed to the playing function to write the file
      if (which_switch == 3){
        file_number = 1;
      }
      else if (which_switch == 4){
        file_number = 2;
      }
      else if (which_switch == 5){
        file_number = 3;
      }
      else if (which_switch == 6){
        file_number = 4;
      }

  #ifdef DEBUG
    Serial.println("Playing to playing transition");
  #endif

  return true;
  }
  else{
  return false;
  }
}

// Create transition from playing to waiting

bool transitionS1S0(){

  if((play_transition[1] == true) && (play_transition[2] == false)){
    #ifdef DEBUG
      Serial.println("Playing to waiting transition");
    #endif
    play_transition[1] = false;
    return true;
  }
  else{
    return false;
  }
}

// Create transition from playing a message to switch scanning delay
// bool transitionS1S2(){
//   if(switch_scanning && (play_transition[1] == true) && (play_transition[2] == false)){
//     #ifdef DEBUG
//     Serial.println("Playing to scan delay transition");
//     #endif
//     // play_transition[1] = false;
//     mytimer = Neotimer(interval);
//     mytimer.start();
//     return true;
//   }
//   else{
//     return false;
//   }
  
// }

// Create transition from switch scanning delay to playing message

// bool transitionS2S1(){

//     }

// }

// Create transition from switch scanning delay to waiting

// bool transitionS2S0(){
//   if(switch_back){
//     switch_back = false;
//     switch_scanning = false;
//     #ifdef DEBUG
//       Serial.println("Scan delay to waiting transition");
//     #endif
//     return true;
//   }
//   else{
//     return false;
//   }
// }


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
  pinMode(message_button_1, INPUT_PULLUP);
  pinMode(message_button_2, INPUT_PULLUP);
  pinMode(message_button_3, INPUT_PULLUP);
  pinMode(message_button_4, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(level_button),level_int,FALLING);
  //pinMode(power_switch, INPUT_PULLUP);
  pinMode(speaker_shutdown, OUTPUT);
  pinMode(switch_scan_button,INPUT_PULLUP);
  pinMode(switch_advance_button,INPUT_PULLUP);
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
  // Set transitions between states
  S0->addTransition(&transitionS0S1,S1); // Transition from waiting to playing a message
  S1->addTransition(&transitionS1S0,S0); // Transition from playing a message to waiting
  S1->addTransition(&transitionS1S1,S1); // Transition from playing a message to playing a new message
  // S1->addTransition(&transitionS1S2,S2); // Transition from playing a message to the switch delay

}

void loop() {
  // put your main code here, to run repeatedly:
  play_machine.run();

}
