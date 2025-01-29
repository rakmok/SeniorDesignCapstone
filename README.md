# SeniorDesignCapstone

This is the repository to record and motivate our capstone project for ECE 445: Senior Design in the Spring of 2025 titled PawFeast: Food on Demand. The group members are Arash Amiri, Kathryn Thompson, and Omkar Kulkarni.

The project entails the analysis, design, and build of a pet food dispensory system. The initial features of this charger will include laptop-to-laptop charging capability, battery health monitoring, and the ability to set a battery percentage value for the device to charge up to. In short, this will be a bi-directional low-voltage DC-DC converter with control and communications sub-systems. Additionally, the charger will include a display that would list certain values like the voltage or battery charge.

# Problem

All pet owners must remember to feed their pets at set times during the day. There are times that people forget to feed their pets, double feed them when there are multiple people in the house and there is poor communication, or have trouble feeding them on time when they get home late if there are prior conflicts. In these times, pets either eat too much, too little, or irregularly. As a result, timer-based pet feeders have been created that release food at set times. However, this introduces a new problem, food that was sealed in an airtight container is now released into an open environment. When this food sits out for extended periods of time, this risks the food becoming stale or bugs getting introduced to the food.

# Solution

We are seeking to solve this problem by having timers preventing overeating, coupled with pressure sensors required to release food. Given both of these conditions, the dispenser will release the food for the pet. This ensures that the food is fresh when the pet goes to eat and that the pet is fed on time. In addition, if the pet were to not eat all the food at once and leave the food dispenser, the pressure sensor will tell the dispenser to cover the food until the pet returns ensuring freshness and preventing bugs. In addition, our system will notify owners when the pet has eaten, or when the food dispenser has low levels of food.

# Solution Components

## Subsystem 1: Refilling Food Store

The subsystem for checking if the food store is empty will include an infrared presence sensor (Vishay TSSP77038) that is able to measure up to 2 meters in front of it to determine whether an object is present—this will serve to detect whether the food inside of the container is low and needs to be refilled. When the food store reaches a certain level, a signal will be sent to subsystem 7 (the brain), which will send an automated message to notify the owner to refill the food store.

## Subsystem 2: Power System

The power system will supply energy to all of the various subsystems, including the microcontroller of subsystem 7, the stepper motor driver in subsystem 3, and the various sensors in subsystems 1, 5, and 6. The low voltage buses will be 3.3V, 12V, and 5V, all of which will be powered by a combination of rechargeable lithium-ion batteries and low-dropout regulators. This compact, custom battery pack will allow the pet dispenser to be portable, even for family vacations or road trips. The AMS1117 LDO has variable voltage levels of 1.2V, 1.5V, 3.3V, 5V, and more for all of the lower voltage needs; the 12V supply will come directly from the battery pack or possibly from a DC-DC converter that is implemented.

## Subsystem 3: Food Dispenser

This subsystem will utilize a E Series Nema 17 stepper motor that will slide open a door to the food container when needed and allow a set amount of food through before sliding closed the door. The motor will be controlled by a stepper motor controller IC (DRV8825) with a custom control circuit that takes in 12V and outputs 3.4V to the motor. Hard cut offs will also be coded for maximum food and min and max on times during the day.

The main objective of this subsystem is to make the system an on-demand food system. We will utilize a large button for the dog to step on when hungry. When pressed, the system will start the motors to release food, on the condition that enough time has elapsed since it ate last. The pet would be trained how to ask for food using this button. Only when the button is pressed and enough time has passed, will food then be dispensed.

## Subsystem 4: RFID for pet identification

We will have receivers for minimum 2 RFIDs, UHF RFID tag 9662 Long Distance Passive Alien H3. The RFIDs would be used to signal which pet is using the dispenser, so the owner is able to utilize the same dispenser for one or more pets.

## Subsystem 5: User Interface

The subsystem for notifications will be a phone application that we will build that is able to connect with the pet feed dispenser system. For the application side, we will utilize React Native and Expo to create a user friendly method to check the pet’s eating habits. This will notify users for when the pet has been fed, if the food tank is low and requires refiling, and if the food was covered due to an empty bowl or a partially filled one.

## Subsystem 6: Brain

The brain subsystem will take in inputs from all of the other sensor subsystems and output the according signals to the user interface (for notifications) and the food dispenser motor driver. This system will also track the amount of time that has passed and sound a soft chime for when enough time has passed for the pet to be able to eat. The ESP32 microcontroller, known for its wifi connectivity and security measures, would have a set of I/O pins for taking in these signals. This MCU programmer circuit and other control level circuitry—firmware or equivalent—would be incorporated onto this board along with a UART circuit to detect pet RFIDs (done with the UHF RFID reader JRD-4035).

# Criterion For Success

The mechanical feeder system should drop a bowl-full of food once a valid RFID is nearby, a pet-intended button has been pressed, AND enough time has passed since the pet last ate. An owner should receive a notification once the food store has decreased below 10% and 20%, and when criterion 1 has been satisfied. The power system should be power efficient: the rechargeable battery should last for at least 10 food dispenses.

# Results

TBA

# Schematic

TBA

# Figures

TBA

# Demo

TBA
