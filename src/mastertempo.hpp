#ifndef _MASTER_TEMPO_
#define _MASTER_TEMPO_

#include <string>
#include <vector>

using namespace std;


#define MAX_VELOCITY 127.0f


// For now I only care about note on/off events, this class might be extended
// to account for other event types (pitch wheel, aftertouch...) later on
class MidiEvent{
public:
	MidiEvent(int note, bool noteIsOn, float velocity, int ticks){
		this->n = note;
		this->noteIsOn = noteIsOn;
		this->vel = velocity;
		this->ticksUntilNextEvent = ticks;
	};

	int getNote(){return n;};
	bool isNoteOn(){return noteIsOn;};
	float getVel(){return vel;};
	int getticksUntilNextEvent(){return ticksUntilNextEvent;};
	void printEvent();

private:
	int n;
	bool noteIsOn;
	float vel;
	int ticksUntilNextEvent;
};



class MasterTempo{
public:
	MasterTempo(){
		initialized = false;
		midiEvents = vector<MidiEvent>();
	}

	MasterTempo(string midiFile){
		midi = midiFile;
		if(decodeMidi())
			initialized = true;
	}

	bool decodeMidi();
	void printMidiEvents();

private:
	bool initialized;
	string midi;

	vector<MidiEvent> midiEvents;

	void noteOff(char data1, char data2, char data3);
	void noteOn(char data1, char data2, char data3);

	int getNoteId(char data);
	int getVelocity(char data);
};

#endif // _MASTER_TEMPO_