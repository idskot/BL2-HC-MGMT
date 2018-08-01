/*
 * Borderlands 2 Hardcore Manager
 * 
 * Hardware includes 128x64 OLED, Arduino (Nano), two buttons with pull-down resistors.
 * 
 * Functionality: Loop intro until button press, select # equipment slots,
 *        then select mode (Scaling, Random, Pandemonium). Repeat every button press
 *        Reset w/ start button. Repeat w/ select button. (Start -> Next state, Sel -> Alternate)
 * 
 * Connections:
 *   OLED:      SDA: A4
 *              SCL: A5
 *   Buttons:   Start: D2
 *              Selection: D3
 *              
 *  State Diagram:
 *              State 0: Intro
 *              State 1: Equipment Number Select
 *              State 2: Mode select (Scaling, Random, Pandemonium)
 *              State 3: Scaling intro - choose difficulty: 
 *                State 4: Scaling - Normal (Rand(0-20)>10)                   [* Terminal:: Start-> State = 0]
 *                State 5: Scaling - True Vault Hunter (TVH) (Rand(0-20)>7)         [* Terminal:: Start-> State = 0]
 *                State 6: Scaling - Ultimate Vault Hunter (UVH) (Rand(0-20)>4)   [* Terminal:: Start-> State = 0]
 *              State 7: Random (Rand(Equip#))                                [* Terminal:: Start-> State = 0]
 *              State 8: Pandemonium (For each equip, Rand(0-1))              [* Terminal:: Start-> State = 0]
 *              
 *   Programmer:  Scott Levine
 *                idskot@gmail.com
 *   
 *   Date: 7/31/2018
 */

#include <Wire.h> 
#include "U8glib.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0); // Set OLED

// Define button inputs
#define selButton 2
#define startButton 3

// Define interrupt delay
#define INTERRUPTDELAY 650        // in ms

// Initialize finite state machine @ state 0
int STATE = 0;
int SELSTATE = 0;
int EQUIPSLOTS = 0;

// start ISR to act as state incrementor
void startISR()
{
  // Interrupt debounce
  static unsigned long last_interrupt_time_start = 0;
  unsigned long interrupt_time_start = millis();

  if (interrupt_time_start - last_interrupt_time_start > INTERRUPTDELAY) // IFF time between interrupts isn't long enough, don't change
  {
    if(STATE == 2)               // Select Mode
    {
      if(SELSTATE == 0)          // Scaling Mode Intro
        STATE = 3;
      else if(SELSTATE == 1)     // Random Mode
        STATE = 7;
      else if(SELSTATE == 2)     // Pandemonium Mode
        STATE = 8;
      else                       // ERROR
        STATE = 0;
    }
    else if(STATE == 3)               // IFF in Scaling intro, choose next state
    {
      if(SELSTATE == 0)
          STATE = 4;      // IFF Sel = 0, Normal Mode
      else if(SELSTATE == 1)
          STATE = 5;      // IFF Sel = 1, TVH Mode
      else if(SELSTATE == 2)
          STATE = 6;      // IFF Sel = 2, UVH Mode
      else
          STATE = 0;     // IFF out of bounds, reset program
    }
  
    else if(STATE > 3)          // IFF at terminal state, terminate program
    {
      STATE = 0;
      SELSTATE = 1;
    }
    else                        // ELSIFF go to next state
    {
      STATE++;
    }

    last_interrupt_time_start = interrupt_time_start; // Store current interrupt time
  }
}

