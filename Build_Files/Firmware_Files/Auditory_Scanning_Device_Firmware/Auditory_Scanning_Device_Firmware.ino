/* 
* File: Open_Playback_Recorder_Firmware.ino
* Developed by: MakersMakingChange
* Version: v1.0 (02 February 2024)
  License: GPL v3.0 or later

  Copyright (C) 2024 Neil Squire Society
  This program is free software: you can redistribute it and/or modify it under the terms of
  the GNU General Public License as published by the Free Software Foundation,
  either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with this program.
  If not, see <http://www.gnu.org/licenses/>

<<<<<<<< HEAD:Build_Files/Firmware_Files/Open_Playback_Switch_Firmware/Open_Playback_Recorder_Firmware.ino
  The Open Playback Button is an assistive technology device capable of recording voice messages and storing them for future playback.   
  Messages can be recorded after entering record mode by holding the record button for 2 seconds. In record mode, multiple messages can be recorded to a micro SD 
  card by pressing and holding the play button before pressing the record button again to exit record mode. The stored
  messages can then be played back with each press of the play button. An external button can be plugged into a 3.5 mm
  mono jack to act as an accessible play button.  
========

  The Open Playback Button is a device capable of recording voice messages and storing them for future playback.   
  This code will showcase the basic functionality of the device. Messages can be recorded after entering record 
  mode by holding the record button for 2 seconds. In record mode, multiple messages can be recorded to a micro SD 
  card by pressing and holding the play button before pressing the record button again to exit record mode. The stored
  messages can then be played back with each press of the play button. An external button can be plugged into a 3.5 mm
  mono jack to act as an accessible play button.  

  July 25, 2023
  Brad Wellington

  Updating code to work with the new microcontroller and include the 3 levels or recordings

  Components:

  Play button
  Record button
  Volume potentiometer with on/off switch
  play/rec leds
  3.5 mono jack

  Microphone and Amp
    MAX9814        Arduino Uno
      GND -----------> GND
      VDD -----------> 5V 
      Out -----------> A0

  Micro SD Breakout Board
    SD Module       Arduino Uno
      5V  -----------> 5V
      GND -----------> GND
      CLK -----------> SCK
      DO  -----------> MI
      DI  -----------> MO
      CS  -----------> SS

  Audio Amp: PAM8302A
    Amp Module       Arduino Uno
      GND -----------> GND
      Vin -----------> 5V
      SD  -----------> D11
      A-  -----------> GND 
      A+  -----------> D9 (Through Vol Pot)
      +   -----------> Speaker postive
      -   -----------> Speaker negitive
      
  File: Open_Playback_Recorder_Firmware.ino
  Developed by: Makers Making Change
  Version: V2.0(17 July 2024)
  License: GPL v3
  
Copyright (C) 2023-2024 Neil Squire Society
  This program is free software: you can redistribute it and/or modify it under the terms of
  the GNU General Public License as published by the Free Software Foundation,
  either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with this program.
  If not, see <http://www.gnu.org/licenses/>
  
>>>>>>>> PCB-Version:Build_Files/Firmware_Files/Open_Playback_Recorder_Firmware/Open_Playback_Recorder_Firmware.ino
*/

// Add libraries
//-----------------------------------------------------------------------------------
#include <TMRpcm.h>
#include <SD.h>
#include <SPI.h>
#include <StateMachine.h>
#include <neotimer.h>

// Debugging line
#define DEBUG

// Define library variables
//-----------------------------------------------------------------------------------
TMRpcm audio;
File myFile;
Neotimer delayTimer; // Set timer for the delay between advancing messages without an input

// As mentioned later, actually using this timer causes issues
Neotimer record_timer; // Set timer for when to go from just replaying a message to recording it

// Declare state machine and states
//-------------------------------------------------------------------------------------
StateMachine play_machine = StateMachine(); // Declare the state machine

State* S0 = play_machine.addState(&waiting); // State for when waiting for an input
State* S1 = play_machine.addState(&play_message); // State for playing a message
State* S2 = play_machine.addState(&record_waiting); // State for when waiting for an input to record a new message
// State* S3 = play_machine.addState(&record_playback); // Uncommenting this line causes issue


//Additional Arduino connections
//----------------------------------------------------------------------------------------
const int mic {A0};
const int rec_LED {A1};
const int play_LED {A2};
const int switch_advance_button {2}; // Button to select a message
const int message_button_1 {3}; // Button to play/record first message
const int message_button_2 {4}; // Button to play/record second message
const int message_button_3 {5}; // Button to play/record third message
const int message_button_4 {6}; // Button to play/record fourth message
const int switch_scan_button {7}; // Button to start switch scanning
const int speaker_shutdown {8};
//const int message_LED_1 {A1}; // Message 1 LED output. Will be updated later
//const int message_LED_2 {A2}; // Message 2 LED output. Will be updated later
const int message_LED_3 {A3}; // Message 3 LED output
const int message_LED_4 {A4}; // Message 4 LED output
const int mode_ID {A5}; // analog input pin to read if it's in playback or recording mode
const int level_ID {A6}; // analog input pin to read which level is selected
const int speed_ID {A7}; // analog input pin to read which playback speed is selected

