#ifndef __SIMULINKCONNECTOR_H__
#define __SIMULINKCONNECTOR_H__

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <iterator> // The iterator has to be included if you want to use <vector>
#include <vector>
#include <climits>

/**
 * Provides an interface between simulink and Arduino. 
 * 
 * Dependencies:
 * STL library: iterator, vector and climits. See https://github.com/maniacbug/StandardCplusplus
 *
 * Usage example (check function specific descriptions for extra information):
 * Initialization: 
 * 	std::vector<long> outputPacketVector(5,0);
 *	std::vector<long> receivedPacketVector(5,0);  // We expect 5 values from simulink
 *	SimulinkConnector simulinkConnection("S %l %l %l %l %l E", receivedPacketVector, outputPacketVector, 20);
 * 
 * Arduino loop:
 * simulinkConnection.update();
 *
 * Set output variables (at any point in the loop or supporting function):
 * outputPacketVector[0] = 99;
 *
 * Read input variables (this is always accesible after initialization):
 * int incomingValue = receivedPacketVector[2]   // Read third value of incoming data vector
 * 
 * <p>
 * The simulink->arduino protocol is defined as follows:
 * "<Start bit><Data><Stop bit>"
 * The start bit always consists of: '<'[char] / 60[DEC] (8 bits).
 * The end bit always consists of: '>'[char] / 62[DEC] (8 bits).
 *
 * Currently CRC checks have not been implemented. This is is mainly because this library is meant to be used with USB serial which has its own CRC as well.
 * 
 * <p>
 * More information on serial connections:
 * http://www.pjrc.com/teensy/td_serial.html
 * http://eopossum.blogspot.nl/2011/11/processing-simple-osc-messages-on.html
 * http://my.safaribooksonline.com/book/hobbies/9781449399368/serial-communications/sending_binary_values_from_processing
 * http://leaflabs.com/docs/lang/cpp/bitshift.html
 * http://stackoverflow.com/questions/15869609/arduino-trouble-with-serial-readbytes-when-reading-from-a-java-program-over-us
 * http://www.gammon.com.au/forum/?id=11425
 * 
 * @author      Wilbert van de Ridder <l.w.vanderidder @ student.utwente.nl>
 * @version     1.1
 * @since       2013-07-07
 */
class SimulinkConnector {		
	public:	
		/**
		 * Constructor
		 *
		 * The output format accepts the following types:
		 * l = long
		 * ul = unsigned long
		 * u = unsigned int
		 * i or d = int
		 * Multiple datatypes can be used at once, however, 
		 * the sendPacket() function only accepts long types. 
		 * You can use casting to transmit the other datatypes.
		 * 
		 * @param aOutputFormat char string containing the output format; e.g. "S %d %l %ul E".
		 * @param aInputPacketVector vector to which serial input will be written
		 * @param aOutputPacketVector vector from which output serial is composed
		 * @param aOutputInterval sample time of serial output in milliseconds
		 */
		SimulinkConnector(char* aOutputFormat, std::vector<long>& aInputPacketVector, const std::vector<long>&  aOutputPacketVector, int aOutputInterval);
		
		/**
		 * Constructor
		 *
		 * The output format accepts the following types:
		 * l = long
		 * ul = unsigned long
		 * u = unsigned int
		 * i or d = int
		 * Multiple datatypes can be used at once, however, 
		 * the sendPacket() function only accepts long types. 
		 * You can use casting to transmit the other datatypes.
		 * 
		 * @param aOutputFormat char string containing the output format; e.g. "S %d %l %ul E".
		 * @param aInputPacketVector vector to which serial input will be written
		 * @param aOutputPacketVector vector from which output serial is composed
		 * @param aOutputInterval sample time of serial output in milliseconds
		 * @param aEnabled boolean to indicate whether the connection is active
		 */
		SimulinkConnector(char* aOutputFormat, std::vector<long>& aInputPacketVector, const std::vector<long>&  aOutputPacketVector, int aOutputInterval, bool aEnabled);
		
		/**
		 * Updates/changes the output serial packet format.
		 * 
		 * @param aOutputFormat string containing the output format;
		 */
		void setPacketFormat(char* aOutputFormat);
	
		/**
		 * Check if new serial input has been received
		 * 
		 * @return boolean
		 */	
		bool isUpdated();
		
		/**
		 * Update the simulink connection. Checks for incoming messages and sends output message if sample time interval has passed.
		 * 
		 * @return boolean
		 */	
		void update();
		
		/**
		 * Enable or disable the current connection.
		 * 
		 * @param bool to enable/disable
		 */	
		void enable(bool);
		
	private:
		/** 
		 * The serial output format; e.g. "S %d %d %d E"
		 */
		char* outputFormat;
		
		/**
		 * Keeps track of the current state. Currently these are defined:
		 * 0. Wait for start bit
		 * 1. Receiving data
		 * 2. Checking end bit
		 * 3. CRC check (not used at the moment)
		 */
		byte currentState;
		
		/**
		 * Checks for incoming messages
		 */	
		void checkIncoming();
		
		/**
		 * Transition to next packet read state
		 */	
		void nextState();
		
		/**
		 * Sends a data packet using serial output in the format defined earlier.
		 * 
		 * @see SimulinkConnector()
		 * @see setPacketFormat()
		 * @see outputFormat
		 */
		void sendPacket();
		
		/**
		 * Helper function to set default values. To be called from any of the constructors.
		 */	
		void setDefaults();
		
		/**
		 * Keep track of available amount of states
		 */	
		byte totalStates;
		
		/**
		 * boolean to store new incoming message available indication
		 */	
		bool updated;
		
		/**
		 * value to keep track of what index the currently receiving value has in the output vector
		 */	
		byte currentValuePosition;
		
		/**
		 * Sample time of output messages in milliseconds
		 */	
		int outputInterval;
		
		/**
		 * Save timestamp of last update
		 */	
		long lastUpdateTime;
		
		/**
		 * Reference vector of received values
		 */	
		std::vector<long>& inputPacketVector;
		
		/**
		 * Reference vector of output values
		 */	
		const std::vector<long>& outputPacketVector;
		
		/**
		 * Boolean to track whether the connection is enabled
		 */	
		bool enabled;
};
#endif
