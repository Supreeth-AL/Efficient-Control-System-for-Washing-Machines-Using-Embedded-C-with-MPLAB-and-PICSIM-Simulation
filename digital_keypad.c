#include <xc.h>
#include "digital_keypad.h"

void init_digital_keypad(void)
{
    /* Set Keypad Port as input */
    KEYPAD_PORT_DDR = KEYPAD_PORT_DDR | INPUT_LINES;
}

unsigned char read_digital_keypad(unsigned char mode)
{
    static unsigned char once = 1;
    static unsigned char longpress;
    static unsigned char prev_key;
    
    if (mode == LEVEL_DETECTION)
    {
        return KEYPAD_PORT & INPUT_LINES;
    }
    else
    {
        if (((KEYPAD_PORT & INPUT_LINES) != ALL_RELEASED) && once)
        {
            once = 0;
            longpress =0;
            prev_key = KEYPAD_PORT & INPUT_LINES;
        }
        
        // for next key press,  comppare current key and prev_key
        else if (!once && (prev_key ==( KEYPAD_PORT & INPUT_LINES))  && longpress < 30)
        {
            longpress++; 
        }
        else if (longpress == 30)
        {
            longpress++;
            // return long press value 
            return prev_key | 0x80; //0011 0111 | 1000 0000 -> 1011 0111 -> 0xB7
            
        }
        else if (((KEYPAD_PORT & INPUT_LINES) == ALL_RELEASED) && ! once) // check if key was pressed 
        {
            once = 1;
            if (longpress <30) // IF key is not pressed for long time 
            {
                return prev_key;
            }
        }
    }
    
    return ALL_RELEASED;
}
