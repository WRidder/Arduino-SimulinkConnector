# Arduino Simulink serial connection library
The simulink serial connection library has been created due to both the lack of a standard approach and as a training exercise
to learn about serial communication protocols.

## Usage
```cpp
#include "SimulinkConnector.h"

/**
 * Initialization
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
std::vector<long> outputPacketVector(5,0);
// Configured an output of 5 long values from Matlab Simulink
std::vector<long> receivedPacketVector(5,0);
SimulinkConnector simulinkConnection("S %l %l %l %l %l E", receivedPacketVector, outputPacketVector, 20);

/**
 * Usage
 */
// Arduino loop
simulinkConnection.update();

// Set output variables (at any point in the loop or supporting function):
outputPacketVector[0] = 99;

// Read input variables (this is always accessible after initialization):
int incomingValue = receivedPacketVector[2]   // Read third value of incoming data vector
```

## Author
This library has been developed by [Wilbert van de Ridder](http://www.github.com/WRidder) for a BSc assignment at the [University of Twente](http://www.utwente.nl).