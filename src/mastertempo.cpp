#include "mastertempo.hpp"
#include "MidiFileIn.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>



using namespace stk;
using namespace std;


void MidiEvent::printEvent(){
	cout << "Note ID: " << n << " | " <<
		"Pressed: " << noteIsOn <<
		" | Velocity: " << vel <<
		" | Ticks since last event: " << ticksSinceLastEvent << "\n";
}

bool MasterTempo::decodeMidi(){
	vector<unsigned char> midiEvents = vector<unsigned char>();
	vector<unsigned char> nextEvent = vector<unsigned char>();
	MidiFileIn midiFile(midi);

	unsigned long deltaTime = midiFile.getNextMidiEvent(&midiEvents, 0);

	while(!midiEvents.empty()){
		double currentDeltaTimeTicks = midiFile.getTickSeconds(0);

		char d2 = midiEvents.back();         midiEvents.pop_back();
		char d1 = midiEvents.back();         midiEvents.pop_back();
		int ch_msg = (int)midiEvents.back(); midiEvents.pop_back();

		if(ch_msg == (128+16)){
			noteOn(d1, d2, deltaTime);
		}
		if(ch_msg == 128){
			noteOff(d1, d2, deltaTime);
		}

		deltaTime = midiFile.getNextMidiEvent(&midiEvents, 0);
	}
	return true;
}


void MasterTempo::noteOff(char data1, char data2, unsigned long ticks){
	int note = getNoteId(data1);
	uint8_t vel = getVelocity(data2);
	float velf = ((float)vel)/MAX_VELOCITY;
	MidiEvent mev = MidiEvent(note, false, velf, ticks);
	midiEvents.push_back(mev);
}


void MasterTempo::noteOn(char data1, char data2, unsigned long ticks){
	int note = getNoteId(data1);
	uint8_t vel = getVelocity(data2);
	float velf = ((float)vel)/MAX_VELOCITY;
	MidiEvent mev = MidiEvent(note, true, velf, ticks);
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