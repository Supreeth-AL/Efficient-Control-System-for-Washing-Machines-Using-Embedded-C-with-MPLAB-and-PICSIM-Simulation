/*
 * File:   main.c
 * Author: supreeth
 *
 * Created on 20 March, 2024, 10:35 AM
 */


#include <xc.h>
#include "main.h"
#include "digital_keypad.h"
#include "timers.h"
#include "clcd.h"
#pragma config WDTE = OFF 

void init_config(void)
{
    /* initialies the digital keypad*/
    init_digital_keypad();
    
    /*initialies the_clcd*/
    init_clcd();
    
    /* CONGI RC2 AS THE OUTPUT PIN*/
    FAN_DDR = 0;
    
    // CONFIG RC1 AS OUTPUT
    BUZZER_DDR =0;
    BUZZER = OFF;
    
     init_timer2();
    
    PEIE =1;
    GIE =1;
    
    /**/
    
}
/*initially operation mode washing program*/
unsigned char operation_mode = WASHING_PROGRAM_DISPALY ;
unsigned char reset_mode = WASHING_PROGRAM_DISPALY_RESET ; 
unsigned char program_no = 0,water_level_index = 0; 
unsigned char *washing_prog[] = {"Deily", "Heavy", "Delicates", "whites", "Stainwash", "Ecocottons", "Woollens", "Bedsheets", "Rinse+dry", "Dry only", "wash only ", "Aqua store" };
unsigned char *water_level_option[] = {"Auto","Low","Medium","High","Max"};
unsigned char min,sec;
unsigned int rinse_time, wash_time, spin_time; 
void main(void) 
{
    init_config();
    unsigned char key = read_digital_keypad(STATE);
    
    /*display the keypad status*/
    clcd_print("Press Key5 To",LINE1(1));
    clcd_print("Power ON ",LINE2(4));
    clcd_print("Washing Machine",LINE3(1));
    
    /*keep waiting till switches is pressed */
      while(read_digital_keypad(STATE) != SW5)
      {
          for(unsigned int wait = 3000; wait--;);
      }
    
    /*power on screen*/
     power_on_screen();
    
    while (1)
    {
        
        key = read_digital_keypad(STATE); //SW5 ,SW6 .... ALL_RELEASED
        for(unsigned char wait = 100; wait--;);
        
        // if SW4 is pressed for long time if the screen is washing mode
        if(key == LSW4 && operation_mode == WASHING_PROGRAM_DISPALY )
        {
            operation_mode = WATER_LEVEL;
            reset_mode =  WATER_LEVEL_RESET;
            
        }
        // if SW4 is pressed for long time , operation mode is water level screen 
        else if (key == LSW4 &&  operation_mode ==  WATER_LEVEL )
        {
            set_time();
            // change operation  mode 
            operation_mode = START_STOP_SCREEN;
            clear_screen();
            
            clcd_print("Press Switch",LINE1(1));
            clcd_print("SW5: Start",LINE2(1));
            clcd_print("SW6: Stop",LINE3(1));
            
        }
        
        //operation_mode is pause sw5 is pressed 
        if(key == SW5 && operation_mode == PAUSE)
        {
            TMR2ON = 1;
            FAN = ON;
            operation_mode = START_PROGRAM ;
        }
        
        switch(operation_mode)
        {
            case WASHING_PROGRAM_DISPALY:
                washing_program_dispaly(key);
                break;
                
            case WATER_LEVEL:
                water_level_display(key);
                break;
            case  START_STOP_SCREEN:
                if(key == SW5)
                {
                    operation_mode= START_PROGRAM ;
                    reset_mode = START_PROGRAM_RESET ;
                    continue;
                }
                else if (key == SW6)
                {
                    operation_mode = WASHING_PROGRAM_DISPALY ;
                    reset_mode = WASHING_PROGRAM_DISPALY_RESET ; 
                    continue;
                }
                break;
            case START_PROGRAM:
               run_program(key);
                break;
        }
        reset_mode = RESET_NOTHING;
    }
    return;
}

void  power_on_screen(void)
{
    /*to print block*/
    for(unsigned char i=0; i<16; i++)
    {
        clcd_putch(BLOCK,LINE1(i));
    }
    clcd_print("Powering on ",LINE2(2));
    clcd_print("Washing Machine",LINE3(1));
     for(unsigned char i=0; i<16; i++)
    {
        clcd_putch(BLOCK,LINE4(i));
    }
    
     __delay_ms(1000);
     
     clear_screen();
}