// Sel ISR which controls all sub-menu variations && state resets
void selISR()
{
  // Interrupt debounce
  static unsigned long last_interrupt_time_sel = 0;
  unsigned long interrupt_time_sel = millis();  

  if (interrupt_time_sel - last_interrupt_time_sel > INTERRUPTDELAY)    // IFF time between interrupts isn't long enough, don't change
  {
      // IFF equipment number select, use SELSTATE to determine quantity of slots, max 5
    if (STATE == 1 && SELSTATE<5)
    {
      SELSTATE++;
    } 
    else if(STATE == 1 && SELSTATE>=5)                       // IFF STATE = 1, and out of SELSTATE, reset SELSTATE
    {
      SELSTATE = 0;
    }
  
        // IFF Mode select, use SELSTATE to determine mode type, max 3 types
    if (STATE == 2 && SELSTATE<2)             // IFF in range, go to next option
    {
      SELSTATE++;
    }
    else if(STATE == 2 && SELSTATE>=2)                      // ELSIFF out of range, reset
    {
      SELSTATE = 0;
    }
  
      // IFF scaling intro, use SELSTATE to determine difficulty, max 3
    if (STATE == 3 && SELSTATE<2)             // IFF in range, go to next option
    {
      SELSTATE++;
    }
    else if(STATE == 3 && SELSTATE>=2)                      // ELSIFF out of range, reset
    {
      SELSTATE = 0;
    }

    if (STATE>3)                             // Utilize SELSTATE as a repeat function for terminal states
    {
      SELSTATE++;
    }

    last_interrupt_time_sel = interrupt_time_sel;   // Store current interrupt time
  }
}


// Prepare OLED for writing
void u8g_prepare(void) 
{
  u8g.setFont(u8g_font_6x10);
  u8g.setFontRefHeightExtendedText();
  u8g.setDefaultForegroundColor();
  u8g.setFontPosTop();
}

// Clear OLED screen
void clearOLED()
{
  u8g.firstPage();
  do{
  }while(u8g.nextPage());
}

// Repeat function for state 0 to show intro text
void intro()
{ 
  u8g.firstPage();
  do{
    u8g.setFont(u8g_font_unifont);
    u8g.drawStr(2, 19, "Welcome To");
    u8g.drawStr(2, 38, "Borderlands 2");
    u8g.drawStr(2, 57, "Hardcore mgmt");  
  }while(u8g.nextPage());
}

// Allow user to select number of slots in which to play with
void equipSelect()
{
  SELSTATE = 0;         // Reset Option #

  // Do loop which repeats so long as the state remains the same
  do{
    // Display process: First page routine, then do loop while nextPage routine
    u8g.firstPage();
    do
    {
      // Properly sets up display characteristics
      u8g_prepare();
      u8g.setFont(u8g_font_unifont);
      u8g.drawStr(2, 19, "Select Number");  // Print first line
      u8g.drawStr(2, 38, "of slots:");      // Print second line
        // Switch case which displays currently selected number of equipment slots && sets equipment slot value - which is global variable
      switch(SELSTATE)
        {
          case 0:   u8g.drawStr(2, 57, "1 Slot");  EQUIPSLOTS = 1; break;
          case 1:   u8g.drawStr(2, 57, "2 Slots"); EQUIPSLOTS = 2; break;
          case 2:   u8g.drawStr(2, 57, "3 Slots"); EQUIPSLOTS = 3; break;
          case 3:   u8g.drawStr(2, 57, "4 Slots"); EQUIPSLOTS = 4; break;
          case 4:   u8g.drawStr(2, 57, "5 Slots"); EQUIPSLOTS = 5; break;
          case 5:   u8g.drawStr(2, 57, "6 Slots"); EQUIPSLOTS = 6; break;
          default:  u8g.drawStr(2, 57, "ERR");     EQUIPSLOTS = 0; break;
        }
    }while(u8g.nextPage());
  }while(STATE == 1);
}

// Allows user to choose which mode to be played with. Cycles through options
void modeSelect()
{
  SELSTATE = 0;     // Reset option

  // Do loop which repeats so long as state remains the same
  do{
    // Display routine
    u8g.firstPage();
    do
    {
      u8g_prepare();
      u8g.setFont(u8g_font_unifont);
      u8g.drawStr(2, 19, "Select Mode");  // First line
      u8g.drawStr(2, 38, "Type:");        // Second line
        // Switch case which allows user to cycle through different mode options - SELSTATE is used in start ISR to determine next state
      switch(SELSTATE)
        {
          case 0:   u8g.drawStr(2, 57, "Sliding Scale"); break;   // Sliding scale -- predefined comparison amount per difficulty
          case 1:   u8g.drawStr(2, 57, "Random"); break;          // Random mode -- selects 1 random piece of equipment to throw away
          case 2:   u8g.drawStr(2, 57, "Pandemonium"); break;     // Each equipment slot has a 50% chance of getting thrown away
          default:  u8g.drawStr(2, 57, "ERR"); break;
        }
    }while(u8g.nextPage());
  }while(STATE == 2);
}