// Declare variables
//-----------------------------------------------------------------------------------------
// Timing variables
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
// Hard code in the file variable name
char file_name[][12]{"rec_1_1.wav", "rec_1_2.wav", "rec_1_3.wav", "rec_1_4.wav"}; // Level 1 filename
char file_name2[][12]{"rec_2_1.wav", "rec_2_2.wav", "rec_2_3.wav", "rec_2_4.wav"}; // Level 2 filename
char file_name3[][12]{"rec_3_1.wav", "rec_3_2.wav", "rec_3_3.wav", "rec_3_4.wav"}; // Level 3 filename
char file [12];

// Other variables
int playback_threshold = 10; // variable for checking the threshold to be in playback vs record mode


// Boolean values for triggering transitions
//-----------------------------------------------------------------------------------------------------
bool advance_message = false; // variable to advance to next message
bool playback_mode = true; // variable to store if it's in record or playback mode
bool play_transition[2] = {false, false};
bool switch_scanning = false; // Variable to track if we're in switch scanning or direct press

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

// Default state for waiting on an input
void waiting(){
// Read inputs, blink LEDs. Make the transition here: check if I have an input, if the audio is playing, etc.

  // #ifdef DEBUG
  //   Serial.print(analogRead(mode_ID));
  //   Serial.println("Analog pin reading");
  // #endif
  
  // Handling switch scanning (waiting for input and/or advancing message)
  if(switch_scanning){

    // NOTE: Will need to read the interval later, rather than hard coded.

    // If the timer ends without an input, advance to the next message if it's within the message count total.
    if(delayTimer.done()){
      if(file_number < message_total){
      file_number += 1;
      strcpy(file,file_name[file_number-1]);
      advance_message = true;
      delayTimer.reset();
      }

      // If the message is going past the total message count, stop switch scanning
      else{
        switch_scanning = false;
        advance_message = false;
        delayTimer.reset();
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
      delayTimer.reset();
      }

      // If the message is going past the total message count, stop switch scanning
      else{
        switch_scanning = false;
        advance_message = false;
        delayTimer.reset();
      }
    }

    // If the switch scanning button is pressed again, replay the current message and stop switch scanning.
    else if(digitalRead(switch_scan_button) == LOW){
      while(digitalRead(switch_scan_button) == LOW){
        // wait for the switch to be released
      }
      #ifdef DEBUG
        Serial.println("Switch scan button pressed during switch scan pause");
      #endif
      switch_scanning = false;
      advance_message = true;
      delayTimer.reset();
    }

  }
}

// State to play message. Might make more sense as a function than a state.
void play_message(){
  while(digitalRead(which_switch) == LOW){
    // Wait for switch to be released
  }

  digitalWrite(play_LED, HIGH); // Turn the LED on
  digitalWrite(speaker_shutdown, HIGH); // Turn speaker on

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

  audio.play(file); // Start playing the file. The file is defined in the transition to this state.

  while(audio.isPlaying()){
    blink_LED(); // Blink the LED while the audio is playing
    // Update transition variable.
    play_transition[1] = play_transition[2];
    play_transition[2] = audio.isPlaying();
    // #ifdef DEBUG
    // Serial.print("Play transition: ");
    // Serial.print(play_transition[1]);
    // Serial.print(": ");
    // Serial.println(play_transition[2]);
    // #endif

    // Interrupt playing if another button is pressed, and start playing that message instead.
    if((digitalRead(message_button_1) == LOW) || (digitalRead(message_button_2) == LOW) || (digitalRead(message_button_3) == LOW) || (digitalRead(message_button_4) == LOW)){
      play_transition[1] = play_transition[2];
      play_transition[2] = audio.isPlaying();
      audio.stopPlayback(); // Stop current audio
    }
    // Interrupt playing if the switch advance button is pressed and advance message
    else if((digitalRead(switch_advance_button) == LOW) && switch_scanning){
      play_transition[1] = play_transition[2];
      play_transition[2] = audio.isPlaying();
      audio.stopPlayback(); // Stop playing message
      while(digitalRead(switch_advance_button) == LOW){
        // wait for the switch to be released
      }
      // Increase the file number to play the next message
      if(file_number < message_total){
      file_number += 1;
      strcpy(file,file_name[file_number-1]);
      advance_message = true;
      delayTimer.reset();
      }

      // If the message is going past the total message count, stop switch scanning
      else{
        switch_scanning = false;
        advance_message = false;
        delayTimer.reset();
      }
    }
    // If the switch scan button is pressed while switch scanning, replay the message
    else if((digitalRead(switch_scan_button) == LOW) && switch_scanning){
            while(digitalRead(switch_scan_button) == LOW){
        // wait for the switch to be released
      }
      play_transition[1] = play_transition[2];
      play_transition[2] = audio.isPlaying();
      audio.stopPlayback();
      advance_message = true; // Re-use advance message transition here to reduce number of variables
      switch_scanning = false;
      delayTimer.reset();

    }
  }
  digitalWrite(speaker_shutdown, LOW); // Turn the speaker off after a message stops
  digitalWrite(play_LED, LOW); // Turn off the LED
  play_transition[1] = true; // Set first play transition variable in case timing has gone odd.
  play_transition[2] = audio.isPlaying(); // Check to make sure this transition variable is false.
  #ifdef DEBUG
    Serial.print(play_transition[1]);
    Serial.print(play_transition[2]);
    Serial.println("Exited playing");
  #endif

  // Start the delay timer for waiting between playing messages if in switch scanning.
  if(switch_scanning){
    // Will need to make thing to read timer interval later
    delayTimer = Neotimer(interval);
    delayTimer.start();
  }

}

