# 21-Tacos
Our project for ME 235 is to build an automatic bicycle gear shifter we call SmartCycle.

## Cadence
Here, you will find Arduino files for the cadence esp-32, which is attached to the bicycle pedal arm. This ESP will send telemetry data to the main ESP board using the ESP-NOW protocol. 

## Communications
Here is where our protobuffer header files are. Protobuffers help reduce the number of bytes sent over the TCP connection and formalize the package structure. Our main board communicates to our GUI app using protobuffers.

## Simulation
This is our folder for basic scripts and other simulated research, such as handling the reed switch and picking the optimal gear. 

## SmartCycle
This is our main project folder. You will find a PlatformIO folder structure to process the protobuffer file and handle all primary functionality. Additionally, there are several testing files for developing subsystems.

## SmartCycleMonitor
Here is our Python GUI for displaying the SmartCycle system state and tuning parameters remotely.