void washing_program_dispaly(unsigned char key)
{
    /*CODE TO BE ONLY FIRST TIME WHEN FUCTION CALLED */
    if(reset_mode == WASHING_PROGRAM_DISPALY_RESET)
    {
        clear_screen();
        program_no = 0;
        
    }
    
    /*  to check if sw4 if pressed increment the index of program*/
    if( key == SW4 )
    {
        program_no++;
        clear_screen();
        if(program_no == 12)
        {
            program_no = 0;
        }
    }
    
    clcd_print("Washing Program",LINE1(0));
    clcd_putch('*',LINE2(0));
    
    /*program 12 daily washing-prog[], heavy ......*/
    /* program_no = 0 -> daliy,heavy,delicates */
    /* program_no =1 -> havey,delicates,whites*/
    if(program_no <= 9)
    {
       
    clcd_print( washing_prog[program_no],LINE2(2));
    clcd_print( washing_prog[program_no + 1],LINE3(2));
    clcd_print( washing_prog[program_no + 2],LINE4(2));
    }
    // wash only , aquastroe , daily , 
    else if (program_no == 10)
    {
    clcd_print( washing_prog[program_no],LINE2(2));
    clcd_print( washing_prog[program_no + 1],LINE3(2));
    clcd_print( washing_prog[0],LINE4(2));
    }
    // aquastore , daily, heavy
    else if (program_no == 11)
    {
    clcd_print( washing_prog[program_no],LINE2(2));
    clcd_print( washing_prog[0],LINE3(2));
    clcd_print( washing_prog[1],LINE4(2));
    }
    
    
}

void water_level_display(unsigned char key)
{
    if(reset_mode == WATER_LEVEL_RESET)
    {
        water_level_index = 0;
        clear_screen();
        
    }
    if(key == SW4 )
    {
        water_level_index++;
        if(water_level_index == 5)
        {
                    water_level_index = 0;

        }
        clear_screen();
        //increment the water level option 
    }
    
    //print the option based on sw press
    clcd_print("Water level:",LINE1(0));
    clcd_putch('*',LINE2(0));
    
    //Auto Low Medium 
    //Low Medium High
    //medium high max
    //high max auto
    //max auto low
    
    if(water_level_index <= 2)
    {
            clcd_print(water_level_option[water_level_index],LINE2(2));
            clcd_print(water_level_option[water_level_index + 1],LINE3(2));
            clcd_print(water_level_option[water_level_index + 2],LINE4(2));
    }
    else if (water_level_index ==3)
    {
        clcd_print(water_level_option[water_level_index],LINE2(2));
        clcd_print(water_level_option[water_level_index + 1],LINE3(2));
        clcd_print(water_level_option[0],LINE4(2));
    }
     else if (water_level_index ==4)
    {
        clcd_print(water_level_option[water_level_index],LINE2(2));
        clcd_print(water_level_option[0],LINE3(2));
        clcd_print(water_level_option[1],LINE4(2));
    }
    
}

void set_time(void)
{
    switch(program_no)
    {
        // daliy
        case 0:
            switch(water_level_index)
            {
                //low
                case 1:
                    sec =33;
                    min =0;
                    break;
                    // auto, medium
                case 0:
                case 2:
                    sec = 41;
                    min =0;
                    break;
                    //high, max
                case 3:
                case 4:
                    sec =45;
                    min =0;
                    break;
            }
            break;
            //heavy
        case 1:
        {
            switch(water_level_index)
            {
                //low
                case 1:
                    sec =43;
                    min =0;
                    break;
                    // auto, medium
                case 0:
                case 2:
                    sec = 50;
                    min =0;
                    break;
                    //high, max
                case 3:
                case 4:
                    sec =57;
                    min =0;
                    break;
            }
            break;
        }   
            
        case 2:
        {
            switch(water_level_index)
            {
                //low, medium, auto
                case 1:
                case 0:
                case 2:
                    sec = 26;
                    min =0;
                    break;
                    //high, max
                case 3:
                case 4:
                    sec =31;
                    min =0;
                    break;
            }
        }
        //whites
        case 3:
            {
                sec =16;
                min = 1;
                break;
            }
            break;
            
          // satinwash  
        case 4:
            {
                sec =36;
                min = 1;
                break;
            }
        // ecocotton
        case 5:
            {
                sec =36;
                min = 0;
                break;
            }
        // woolens
        case 6:
            {
                sec =29;
                min = 0;
                break;
            }
        
        //bedsheets
        case 7:
            {
                switch(water_level_index)
                {
                    //low
                    case 1:
                        sec =46;
                        min =0;
                        break;
                    // auto, medium
                    case 0:
                    case 2:
                        sec = 53;
                        min =0;
                        break;
                        //high, max
                    case 3:
                    case 4:
                        sec =00;
                        min =1;
                        break;
                }
                break;
            }
        case 8:  // rinse and dry
        {
            switch(water_level_index)
                {
                    //low
                    case 1:
                        sec =18;
                        min =0;
                        break;
                    // auto, medium
                    case 0:
                    case 2:
                        //high, max
                    case 3:
                    case 4:
                        sec =20;
                        min =0;
                        break;
                }
                break;
        }
        case 9:  //dry
        {
           
                sec =6;
                min = 0;
                break; 
        }
        case 10:
        case 11: //wash only // aqua store
        {
            switch(water_level_index)
                {
                    //low
                    case 1:
                        sec =16;
                        min =0;
                        break;
                    // auto, medium
                    case 0:
                    case 2:
                        sec =21;
                        min =0;
                        break;
                        //high, max
                    case 3:
                    case 4:
                        sec =26;
                        min =0;
                        break;
                }
                break;
        }
    }
}
    