// Recording states. Note that they just blink the LED currently to have them do something.
void record_waiting(){
  // Blink the built-in LED while waiting for any input
  blink_LED();

}

void record_playback(){
 blink_LED();

}

void record_message(){
  blink_LED();
}

//-----------------------------------------------------------------------------------------------------------------------
// Transitions

// Create transition from waiting state to direct message playing
bool transitionS0S1(){
  // Transition to playing if any of the direct message buttons or the switch scanning button are pressed.
  if((digitalRead(message_button_1) == LOW) || (digitalRead(message_button_2) == LOW) || (digitalRead(message_button_3) == LOW) || (digitalRead(message_button_4) == LOW) || (digitalRead(switch_scan_button) == LOW)){
    which_switch = -1; // Variable to store which switch is pressed
    previous_file_number = file_number; // Checking if we are repeating a file

    // Step through the buttons to find out which one was pressed
      for(int i = 3;i<8;i++){
        if (digitalRead(i) == LOW){
          which_switch = i;
        }
      }

      // Write the file number and read the "file" variable to be sent to the play function
      if (which_switch == 3){
        file_number = 1;
        strcpy(file,file_name[file_number-1]);
        // Make sure switch scanning is false/reset if it wasn't already
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

      // This is for the switch scanning button.
      else if (which_switch == 7){
        // If not currently switch scanning, then start at the first message
        if(switch_scanning == false){
          file_number = 1; 
          switch_scanning = true; // Turn on the switch scanning variable to track that we are switch scanning when we go into the play button transitions.
          strcpy(file,file_name[file_number-1]);
          #ifdef DEBUG
            Serial.println("Start switch scanning");
          #endif
        }
        // If already switch scanning, then turn off switch scanning and repeat the message
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
  // Returning true if a button to start playing a message is pressed.
  return true;
  }
  
  // If a message needs to be advanced or replayed:
  else if(advance_message){
    // Turn off advancing the message
    advance_message = false;
    // Transition into playing a message
    return true;
  }

  // If a message button isn't pressed, etc., do not activate this transition.
  else{
  return false;
  }
}

bool transitionS0S2(){
  if((digitalRead(switch_advance_button) == LOW) && (switch_scanning == false)){
    while(switch_advance_button == LOW){
      
    }

    delayTimer = Neotimer(interval);
    delayTimer.start();
    // If I use a second timer here then it will not run. If I don't use the "start" command the rest of the code works, but the timer won't
    // Using the same delayTimer allows it to run. 
    // record_timer = Neotimer(interval);
    // record_timer.start();
    return true;
  }
  else{
    return false;
  }
}

bool transitionS2S0(){
  if(delayTimer.done()){
    delayTimer.reset();
    return true;
  }
  // if(record_timer.done()){
  //   record_timer.reset();
  //   return true;
  // }
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
  pinMode(mode_ID,INPUT); // Input mode for the analog pin to read the resistor ladder for the mode
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
  // Set transitions between states for the playback machine
  S0->addTransition(&transitionS0S1,S1); // Transition from waiting to playing a message
  S1->addTransition(&transitionS1S0,S0); // Transition from playing a message to waiting
  S1->addTransition(&transitionS1S1,S1); // Transition from playing a message to playing a new message
  S0->addTransition(&transitionS0S2,S2); // Transition from waiting to recording mode
  // S2->addTransition(&transitionS2S0,S0); // Transition back from recording to waiting mode. Uncommenting this line causes the issue.

}

void loop() {
  // put your main code here, to run repeatedly:

  play_machine.run();

}
