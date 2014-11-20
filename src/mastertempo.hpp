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
	MidiEvent(int note, bool noteIsOn, float velocity, double etime){
		this->n = note;
		this->noteIsOn = noteIsOn;
		this->vel = velocity;
		this->eventTime = etime;
	};

	int getNote(){return n;};
	bool isNoteOn(){return noteIsOn;};
	float getVel(){return vel;};
	double getEventTime(){return eventTime;};
	void printEvent();

private:
	int n;
	bool noteIsOn;
	float vel;
	int ticksSinceLastEvent;
	double eventTime;
};



class MasterTempo{
public:
	MasterTempo(){
		initialized = false;
		midiEvents = vector<MidiEvent>();
	}

	MasterTempo(string midiFile, double beatsPerMinute, int framerate, int frame){
		for(int i = 0; i < 128; i++){
			noteStatus[i] = false;
			noteVelocity[i] = 0.0f;
		}
		midi = midiFile;
		bpm = beatsPerMinute;
		fps = framerate;
		currentFrame = frame;
		if(decodeMidi())
			initialized = true;
	}

	bool getNoteStatus(int id){
		if(id < 0 || id > 127) return false;
		return noteStatus[id];
	}

	float getNoteVelocity(int id){
		if(id < 0 || id > 127) return 0;
		return noteVelocity[id];
	}

	bool decodeMidi();
	void printMidiEvents();
	void updateFrame(int frame);

private:
	bool initialized;
	int currentFrame;
	int fps;
	string midi;
	double bpm;

	vector<MidiEvent> midiEvents;
	bool noteStatus[128]; // true = note on, false = note off
	float noteVelocity[128];
	double secondsPerTick;

	void updateState();

	void noteOff(char data1, char data2, double eventTime);
	void noteOn(char data1, char data2, double eventTime);

	int getNoteId(char data);
	int getVelocity(char data);
};

#endif // _MASTER_TEMPO_