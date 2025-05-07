# SeniorDesignCapstone

This is the repository to record and motivate our capstone project for ECE 445: Senior Design in the Spring of 2025 titled PawFeast: Food on Demand. The group members are Arash Amiri, Kathryn Thompson, and Omkar Kulkarni.

The project entails the analysis, design, and build of a pet food dispensory system. If you wish to see each member's notebook, you can switch branches in github.com by clicking main or run git checkout \[member_first_name]-notebook. Each member hosts their own notebook on a different branch. The Hardware folder contains all of our KiCAD boards and other libraries. The images folder is only meant for displaying images to the README and the Assignments folder includes all of our important documents.

# Problem

All household pet owners must remember to feed their pets—whether they are birds, guinea pigs, rabbits, dogs, or cats—at set periods during the day. But there can be times when people forget to feed their pets, double feed them, or have trouble feeding them on schedule. This can be due to poor communication between family members or even occur when pet owners come home late from work or prior commitments. During these times, pets either overeat, go hungry, or eat irregularly. As a result, timer-based pet feeders have been developed to release food at set intervals. When food sits out for extended periods of time, the risk of the food becoming stale or getting infected with bugs increases drastically.

If an insect is near your food, it is a common reaction to remove or swat it away, but that mechanism is not present for the pet’s food bowl, especially when the family is not at home. Another issue is that smarter pets can nose into the food storage and overeat or eat something that their bodies will reject (like chocolate for dogs). With a set schedule to limit food dispensation throughout the day, the pet will not take advantage of either a careless overpour of food or of an unmanned food station. Furthermore, data on how much food was dispensed and eaten will help track and optimize the pet’s eating habits.

# Solution

We are seeking to solve this problem by having timers coupled with a button and RFID component to release food—thus preventing overeating. Given all of these conditions, the dispenser will release the pet food. This ensures that the pet is not only fed on time, but that the food is not polluted when the pet goes to eat; additionally, a chime will go off signaling to the pet that it is time for them to eat. We will incorporate a notification system to alert the owner(s) whether the pet has eaten or when the food store has low levels of food.

If the pet were to not eat all the food at once and leave the food dispenser, the pressure sensor will tell the dispenser to cover the food until the pet returns ensuring freshness and preventing bugs.

# Solution Components

## Subsystem 1: Refilling Food Store

The subsystem for checking if the food store is empty will include a through-beam optical sensor that is able to determine whether an object is present—this will serve to detect whether the food inside of the container is low and needs to be refilled. When the food store reaches a certain level, a signal will be sent to subsystem 6 (the brain), which will send an automated message to notify the owner to refill the food store.

## Subsystem 2: Power System

The power system will supply energy to all of the various subsystems, including the microcontroller of subsystem 6 (the brain), the stepper motor driver in subsystem 3, and the various sensors in subsystems 1 (refilling food) and 3 (food dispenser). The low voltage buses will be 3.3V, 12V, and 5V, all of which will be powered by a combination of rechargeable lithium-ion batteries with voltage regulators and buck converters. This compact, custom 3s2p battery pack will allow the pet dispenser to be portable, even for family vacations or road trips—more importantly, it will provide a steady current, even when the pack voltage drops (unlike standard AA/AAA cells). The 12V to 3.3V regulator chip will be Monolithic Power Systems’s MP2315SGJ-Z, which can take a 12V input and output up to 2.5A [4.7]. The 12V to 5V regulator chip will be Monolithic Power Systems’s MP2338GTL-Z [4.6]. The 12V input into the motor driver IC circuit will come straight from the battery pack. There will also be a backup supply voltage of 5V from a typical barrel jack wall-outlet, integrated into the brain board. This will implement the AMS1117 LDO, which is perfect for 5V to 3.3V power conversion. Just to be clear, this 5V barrel-jack addition will just be included on the brain board as a backup power subcircuit in case of power shutdown.

## Subsystem 3: Food Dispenser

This subsystem will utilize a E Series Nema 17 stepper motor that will rotate a door to the food container when needed and allow a set amount of food through before rotating again to close the door [4.8]. The motor will be controlled by a stepper motor controller IC (DRV8825) with a custom control circuit that takes in 12V and outputs 3.4V to the motor. We will define hard cut offs that will be coded for maximum amount of food to be dispensed and specify times at which food may be dispensed during the day.

The main objective of this subsystem is to make the system an on-demand food system. We will utilize a large button for the dog to step on when hungry. When pressed, the system will start the motors to release food, on the condition that enough time has elapsed since it ate last. The pet would be trained how to ask for food using this button. Only when the button is pressed and enough time has passed, will food then be dispensed.

## Subsystem 4: RFID for pet identification

We will have a receiver for a minimum of 2 HF RFID tags 9662 Long Distance Passive Alien H3. We will use the RC522 RFID chip for reading and writing to the RFID chips. The RFIDs would be used to signal which pet is using the dispenser, so the owner is able to utilize the same dispenser for one or more pets.

## Subsystem 5: User Interface

The subsystem for notifications will be a web application that we will build that is able to connect with the pet feed dispenser system. For the application side, we will utilize Bottle for our backend, SQLite for our database, and html pages for our frontend to create a user friendly method to check the pet’s eating habits. This will notify users for when the pet has been fed, if the food tank is low and requires refiling, and if the food was covered due to an empty bowl or a partially filled one.

## Subsystem 6: Brain

The brain subsystem will take in inputs from all of the other sensor subsystems and output the according signals to the user interface (for notifications) and the food dispenser motor driver. This system will also track the amount of time that has passed and sound a soft chime for when enough time has passed for the pet to be able to eat. The ESP32 WROOM-32E-4N microcontroller, known for its wifi connectivity and security measures, would have a set of I/O pins for taking in these signals. This MCU programmer circuit and other control level circuitry—firmware or equivalent—would be incorporated onto this board along with a UART circuit to detect pet RFIDs (done with the UHF RFID reader JRD-4035).

# High Level Requirements List

For this project to be considered successful, our project must meet the following requirements:

- The **Pet Food Dispenser** must **not** dispense food before user preset times, such as **10 AM and 6 PM, daily**, with a **10-minute tolerance**.
- The **Pet Food Dispenser** must dispense food into the **designated food bowl** upon detecting the **presence of the pet** via the **button** and the **2+ RFID tags** on the pet’s collar **within 2 minutes**.
- The **user interface** must update **within 5 minutes** of an event on the application we create, detailing:
  - The pet’s **feeding status**
  - **Low food store** notifications at **20% & 10%**
  - **Unfinished meals**

# Results

![Final Product](images/IMG_7445.jpg)

# Demo

Here is the link to our demonstration video:
[Demo](https://www.youtube.com/embed/mT6wlLyjihs?si=MdiL9m5ufs3mbADt)  
![Post Presentation Picture](Images/IMG_0642.jpg)
