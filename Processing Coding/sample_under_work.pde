// Sample Control for Processing V.90
// Description: Loads and manipulates the Pitch and Panning of a sample object
// Instructions: Move the mouse Up/Down to control the sample-pitch, and sideways for the panning.
// By: Amit Pitaru on July 16th 2005

import pitaru.sonia_v2_9.*; // automatically created when importing sonia using the processing menu.
Sample mySample;

import processing.serial.*;
Serial myPort;
float value_x = 0;
float value_y = 0;

float [] valueArrayX;
float [] valueArrayY;

float avg_X;
float avg_Y;

int countIndex = 0;
int recordInterval = 5000;
int recordTime = 10;

int currentTime = 0;
int previousTime = 0;

int i;

void setup(){

size(512,200);
  Sonia.start(this); // Start Sonia engine.
  // create a new sample object.
  mySample = new Sample("rolling.aif");   
  
  String portName = "COM22";
  myPort = new Serial(this,portName,19200);
  myPort.bufferUntil('\n');
  
  valueArrayX = new float[10];
  valueArrayY = new float[10];
}

void draw()
{
 background(0,30,0);
 strokeWeight(1); 
 
 currentTime = millis();
 if((currentTime - previousTime) > recordInterval){
   println("assessing run");
   countIndex = (countIndex + 1) % recordTime;
   valueArrayX[countIndex] = value_x;
   valueArrayY[countIndex] = value_y;
   
   println("index:"+countIndex+"X:"+","+"Y:"+valueArrayY[countIndex]);
   
   for(int i = 0; i < recordTime; i++){
     avg_X = avg_X + valueArrayX[i];
     avg_Y = avg_Y + valueArrayY[i];
   }
   avg_X = avg_X + valueArrayX[i];
   avg_Y = avg_Y + valueArrayY[i];
 }
 avg_X = avg_X / recordTime;
 avg_Y = avg_Y / recordTime;
 previousTime = currentTime;

 
 // If sample is playing (or looping), do this...
 if(mySample.isPlaying()){
   background(0,40,0);
 }
           
 setRate(); // use mouseY to control sample-rate playback
 setPan(); // use mouseX to control sample Panning
 //setVolume(); // use mouseX to control sample volume
 drawScroller();            
 
 mySample.repeat();
}

  void serialEvent(Serial p){
  String s ="";
  String[] values;
  
  if(p.available() > 0){
    s = p.readString();
    if(s != null){
      //println("received:" + s);
      
      values = s.split(",");
      //println(values);
      
      value_x = value_x - (value_x - float(values[0]))*0.1; //smoothing
      value_y = float(values[1]);
    }
  }
}


void setPan(){
  // set the pan of the sample object.
  // Range: float from -1 to 1 .... -1 -> left, 0 -> balanced ,1 -> right
  // notes: only works with MONO samples. Pan for Stereo support in next version.
  float pan = -1f + avg_X/(width/2f);
  mySample.setPan(pan);

}

// NOT IN USE FOR THIS EXAMPLE
void setVolume(){
  // set the volume of the sample. 
  // Range: float from 0 to 1 
  float vol = avg_Y/(height*1f);
  mySample.setVolume(vol);
}

void setRate(){
   // set the speed (sampling rate) of the sample.
   // Values:
   // 0 -> very low pitch (slow playback).
   // 88200 -> very high pitch (fast playback).
   float rate = (height - avg_Y)*88200/(height);
   mySample.setRate(rate);
}

// Draw a scroller that shows the current sample-frame being played.
// Notice how the sample plays faster when the Sample-Rate is higher.(controlled by mouseY)

void drawScroller(){
 strokeWeight(1);
 stroke(0,255,0);

 // figure out which percent of the sample has been played.
 float percent = mySample.getCurrentFrame() *100f / mySample.getNumFrames();
 // calculate the marker position
 float marker = percent*width/100f;
 // draw...
 line(marker,0,marker,20);
 line(0,10, width,10);
}

// Safely close the sound engine upon Browser shutdown.
public void stop(){
  Sonia.stop();
  super.stop();
}
         
