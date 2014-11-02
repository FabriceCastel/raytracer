#include "mastertempo.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>

// MIDI files are created in Ableton Live
// Relevant details:
// Ableton exports MIDI files in the SMF0 format with a resolution of 96 ticks
// PER QUARTER NOTE ONLY
//
// Source: https://www.ableton.com/en/articles/midi-file-format/
//
// In order to get 1/8, 1/16 or 1/32 notes, you could simply multiply the song
// tempo by a power of two and stretch the MIDI track accordingly


void MidiEvent::printEvent(){
	cout << "Note ID: " << n << " | " <<
		"Pressed: " << noteIsOn <<
		" | Velocity: " << vel <<
		" | Ticks since last event: " << ticksUntilNextEvent << "\n";
}

bool MasterTempo::decodeMidi(){
	// The MIDI message byte type is indicated by the first bit:
	// 0 = Data byte
	// 1 = Status byte
	// 
	// The next three bits denote the type of message
	// The last four bits denote the channel it applies to (0-15)
	//
	// *********************
	// THE MIDI MESSAGE BITS
	// *********************
	//                              Data 1               Data 2
	// 
	// 0 = Note off                 Note number          Velocity
	// 1 = Note on                  Note number          Velocity
	// 2 = Polyphonic aftertouch    Note number          Pressure
	// 3 = Control change           Controller number    Data
	// 4 = Program change           Program number       -
	// 5 = Channel aftertouch       Pressure             -
	// 6 = Pitch wheel              LSbyte               MSbyte
	//
	// NOTE: 'note on' + 'velocity = 0' is equivalent to 'note off'
	//
	// the numbers above are the decimal interpretations of the three message
	// bits alone (ie. without the prepended byte type bit)
	//
	// Normally, it takes 6 bytes for each note:
	// [1][pitch][velocity] or [0][pitch][velocity]
	//
	// BUT to save on space, running status is used.
	// This means if the status is unchanged, it won't be included in the file
	// and while reading it, we should assume the following bytes correspond
	// to the same status (status being the on/off state of the note) so we get
	// [1][pitch][velocity][pitch][velocity]...
	//

	char in, data1, data2, data3;
	ifstream file;

	file.open(midi);
	if(file.fail()){
		cout << "Failed to open \"" << midi <<
				"\", make sure the file is in the data folder.\n";
		return false;
	}
	
	int currentStatus = -1;
	bitset<8> b;

	while(file >> in){
		// Read chunk type data
		char chunkType[5] = {'.', '.', '.', '.', '\0'};
		chunkType[0] = in;
		for(int i = 0; i < 3; i++){
			file >> in;
			chunkType[i + 1] = in;
		}
		int MThd = strcmp("MThd", chunkType);
		int MTrk = strcmp("MTrk", chunkType);

		// The next four bytes together form an int; the size of the chunk
		int chunkLength = 0;
		for(int i = 0; i < 4; i++){
			file >> in;
			b = in;
			// Took me forever to figure this one out, but you need to & 0x00FF
			// to the 'in' value because interpreting it as an unsigned int
			// without extends the MSB = 1 and completely messes up the value
			unsigned int ich = 0x00FF & in;
			unsigned int tpi = pow(2, (3-i)*8);
			chunkLength = chunkLength + (ich * tpi);
		}

		if(MThd == 0){
			// Check that the format type is 0 (single track)
			for(int i = 0; i < chunkLength; i++){
				file >> in;
				b = in;
				cout << b << "\n";
			}
			bitset<32> b2(chunkLength);
			cout << "MThd chunk\nLength: " << chunkLength  << "\n" << b2 << "\n";
		}
		if(MTrk == 0){
			bitset<32> b2(chunkLength);
			// skip over track chunk
			for(int i = 0; i < chunkLength && file >> in; i++){
				
			}
			cout << "MTrk chunk\nLength: " << chunkLength << "\n" << b2 << "\n";
		}






		/*
		// look at the first bit in the byte
		bool isStatusByte = ((in & 0x80)  == 0x80);
		if(isStatusByte){

			// look at the next three bits (message type)
			int ch_msg = (in >> 4) & 0x07;
			int channel = (in & 0x0F);
			if(ch_msg < 8 && channel == 0){
				cout << " - VALID MIDI NOTE MESSAGE\n";
				currentStatus = ch_msg;
				file >> data1;
				file >> data2;
				file >> data3;
				b = data1;
				cout << b << " - note ID = " << (int)data1 << "\n";
				b = data2;
				cout << b << " - note velocity = " << (int)data2 << "\n";
				b = data3;
				cout << b << " - ticks until next = " << (int)data3 << "\n";
				switch(ch_msg){
					case 0: // Note off
						noteOff(data1, data2, data3);
						break;
					case 1: // Note on
						noteOn(data1, data2, data3);
						break;
					case 2: // Polyphonic aftertouch
						break;
					case 3: // Control change
						// For now I don't care about messages to controllers
						break;
					case 4: // Program change
						// Used to change between patch/presets of instrument
						break;
					case 5: // Channel aftertouch
						break;
					case 6: // Pitch wheel
						break;
					default:
						break;
				}
			} else {
				//cout << "System message ";
				cout << " - Message type not relevant\n";
			}

			// look at the channel bits
			//cout << "   channel " << (in & 0x0F);
		} else {
			cout << " - unknown data\n";
			//cout << "DATA:   " << (unsigned int)in;
			// if(currentStatus == 0){
			// 	file >> data1;
			// 	file >> data2;
			// 	file >> data3;
			// 	noteOff(data1, data2, data3);
			// } else if(currentStatus == 1){
			// 	file >> data1;
			// 	file >> data2;
			// 	file >> data3;
			// 	noteOn(data1, data2, data3);
			// }

		}*/
	}

	file.close();
	return true;
}


// For both note off and on, run checks for valid (positive) velocity
// It seems to be accepting notes with negative velocity at the moment,
// which clearly isn't correct
void MasterTempo::noteOff(char data1, char data2, char data3){
	int note = getNoteId(data1);
	int vel = getVelocity(data2);
	int ticksUntilNextEvent = (int)data3;
	float velf = ((float)vel)/MAX_VELOCITY;
	if(vel > 0){
		MidiEvent mev = MidiEvent(note, false, velf, ticksUntilNextEvent);
		midiEvents.push_back(mev);
	}
}


void MasterTempo::noteOn(char data1, char data2, char data3){
	int note = getNoteId(data1);
	int vel = getVelocity(data2);
	int ticksUntilNextEvent = (int)data3;
	float velf = ((float)vel)/MAX_VELOCITY;
	if(vel > 0){
		MidiEvent mev = MidiEvent(note, true, velf, ticksUntilNextEvent);
		midiEvents.push_back(mev);
	} else if(vel == 0){
		MidiEvent mev = MidiEvent(note, false, 0, ticksUntilNextEvent);
		midiEvents.push_back(mev);
	}
}

int MasterTempo::getNoteId(char data){
	return (int)data;
}

int MasterTempo::getVelocity(char data){
	return (int)data;
}

void MasterTempo::printMidiEvents(){
	cout << "Printing MIDI events...\n";
	int linecount = 0;
	for(vector<MidiEvent>::iterator mev = midiEvents.begin(); mev != midiEvents.end(); ++mev){
		cout << linecount++ << ". ";
		(*mev).printEvent();
	}
	cout << "Done printing events\n";
}