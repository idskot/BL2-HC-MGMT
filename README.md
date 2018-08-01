# BL2-HC-MGMT
A little program for Borderlands 2 'Hardcore Mode'
Hardcore is a mode in which, on death, you throw weapons you currently have equipped away. This adds additional challenges by punishing your deaths. The various modes are based off real life dice rolls (Using a physical D20, D8, or D6, depending on which mode you wish to use.) You can either use specific equipment slots, or all of them. Such as only using the 4 weapon slots, or using the 4 weapon slots and the shield slot. Or, if you want to go all in, using the weapon slots (1st, 2nd, 3rd, or 4th slot), shield slot (5th slot) and grenade slot (6th slot), for a total of 6 pieces of equipment you can potentially lose for each death. The modes are detailed as followed:

1) Scaling Mode: This mode is split into three different sub-options which correspond to the difficulty you're playing at. The difficulties (in game) are Normal, True Vault Hunter (TVH), and Ultimate Vault Hunter (UVH). The idea is that you roll a die for each equipment slot you have (or want to use), and if the roll is below a certain threshold, you have to toss the item (The rules I play with are tossing them, and not selling them. But you can do whatever you'd like!) The breakdown of thresholds are as follows: Normal is 7 or lower (out of 20), True Vault Hunter is 4 or lower (out of 20), and Ultimate Vault Hunter is 2 or lower (out of 20). For added difficulty, you can obviously still use 'Normal' mode if you're playing in UVH mode. Just an idea!
2) Random Mode: Using this method, you "roll a die", and it selects one piece of equipment you have to toss upon each death. To translate this into physical die, if I only wanted to use my 4 equipment slots, I would roll a D8; and if it would say land on a 7, I would toss weapon #3. Or, if you're utilizing all 6 of the equipment slots you can roll a D6 and toss whichever equipment slot it would land on.
3) Pandemonium Mode: This is the most hardcore method! You basically flip a coin for EACH equipment slot. If it lands tails, you have to toss that item.
[BONUS] 4) Scorched Earth Mode: In this mode, you toss everything you have equipped upon death. This way is clearly incredibly difficult. But, it's easier on earlier levels. This mode isn't included in the program because there's no randomization necessary.

The program itself allows you to: 

1) Select number of equipment slots to "play with". 
2) Cycle through the 3 separate modes (Scaling, Random, and Pandemonium).
3) Displays which item(s) to throw away, and which to keep. 
4) Can repeat with out going through the options again. As such, you can select which equipment slots to use, and what mode/difficulty and just repeat upon each death.

Hardware:
1) Arduino (I specifically used a Nano, but from my understanding an Uno, Mini, Due, 101, Zero, Mega, Mega2560, MegaADK, Micro, Leonardo, or any other 328-based/32u4-based Arduino/AVR microcontrollers would work with out any editing. The only one that needs an edit, it seems, is the MKR-family of boards)
2) Monochrome 1.3" 128x64 OLED display - I2C communication (w/ built in driver utilizing u8g library) For the nano, the SDA connection originates from A4, and the SCL connection originates from A5. Check with your Arduino's pinout to make sure you connect the right wires.
3) 2 buttons (w/ pull down resistors, values don't really matter too much. They're just to stop a short circuit, I think I used 3.3k ohm resistors). Connect power to one side of the button, and on the other side connect both a wire to pins 2 or 3 (One button's wire goes to pin 2, and the other goes to pin 3), and a resistor to ground. The button connected to pin D2 will be the select button, and the button connected to pin D3 will be the start button.