// Function that draws check -- to indicate to user to keep equipment in that slot
void u8gDrawCheck(int x, int y)
{
  u8g.drawLine(x, y-5, x+2, y);
  u8g.drawLine(x+2, y, x+6, y-10);
}
// Displays and allow user to choose difficulty wrt sliding scale
void scalingIntro()
{
  SELSTATE = 0;       // Reset option

  // Do while loop which repeats so long as state is the same  
  do{
    // Display routine
    u8g.firstPage();
    do
    {
      u8g_prepare();
      u8g.setFont(u8g_font_unifont);
      u8g.drawStr(2, 19, "Select Scaling");   // First row
      u8g.drawStr(2, 38, "Difficulty:");      // Second row
        // Switch case which cycles through the difficulties -- SELSTATE is used in start ISR to determine next state
      switch(SELSTATE)
      {
          case 0:   u8g.drawStr(2, 57, "Normal"); break;                // Normal mode - 7/20 chance to trash
          case 1:   u8g.drawStr(2, 57, "True Vault Hunter"); break;     // True Vault Hunter mode - 4/20 chance to trash
          case 2:   u8g.drawStr(2, 57, "Ult Vault Hunter"); break;      // Ult Vault Hunter mode - 2/20 chance to trash
          default:  u8g.drawStr(2, 57, "ERR"); break;
      }
    }while(u8g.nextPage());
  }while(STATE == 3);    
}


// Prints the results from the comparison for all scaling modes and Pandemonium mode (NOTE: Pandemonium mode functions the same as scaling, but with a 10/20 chance to trash equipment, vs the <7/20 from Normal)
void u8gPrintResults(int mode)
{
  // Initialize count for 'for' loop, compVal to compare random values for each equipment slot, and produce 6 random values in which to compare
  int count, compVal, randomValues[] = {random(20), random(20), random(20), random(20), random(20), random(20)};
  SELSTATE = 0;   // Reset Sel -- used to repeat
  u8g_prepare();
  u8g.setFont(u8g_font_unifont);

  u8g.firstPage();
  do
  {
    // Switch case which displays the first line to OLED, based on only function input && sets compVal based on mode
    // As per switch, Mode = 0 == Scaling - Normal, Mode = 1 == Scaling - VH, Mode = 2 == Scaling - TVH, && Mode = 3 == Pandemonium
    switch(mode)
    {
      case 0: u8g.drawStr(2, 19, "Scaling - Norm"); compVal = 7; break;     // Scaling - Norm: 7/20 chance to trash
      case 1: u8g.drawStr(2, 19, "Scaling - TVH"); compVal = 4; break;       // Scaling - TVH: 4/20 chance to trash
      case 2: u8g.drawStr(2, 19, "Scaling - UVH"); compVal = 2; break;      // Scaling - UVH: 2/20 chance to trash
      case 3: u8g.drawStr(2, 19, "Pandemonium"); compVal = 10; break;       // Pandemonium: 10/20 chance to trash
      default: u8g.drawStr(2, 19, "ERR"); compVal = 21; break;              // ERROR state: ALL trash (?)
      }

    // For loop which cycles through all possible equipment slots (EQUIPSLOTS), and displays number and result from random comparison
    for(count = 0; count < EQUIPSLOTS; count++)
    {
        // Case to print out number of equip slot -- determined by EQUIPSLOTS
      switch(count)
      {
          case 0: u8g.drawStr(2, 38, "1:"); break;
          case 1: u8g.drawStr(45, 38, "2:"); break;
          case 2: u8g.drawStr(88, 38, "3:"); break;
          case 3: u8g.drawStr(2, 57, "4:"); break;
          case 4: u8g.drawStr(45, 57, "5:"); break;
          case 5: u8g.drawStr(88, 57, "6:"); break;
      }
  
      // Actual comparison -- stored random values and defined value to compare based on mode
      if(count<3)   // First Row print
      {      
        if(randomValues[count]>=compVal)
        {
          u8gDrawCheck((17+(count*43)), 38);          // Keep (passed roll)
        }
        else
        {
          u8g.drawStr((17+(count*43)), 38, "X");      // Trash (failed roll)
        }      
      }
      else   // Second Row print
      {      
        if(randomValues[count]>=compVal)        
        {
          u8gDrawCheck((17+((count-3)*43)), 57);      // Keep (passed roll)
        }
        else
        {
          u8g.drawStr((17+((count-3)*43)), 57, "X");  // Trash (failed roll)
        }      
      } 
    }
  }while(u8g.nextPage());
}

