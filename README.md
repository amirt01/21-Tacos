# 21-Tacos
Our project for ME 235 is to build an automatic bycicle gear shifter we call SmartCycle.
## Cadence
Here you will find arduino files for the cadence esp-32 which is attatched to the bycicle pedal arm. This esp will be sending telemetry data to the main esp board using the ESP-NOW protocol. 

## Communications
Here is where our protobuffer header files are. Protobuffers are useful for reducing the number of bytes being sent over the wire and formalizing the package structure. Our main board communicates to our GUI app using protobuffers.

To generate the Python protobuffer file, run this command from the project directory:
```shell
protoc -I="." --python_out=. --pyi_out=. Communications\SmartCycle.proto
```

## Simulation
This is our folder for basic scripts and other simulated research such as how to handle the reed switch and how to pick the optimal gear. 

## SmartCycle
This is our main folder for project. In it you will find a PlatformIO folder structure setup to process the protobuffer file and handle all major functionallity. Additionally, there are several testing files for developing subsystems.

### ButtonHandler
This class is designed to, as the name suggests, handle a button. It provides an interface for the user to quickly ask if the button is pressed or not without having to worry themselves with debouncing. The ButtonHandler class employs a GPIO interrupt to detect with the button pin changes and then starts a software timer to wait for the button bounce to stop before reading from the pin.  

### GroundEstimator


### Shifter


### SmartCycleServer


## SmartCycleMonitor
Here is our python GUI for displaying the SmartCycle system state and tuning parameters remotely.