#include "mastertempo.hpp"
#include "MidiFileIn.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>


using namespace stk;
using namespace std;


#define MIDI_FRAME_DELAY -1


void MasterTempo::updateFrame(int frame){
	currentFrame = frame - MIDI_FRAME_DELAY;
	if(currentFrame < 0) currentFrame = 0;
	updateState();
}

void MasterTempo::updateState(){
	double currentTime = ((double)currentFrame)/fps;

	for(vector<MidiEvent>::iterator mev = midiEvents.begin(); mev != midiEvents.end(); ++mev){
		MidiEvent e = (*mev);
		if(e.getEventTime() > currentTime) break;
		int eId = e.getNote();
		bool eStatus = e.isNoteOn();
		float eVel = e.getVel();
		noteStatus[eId] = eStatus;
		noteVelocity[eId] = eVel;
	}
}

void MidiEvent::printEvent(){
	cout << "Note ID: " << n << " | " <<
		"Pressed: " << noteIsOn <<
		" | Velocity: " << vel <<
		" | Event time: " << eventTime << "s\n";
}

bool MasterTempo::decodeMidi(){
	vector<unsigned char> midiEvents = vector<unsigned char>();
	vector<unsigned char> nextEvent = vector<unsigned char>();
	MidiFileIn midiFile(midi);

	unsigned long deltaTime = midiFile.getNextMidiEvent(&midiEvents, 0);
	// not sure why we need to multiply by 2 here, but doing 60.0/bpm yields only
	// half as much as it should, so this'll have to do for now... (in SMF0 the bpm
	// info isn't stored apparently, so that needs manual input)
	//
	// UPDATE: was making the animation 2x as 
	secondsPerTick = midiFile.getTickSeconds(0) * (120.0 / bpm);

	double currentTickValue = 0;

	while(!midiEvents.empty()){
		currentTickValue += deltaTime;

		char d2 = midiEvents.back();         midiEvents.pop_back();
		char d1 = midiEvents.back();         midiEvents.pop_back();
		int ch_msg = (int)midiEvents.back(); midiEvents.pop_back();

		if(ch_msg == (128+16)){
			noteOn(d1, d2, currentTickValue * secondsPerTick);
		}
		if(ch_msg == 128){
			noteOff(d1, d2, currentTickValue * secondsPerTick);
		}

		deltaTime = midiFile.getNextMidiEvent(&midiEvents, 0);
	}

	return true;
}


void MasterTempo::noteOff(char data1, char data2, double eventTime){
	int note = getNoteId(data1);
	uint8_t vel = getVelocity(data2);
	float velf = ((float)vel)/MAX_VELOCITY;
	MidiEvent mev = MidiEvent(note, false, velf, eventTime);
	midiEvents.push_back(mev);
}


void MasterTempo::noteOn(char data1, char data2, double eventTime){
	int note = getNoteId(data1);
	uint8_t vel = getVelocity(data2);
	float velf = ((float)vel)/MAX_VELOCITY;
	MidiEvent mev = MidiEvent(note, true, velf, eventTime);
	midiEvents.push_back(mev);
}

int MasterTempo::getNoteId(char data){
	return (int)data;
}

int MasterTempo::getVelocity(char data){
	return (uint8_t)data;
}

void MasterTempo::printMidiEvents(){
	cout << "Printing MIDI events...\n";
	int linecount = 1;
	for(vector<MidiEvent>::iterator mev = midiEvents.begin(); mev != midiEvents.end(); ++mev){
		for(int i = 1; 10000 > i*linecount; i *= 10) cout << " ";
		cout << linecount++ << ". ";
		(*mev).printEvent();
	}
}