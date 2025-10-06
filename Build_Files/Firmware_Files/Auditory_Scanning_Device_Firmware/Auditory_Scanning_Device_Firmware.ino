/* 
* File: Chatterbox_Firmware.ino
* Developed by: MakersMakingChange
* Version: v1.0 (02 February 2024)
  License: GPL v3.0 or later

  Copyright (C) 2025 Neil Squire Society
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
#include <TMRpcm.h>  // Message recording and playback library
#include <SD.h> // Libary
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
Neotimer recordTimer; // Set timer for when to go from just replaying a message to recording it

// Declare state machine and states
//-------------------------------------------------------------------------------------
StateMachine play_machine = StateMachine(); // Declare the state machine

State* S0 = play_machine.addState(&waiting); // State for when waiting for an input
State* S1 = play_machine.addState(&play_message); // State for playing a message
State* S2 = play_machine.addState(&record_waiting); // State for when waiting for an input to record a new message
State* S3 = play_machine.addState(&record_message); // State for when we're recording a message

// TODO: Use define statements instead of constants to reduce memory. ex: #define CONSTANT_VALUE 2, where 2 is a value
//Additional Arduino connections
//----------------------------------------------------------------------------------------
const int mic {A0};
const int switch_advance_button {2}; // Button to advance to the next message when switch scanning
const int message_button_1 {3}; // Button to play/record first message
const int message_button_2 {4}; // Button to play/record second message
const int message_button_3 {5}; // Button to play/record third message
const int message_button_4 {6}; // Button to play/record fourth message
const int switch_scan_button {7}; // Button to start switch scanning/select a message
const int speaker_shutdown {8};
const int message_LED_3 {A3}; // Message 3 LED output
const int message_LED_4 {A4}; // Message 4 LED output
const int mode_ID {A5}; // analog input pin to read if it's in playback or recording mode
const int level_ID {A6}; // analog input pin to read which level is selected
const int speed_ID {A7}; // analog input pin to read which playback speed is selected
unsigned int current_message_LED = 0; // LED that will light up or blink for messages, etc.

const int message_buttons[5] = {3, 4, 5, 6, 7}; // Array for the switch pins on the microcontroller.
// The first four spots are for the direct message buttons, and the last is for the switch scanning button.

const int message_LEDs[4] = {A1, A2, A3, A4}; // Array for the message LED pins to the microcontroller.

// Declare variables
//-----------------------------------------------------------------------------------------
// Timing variables
const long delay_intervals[3] = {3000, 5000, 7000}; // Set how long the delay is between playbacks in ms.
const long blinkInterval = 500;
unsigned long record_interval = 1000;
unsigned long previousMillis = 0; // TODO: check if using Neotimer library or this is bigger. Use only one.
unsigned long currentMillis = 0;
const unsigned long debounce_delay = 50; // Debouncing delay time
// unsigned long record_millis = 0;
// unsigned long record_previous_millis = 0;

//File properties
int file_number =0;
int file_level {1};
int current_message {1};
const int sample_rate {16000};
const int message_total = 3; // Total number of messages to be stored on any one level
int LEDState = LOW;
int which_switch = -1;
// Hard code in the file variable name
char file_ends[][7]{"_1.wav", "_2.wav", "_3.wav", "_4.wav"};
char file [12];

// Other variables
const int playback_threshold = 510; // variable for checking the threshold to be in playback vs record mode
const int threshold[2] = {300, 510}; // Variable for checking the level to run and changing delay between messages based on the ouptut of a resistor ladder

// Boolean values for triggering transitions
//-----------------------------------------------------------------------------------------------------

struct Flags {
  bool advance_message : 1; // Stop playing a message and move to the next
  bool switch_scanning : 1; // Indicate if in switch scanning or playing a message directly
  bool recording_mode : 1; // Indicate if in recording or switch scanning mode
  bool record_a_message : 1; // Transition to recording a message
  bool record_playback : 1; // Playback a message from the recording mode
  bool first_loop : 1; // Indicate if it's the first time through a loop
  // Add more flags here
};
Flags flags;

bool play_transition[2] = {false, false}; // Tracks the state of messages playing. It stores the previous state (false for not playing, true for playing) and can compare that to the current state.

//------------------------------------------------------------------------------------------------------
// Functions

// Blink single LED function

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
    digitalWrite(current_message_LED, LEDState);
  }
}

// Blink all LEDs function. Used when in record mode.

void blink_all_LEDs(){
  currentMillis = millis();

  if(currentMillis - previousMillis >= blinkInterval){
    previousMillis = currentMillis;

    if(LEDState == LOW){
      LEDState = HIGH;
    }
    else{
      LEDState = LOW;
    }
    digitalWrite(message_LEDs[0], LEDState);
    digitalWrite(message_LEDs[1], LEDState);
    digitalWrite(message_LEDs[2], LEDState);
    digitalWrite(message_LEDs[3], LEDState);
  }
}

// Check and set the level the messages are on
char check_level(){

  if(analogRead(level_ID) <= threshold[0]){
    strcpy(file,"rec_1");
    strcat(file,file_ends[file_number]);
    return file;
  }
  else if(analogRead(level_ID) >= threshold[1]){
    strcpy(file,"rec_3");
    strcat(file,file_ends[file_number]);
    return file;
  }
  else{
    strcpy(file,"rec_2");
    strcat(file,file_ends[file_number]);
    return file;
  }
}

// Check and set the delay between advancing to the next message while switch scanning
long check_delay_duration(){
  if(analogRead(speed_ID) <= threshold[0]){
    return delay_intervals[0];
  }
  else if(analogRead(speed_ID) >= threshold[1]){
    return delay_intervals[2];
  }
  else{
    return delay_intervals[1];
  }
}

// Checking memory function for debugging. Output free RAM in bytes.

extern int __heap_start, *__brkval;
int freeMemory() {
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

//------------------------------------------------------------------------------------------------------
// States of state machine

// Default state for waiting on an input
void waiting(){
// Read inputs, blink LEDs. Make the transition here: check if I have an input, if the audio is playing, etc.

  // #ifdef DEBUG
  //   Serial.print(analogRead(mode_ID));
  //   Serial.println(F(" Mode pin reading"));
  //   Serial.print(analogRead(level_ID));
  //   Serial.println(F(" Level pin reading"));
  //   Serial.print(analogRead(speed_ID));
  //   Serial.println(F(" Speed pin reading"));
  // #endif
  
  // Handling switch scanning (waiting for input and/or advancing message)
  if(flags.switch_scanning){

    // NOTE: Will need to read the interval later, rather than hard coded.

    // If the timer ends without an input, advance to the next message if it's within the message count total.
    if(delayTimer.done()){
      if(file_number < message_total){
      digitalWrite(current_message_LED,LOW);
      file_number += 1;
      // strcpy(file,file_name[file_number]);
      check_level();
      current_message_LED = message_LEDs[file_number];
      flags.advance_message = true;
      delayTimer.reset();
      }

      // If the message is going past the total message count, stop switch scanning
      else{
        digitalWrite(current_message_LED,LOW);
        flags.switch_scanning = false;
        flags.advance_message = false;
        delayTimer.reset();
      }
    }

    // If the advance switch is pressed, then advance to the next message if it's within the message total
    if(digitalRead(switch_advance_button) == LOW){
      delay(debounce_delay);
      while(digitalRead(switch_advance_button) == LOW){
        // wait for the switch to be released
      }
      if(file_number < message_total){
      file_number += 1;
      // strcpy(file,file_name[file_number]);
      check_level();
      current_message_LED = message_LEDs[file_number];
      flags.advance_message = true;
      delayTimer.reset();
      }

      // If the message is going past the total message count, stop switch scanning
      else{
        flags.switch_scanning = false;
        flags.advance_message = false;
        delayTimer.reset();
      }
    }

    // If the switch scanning button is pressed again, replay the current message and stop switch scanning.
    else if(digitalRead(switch_scan_button) == LOW){
      delay(debounce_delay);
      while(digitalRead(switch_scan_button) == LOW){
        // wait for the switch to be released
      }
      #ifdef DEBUG
        Serial.println(F("Switch scan button pressed during switch scan pause"));
      #endif
      flags.switch_scanning = false;
      flags.advance_message = true;
      delayTimer.reset();
    }

  }
}

// State to play message. Might make more sense as a function than a state.
void play_message(){
  // Turn off all LEDs
  digitalWrite(message_LEDs[0],LOW);
  digitalWrite(message_LEDs[1],LOW);
  digitalWrite(message_LEDs[2],LOW);
  digitalWrite(message_LEDs[3],LOW);
  
  while(digitalRead(message_buttons[which_switch]) == LOW){
    // Wait for switch to be released
  }

  digitalWrite(current_message_LED, HIGH); // Turn the LED on
  digitalWrite(speaker_shutdown, HIGH); // Turn speaker on

  #ifdef DEBUG
    Serial.print(F("File: "));
    Serial.println(file);
    // Serial.print(" File Number: ");
    // Serial.print(file_number);
    // Serial.println(flags.switch_scanning);
    // Serial.println(digitalRead(message_buttons[which_switch]));
    // Serial.println(which_switch);
    Serial.print(F("Free memory: "));
    Serial.println(freeMemory());
  #endif

  audio.play(file); // Start playing the file. The file is defined in the transition to this state.

  #ifdef DEBUG
    Serial.print(F("Free memory while playing: "));
    Serial.println(freeMemory());
  #endif

  while(audio.isPlaying()){
    // Update transition variable.
    play_transition[1] = play_transition[2];
    play_transition[2] = audio.isPlaying();

    // Interrupt playing if another button is pressed, and start playing that message instead.
    if((digitalRead(message_buttons[0]) == LOW) || (digitalRead(message_buttons[1]) == LOW) || (digitalRead(message_buttons[2]) == LOW) || (digitalRead(message_buttons[3]) == LOW)){
      delay(debounce_delay);
      play_transition[1] = play_transition[2];
      play_transition[2] = audio.isPlaying();
      audio.stopPlayback(); // Stop current audio
    }
    // Interrupt playing if the switch advance button is pressed and advance message
    else if((digitalRead(switch_advance_button) == LOW) && flags.switch_scanning){
      delay(debounce_delay);
      digitalWrite(current_message_LED,LOW);
      play_transition[1] = play_transition[2];
      play_transition[2] = audio.isPlaying();
      audio.stopPlayback(); // Stop playing message
      while(digitalRead(switch_advance_button) == LOW){
        // wait for the switch to be released
      }
      // Increase the file number to play the next message
      if(file_number < message_total){
      file_number += 1;
      current_message_LED = message_LEDs[file_number];
      // strcpy(file,file_name[file_number]);
      check_level();
      flags.advance_message = true;
      delayTimer.reset();
      }

      // If the message is going past the total message count, stop switch scanning
      else{
        flags.switch_scanning = false;
        flags.advance_message = false;
        delayTimer.reset();
      }
    }
    // If the switch scan button is pressed while switch scanning, replay the message
    else if((digitalRead(switch_scan_button) == LOW) && flags.switch_scanning){
            delay(debounce_delay);
            while(digitalRead(switch_scan_button) == LOW){
        // wait for the switch to be released
      }
      play_transition[1] = play_transition[2];
      play_transition[2] = audio.isPlaying();
      audio.stopPlayback();
      flags.advance_message = true; // Re-use advance message transition here to reduce number of variables
      flags.switch_scanning = false;
      delayTimer.reset();

    }
  }
  digitalWrite(speaker_shutdown, LOW); // Turn the speaker off after a message stops
  digitalWrite(current_message_LED, LOW); // Turn off the LED
  play_transition[1] = true; // Set first play transition variable in case timing has gone odd.
  play_transition[2] = audio.isPlaying(); // Check to make sure this transition variable is false.
  #ifdef DEBUG
    Serial.print(play_transition[1]);
    Serial.print(play_transition[2]);
    Serial.println(F("Exited playing"));
  #endif

  // Start the delay timer for waiting between playing messages if in switch scanning.
  if(flags.switch_scanning){
    // Will need to make thing to read timer interval later
    // delayTimer = Neotimer(delay_interval);
    delayTimer = Neotimer(check_delay_duration());
    delayTimer.start();
    digitalWrite(current_message_LED,HIGH);
  }

  // Turn off the recorded message playback variable if it was on

  if(flags.record_playback){
    flags.record_playback = false;
  }

}

// Recording states. Note that they just blink the LED currently to have them do something.
void record_waiting(){
  // Blink the four message LEDs to indicate that it is in record mode.
  blink_all_LEDs();

  // Check if a button has been pressed, then start the timer to check if it should get played back or recorded over

    if((digitalRead(message_buttons[0]) == LOW) || (digitalRead(message_buttons[1]) == LOW) || (digitalRead(message_buttons[2]) == LOW) || (digitalRead(message_buttons[3]) == LOW)){
      
      // Do some prep that only needs to be completed once while the loop is running
      if(flags.first_loop){
      // Start timer to see if we should replay a message or start recording, based on the 
      recordTimer = Neotimer(record_interval);
      recordTimer.start();
      // Turn off all LEDs
      digitalWrite(message_LEDs[0],LOW);
      digitalWrite(message_LEDs[1],LOW);
      digitalWrite(message_LEDs[2],LOW);
      digitalWrite(message_LEDs[3],LOW);
      
      // Step through the buttons to find out which one was pressed
      for(int i = 0; i < 4; i++){
        if (digitalRead(message_buttons[i]) == LOW){
          which_switch = i;
        }
      }
      file_number = which_switch;
      // strcpy(file,file_name[file_number]);
      check_level();
      current_message_LED = message_LEDs[which_switch];
      flags.switch_scanning = false;
      flags.first_loop = false;
      }

        if(recordTimer.done()){
          recordTimer.reset();
          flags.record_a_message = true;
          #ifdef DEBUG
          Serial.println(F("Timer done, but still in while loop."));
          #endif
        }
      // recordTimer.reset();
      // flags.first_loop = true;
      // flags.record_playback = true;
  }

  // Playback the message if the button was pressed but released before the timer was finished.

  if((flags.first_loop == false) && (digitalRead(message_buttons[which_switch]) == HIGH)){
    recordTimer.reset();
    flags.first_loop = true;
    flags.record_playback = true;
  }

}

void record_message(){
  
  // Turn off all LEDs
  digitalWrite(message_LEDs[0],LOW);
  digitalWrite(message_LEDs[1],LOW);
  digitalWrite(message_LEDs[2],LOW);
  digitalWrite(message_LEDs[3],LOW);
  
  //Start the recording
  #ifdef DEBUG
  Serial.println("Recording Started");
  #endif
  
  audio.startRecording(file, sample_rate, mic);
  
  //blink rec led until playback button is released, ending the recording
  while(digitalRead(message_buttons[which_switch]) == LOW){
    digitalWrite(current_message_LED, LOW);
    delay(500);
    digitalWrite(current_message_LED, HIGH);
    delay(500);
  }

  //Stop the recording
  audio.stopRecording(file);

  
  #ifdef DEBUG
  Serial.println("Recording Stopped");
  
  //check if audio file was saved to the SD card
  if(SD.exists(file)){
    Serial.print(file);
    Serial.println(" has been saved to the SD card");
  }else{
    Serial.print("Error: ");
    Serial.print(file);
    Serial.println(" wasn't saved to SD card"); 
  }
  #endif
  flags.first_loop = true; // Reset the first loop flag
  flags.record_a_message = false;

}

//-----------------------------------------------------------------------------------------------------------------------
// Transitions

// Create transition from waiting state to direct message playing
bool transitionS0S1(){
  // Transition to playing if any of the direct message buttons or the switch scanning button are pressed.
  if((digitalRead(message_buttons[0]) == LOW) || (digitalRead(message_buttons[1]) == LOW) || (digitalRead(message_buttons[2]) == LOW) || (digitalRead(message_buttons[3]) == LOW) || (digitalRead(message_buttons[4]) == LOW)){
    delay(debounce_delay);
    which_switch = -1; // Variable to store which switch is pressed

    // Step through the buttons to find out which one was pressed
      for(int i = 0; i < 5; i++){
        if (digitalRead(message_buttons[i]) == LOW){
          which_switch = i;
        }
      }

            // This is for the switch scanning button.
      if (which_switch == 4){
        // If not currently switch scanning, then start at the first message
        if(flags.switch_scanning == false){
          file_number = 0; 
          current_message_LED = message_LEDs[0];
          flags.switch_scanning = true; // Turn on the switch scanning variable to track that we are switch scanning when we go into the play button transitions.
          // strcpy(file,file_name[file_number]);
          check_level();
          #ifdef DEBUG
            Serial.println(F("Start switch scanning"));
          #endif
        }
        // If already switch scanning, then turn off switch scanning and repeat the message
        else if (flags.switch_scanning){
          flags.switch_scanning = false;
          #ifdef DEBUG
            Serial.println(F("Repeat message"));
          #endif
        }
      }

      else {
        file_number = which_switch;
        // strcpy(file,file_name[file_number]);
        check_level();
        current_message_LED = message_LEDs[which_switch];
        flags.switch_scanning = false;
      }


  
  #ifdef DEBUG
    Serial.println(F("Waiting to playing transition"));
  #endif
  // Returning true if a button to start playing a message is pressed.
  return true;
  }
  
  // If a message needs to be advanced or replayed:
  else if(flags.advance_message){
    // Turn off advancing the message
    flags.advance_message = false;
    // Transition into playing a message
    return true;
  }

  // If a message button isn't pressed, etc., do not activate this transition.
  else{
  return false;
  }
}

bool transitionS0S2(){

  if(analogRead(mode_ID)<=playback_threshold){
    flags.recording_mode = true;
    return true;
  }
  else{
    return false;
  }
}

bool transitionS2S0(){

  if(flags.recording_mode && (analogRead(mode_ID)>=playback_threshold)){

    flags.recording_mode = false;
    digitalWrite(message_LEDs[0],LOW);
    digitalWrite(message_LEDs[1],LOW);
    digitalWrite(message_LEDs[2],LOW);
    digitalWrite(message_LEDs[3],LOW);
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
      Serial.println(F("Playing to waiting transition"));
    #endif
    play_transition[1] = false;
    return true;
  }
  else{
    return false;
  }
}

// Create transition from recording waiting to playing

bool transitionS2S1(){

  if(flags.record_playback){
    return true;
  }
  else{
    return false;
  }

}

// Create transition from recording waiting to recording a message

bool transitionS2S3(){
  if(flags.record_a_message){
    #ifdef DEBUG
      Serial.println(F("Transitioned to recording a message."));
    #endif
    return true;
  }
  else{
    return false;
  }

}

// Create transition from recording a message to recording waiting

bool transitionS3S2(){
  if(flags.record_a_message == false){
    return true;
  }
  else{
    return false;
  }

}

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);

  // TO DO: Add a timeout because if this isn't connected to a computer it won't work.
  // #ifdef DEBUG
  // while (!Serial) {
  //   // wait for serial port to connect
  // }
  // #endif 

  //Define pins
  pinMode(message_LEDs[0], OUTPUT);
  pinMode(message_LEDs[1], OUTPUT);
  pinMode(message_LEDs[2], OUTPUT);
  pinMode(message_LEDs[3], OUTPUT);
  pinMode(message_buttons[0], INPUT_PULLUP);
  pinMode(message_buttons[1], INPUT_PULLUP);
  pinMode(message_buttons[2], INPUT_PULLUP);
  pinMode(message_buttons[3], INPUT_PULLUP);
  pinMode(speaker_shutdown, OUTPUT);
  pinMode(message_buttons[4],INPUT_PULLUP);
  pinMode(switch_advance_button,INPUT_PULLUP);
  pinMode(mode_ID,INPUT); // Input mode for the analog pin to read the resistor ladder for the mode
  pinMode(level_ID,INPUT); // Input for the analog read pint to read the resistor ladder for the level
  pinMode(speed_ID,INPUT);
  //audio.CSPin = 10;
  audio.speakerPin = 9;
  audio.volume(5);
  audio.quality(1);

  //Ensure access to Micro SD Card

  #ifdef DEBUG
  Serial.println(F("Initializing SD card..."));
  #endif
  
  if (!SD.begin(10)) {
    #ifdef DEBUG
    Serial.println(F("Initialization failed!"));
    #endif
    // If the SD card doesn't initialize, blink the LEDs until a reset.
    while (1){
      digitalWrite(message_LEDs[0], HIGH);
      digitalWrite(message_LEDs[1], HIGH);
      delay(500);
      digitalWrite(message_LEDs[0], LOW);
      digitalWrite(message_LEDs[1], LOW);
      delay(500);
    }
    
  }

  // Set default values of flags
  flags.advance_message = false;
  flags.switch_scanning = false;
  flags.recording_mode = false;
  flags.record_a_message = false;
  flags.record_playback = false;
  flags.first_loop = true;
  // Set transitions between states for the playback machine
  S0->addTransition(&transitionS0S1,S1); // Transition from waiting to playing a message
  S1->addTransition(&transitionS1S0,S0); // Transition from playing a message to waiting
  S0->addTransition(&transitionS0S2,S2); // Transition from waiting to recording mode
  S2->addTransition(&transitionS2S0,S0); // Transition back from recording to waiting mode. 
  S2->addTransition(&transitionS2S1,S1); // Transition from recording waiting to playing a message
  S2->addTransition(&transitionS2S3,S3); // Transition from recording waiting to recording a message
  S3->addTransition(&transitionS3S2,S2); // Transition from recording a message to recording waiting
  

}

void loop() {
  // put your main code here, to run repeatedly:

  play_machine.run();

}