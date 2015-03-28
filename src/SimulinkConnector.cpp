#include "SimulinkConnector.h"

SimulinkConnector::SimulinkConnector(char* aOutputFormat, std::vector<long>& aInputPacketVector, const std::vector<long>&  aOutputPacketVector, int aOutputInterval) 
	: outputFormat(aOutputFormat), inputPacketVector(aInputPacketVector), outputPacketVector(aOutputPacketVector), outputInterval(aOutputInterval)
{
	// Set defaults
	setDefaults();
}

SimulinkConnector::SimulinkConnector(char* aOutputFormat, std::vector<long>& aInputPacketVector, const std::vector<long>&  aOutputPacketVector, int aOutputInterval, bool aEnable) 
	: outputFormat(aOutputFormat), inputPacketVector(aInputPacketVector), outputPacketVector(aOutputPacketVector), outputInterval(aOutputInterval)
{
	// Set defaults
	setDefaults();
	
	// Set enabled state
	enabled = aEnable;
}

void SimulinkConnector::setDefaults() {
	currentState = 0;
	totalStates = 2;
	updated = false;
	currentValuePosition = 0;
	lastUpdateTime = 0;
	enabled = true;
};

void SimulinkConnector::setPacketFormat(char* aOutputFormat) {
	outputFormat = aOutputFormat;
};

void SimulinkConnector::enable(bool aEnable) {
	enabled = aEnable;
};

bool SimulinkConnector::isUpdated() {
	if (!updated) {
		return false;
	}
	
	// There has been an update, reset and return
	bool retValue = updated; 
	updated = false;
	return retValue;
};

void SimulinkConnector::nextState() {
	currentState++;
	currentState %= totalStates;
};

void SimulinkConnector::update() {
	if (enabled) {
		// Check for incoming data
		checkIncoming();
	
		// Check output sample rate
		unsigned long now = millis();
		unsigned long timeChange = now - lastUpdateTime;
		if (timeChange >= outputInterval) {
			// Send packet
			sendPacket();
		
			// Update time
			lastUpdateTime = now;
		}
	}
}

void SimulinkConnector::checkIncoming() {
	
	switch(currentState) {
		case 0: 
			// Check for start byte
			while ( Serial.available()) {
				if (Serial.read() == '<') {					
					// Start bit found, switch to next state
					nextState();	
					break;
				}
			}			
			break;
		case 1: 
			// Get i-th position value, check if we've at least one value!
			while ( Serial.available() >= sizeof(long)) {
				// Insert the value in the data vector		
				long value = 0;				
				for (unsigned n = 0; n < sizeof( long ); n++) {
					value = (value << 8) + (unsigned char)Serial.read();
				}
				inputPacketVector[currentValuePosition] = value;
				
				// Update position
				currentValuePosition++;
				currentValuePosition %= inputPacketVector.size();
				
				// Check if we filled the data vector
				if (currentValuePosition == 0) {
					// Set update flag
					updated = true;
					
					// Update state
					nextState();
					
					// Break current loop
					break;
				}
			}
			break;
	}
};

void SimulinkConnector::sendPacket() {
	// loop through format string
	// Variable to track the amount of data-substitutions done; Needed to stay in the bounds of the data array;
	byte variablesInserted = 0; 
	char* startOutputFormat = outputFormat;
	
    for (; *outputFormat != 0; ++outputFormat) {
        if (*outputFormat == '%') {
            ++outputFormat;
            if (*outputFormat == '\0') break;
			
			// Print zero if no more data is available
			if (variablesInserted >= outputPacketVector.size()) {
				Serial.print(0L);
				continue;
			};

            if( *outputFormat == 'l' ) {
				// Long
				Serial.print((long)outputPacketVector[variablesInserted], DEC);
				variablesInserted++;
				continue;
			}
			
			if( *outputFormat == 'ul' ) {
				// Unsigned long
				Serial.print((unsigned long)outputPacketVector[variablesInserted], DEC);
				variablesInserted++;
				continue;
			}
			
			if( *outputFormat == 'u' ) {
				// Unsigned integer
				Serial.print((unsigned int)outputPacketVector[variablesInserted], DEC);
				variablesInserted++;
				continue;
			}
			
			if( *outputFormat == 'd' || *outputFormat == 'i') {
				// Integer
				Serial.print((int)outputPacketVector[variablesInserted], DEC);
				variablesInserted++;
				continue;
			}
        }
		// Output S, E or space
        Serial.print(*outputFormat);
	}
	Serial.print('\n');
	
	// Reset pointer location
	outputFormat = startOutputFormat;
};