// Function used for Scaling - Normal
void scalingNorm()
{
  SELSTATE = 0;     // Reset option
  
  // Do while loop which runs result display
  do{
    u8g.firstPage();
    do
    {
      u8gPrintResults(0);   // Input for u8gPrintResult is mode, defined in function
    }while(u8g.nextPage());

   // Do while loop which delays so long as SELSTATE == 0, SELSTATE == 1 causes function to run again and output new results
   do{
    delay(10);
   }while(SELSTATE==0);

   clearOLED();     // Clear OLED, just in case    
  }while(STATE == 4);      
}

// Function used for Scaling - True Vault Hunter (TVH) -- acts same as scalingNorm() except inputs different mode into u8gPrintResults
void scalingTVH()
{
  SELSTATE = 0; 
    
  do{
    u8g.firstPage();
    do
    {
      u8gPrintResults(1);
    }while(u8g.nextPage());
   
    do{
    delay(10);
   }while(SELSTATE==0);

   clearOLED();
    
  }while(STATE == 5); 
}

// Function used for Scaling - Ultimate Vault Hunter (UVH) -- acts same as scalingNorm() except inputs different mode into u8gPrintResult
void scalingUVH()
{
  SELSTATE = 0;
  
  do{
    u8g.firstPage();
    do
    {
      u8gPrintResults(2);
    }while(u8g.nextPage());
   
    do{
    delay(10);
   }while(SELSTATE==0);

   clearOLED();
    
  }while(STATE == 6); 
}

void randomMode()
{
  SELSTATE=0;
  int trash = random(EQUIPSLOTS)+1;
  enum {BufSize=9};
  char buf[BufSize];
  snprintf(buf, BufSize, "%d", trash);

  u8g.firstPage();
  do
  {
    u8g.drawStr(2, 19, "Random Mode");
    u8g.drawStr(2, 38, "Trash Equip Slot");
    u8g.drawStr(60, 57, buf);
  }while(u8g.nextPage());

  do
  {
    delay(10);
  }while(SELSTATE == 0);
}

// Function used for Pandemonium Mode -- acts same as scalingNorm() except inputs different mode into u8gPrintResults
void pandemoniumMode()
{
  SELSTATE = 0;
  
  do{
    u8g.firstPage();
    do
    {
      u8gPrintResults(3);
    }while(u8g.nextPage());
   
    do
    {
      delay(10);
    }while(SELSTATE == 0);    
  }while(STATE == 8); 
}

// Main setup function
void setup()
{
  #if defined(ARDUINO)
    pinMode(13, OUTPUT);           
    digitalWrite(13, HIGH);  
  #endif

  // Declares button inputs
  pinMode(startButton, INPUT_PULLUP);
  pinMode(selButton, INPUT_PULLUP);
  // Attaches interrupts to buttin inputs and declares ISR (Interrupt Service Routine)
  attachInterrupt(digitalPinToInterrupt(startButton), startISR, RISING);  
  attachInterrupt(digitalPinToInterrupt(selButton), selISR, RISING);
}


// Main loop function
void loop()
{
    // Runs function based on which STATE is current
    switch(STATE)
  {
    case 0:              // Intro
    {
      intro();
      break;
    }
    case 1:              // Equipment Select
    {
      equipSelect();
      break;
    }
    case 2:              // Mode Select
    {
      modeSelect();
      break;
    }
    case 3:              // Scaling Intro
    {
      scalingIntro();
      break;
    }
    case 4:              // Scaling - Normal
    {
      scalingNorm();
      break;
    }
    case 5:              // Scaling - TVH
    {
      scalingTVH();
      break;
    }
    case 6:               // Scaling - UVH
    {
      scalingUVH();
      break;
    }
    case 7:               // Random Mode
    {
      randomMode();
      break;
    }
    case 8:               // Pandemonium Mode
    {
      pandemoniumMode();
      break;
    }
    default:              // Error mode -- reset to STATE == 0
    {
      STATE = 0;
      break;
    }
  }

  delay(100);             // Delay to slow some things down
}