void run_program(unsigned char key)
{
    door_status_check();
    static int total_time, time;
    if (reset_mode == START_PROGRAM_RESET )
    {
        clear_screen();
        clcd_print("Prog: ",LINE1(0));
        clcd_print(washing_prog[program_no], LINE1(6));
        clcd_print("Time: ",LINE2(0));
        
        
        //DISPLAY time taken by the function in sec nd min
        clcd_putch((min/10)+'0',LINE2(6));
        clcd_putch((min%10)+'0',LINE2(7));
        clcd_putch(':',LINE2(8));
        clcd_putch((sec/10)+'0',LINE2(9));
        clcd_putch((sec%10)+'0',LINE2(10));
        clcd_print("(MM:SS)",LINE3(5));
        
        __delay_ms(2000);
        clear_screen();
        clcd_print("Function - ", LINE1(0));
        clcd_print("Time: ",LINE2(0));
        clcd_print("5-START 6-PAUSE",LINE4(0));
        
        time = total_time = (min*60)+ sec; //100
        wash_time =(int ) total_time *(0.46); //46
        rinse_time = (int ) total_time *(0.12); //12
        spin_time = total_time - wash_time - rinse_time;
        
        /* TO TURN ON FAN*/
        FAN = ON;
        // turn on the timer
        TMR2ON =1;
    }
    if(key == SW6)
    {
        FAN = OFF;
        TMR2ON = 0;
        operation_mode = PAUSE;
    }
    
    total_time = ( min*60 ) + sec;
    if(program_no <= 7)
    {
        if (total_time >= (time - wash_time)) // 100-46 ->54
        {
            clcd_print("Wash ", LINE1(11));
        }
        else if(total_time >= (time - wash_time - spin_time)) // 12 100-46-42
        {
            clcd_print("Rinse ", LINE1(11));
        }
        else
        {
            clcd_print("Spin ", LINE1(11));
        }
    }
    else if (program_no == 8) // rinse+dry // 40 rinse // 60% wash
    {
        if(total_time >= (time - (0.40*time)))
        {
            clcd_print("Rinse ",LINE1(11));
        }
         else
        {
            clcd_print("Spin ",LINE1(11));
        }
    }
    
    else if(program_no == 9)
    {
        clcd_print("Spin ",LINE1(11));
    }
    else 
    {
        clcd_print("Wash ",LINE1(11));
    }
    clcd_putch((min/10)+'0',LINE2(10));
    clcd_putch((min%10)+'0',LINE2(11));
    clcd_putch(':',LINE2(12));
    clcd_putch((sec/10)+'0',LINE2(13));
    clcd_putch((sec%10)+'0',LINE2(14));
    
    
    if((sec == 0)&& (min == 0))
    {
       /* TO TURN ON FAN*/
        FAN = OFF;
        // turn on the timer
        TMR2ON =0;
        BUZZER = ON;
        clear_screen();
        
        clcd_print("Prog.completed ", LINE1(0));
        clcd_print("Remove Clothes ", LINE2(0));
        __delay_ms(2000);
          BUZZER = OFF;
          operation_mode = WASHING_PROGRAM_DISPALY ;
          reset_mode = WASHING_PROGRAM_DISPALY_RESET ; 
          clear_screen();
    }
    
}
void door_status_check(void)
{
    if(RB0 == 0)  //IF DOOR IS OPEN
    {
        //STOP THE MECHINE  AND TURN OFF THE BUZZER
        FAN = OFF;
        TMR2ON = OFF;
        BUZZER = ON;
        clear_screen();
        clcd_print("Door : OPEN",LINE1(0));
        clcd_print("Please Close",LINE2(0));
        while(RB0==0)
        {
            ;
        }
        clear_screen();
        clcd_print("Function - ", LINE1(0));
        clcd_print("Time: ",LINE2(0));
        clcd_print("5-START 6-PAUSE",LINE4(0)); /* TO TURN ON FAN*/
        FAN = ON;
        // turn on the timer
        TMR2ON =1 ;
        BUZZER = OFF ;
        
        
        
    }
}