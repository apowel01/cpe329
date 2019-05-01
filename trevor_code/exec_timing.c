/*
 * exec_timing.c
 *
 *  Created on: Apr 28, 2019
 *      Author: Amberley
 */
#include "msp.h"
#include "LCD.h"
#include "keypad.h"
#include "delay.h"
#include "math.h"
//#include "lock.h"
//#include "timer_a.h"

#include "exec_timing.h"

#define var_type int32_t

static var_type test_assign(var_type num)
{
    var_type test_var;

    P1->OUT |= BIT0;// turn RED LED on

    test_var = num;

    P1->OUT &= ~BIT0;
    return test_var;
}

static var_type test_add_1(var_type num)
{
    var_type test_var;

    P1->OUT |= BIT0;// turn RED LED on

    test_var = num + 1;

    P1->OUT &= ~BIT0;
    return test_var;
}

static var_type test_add_7(var_type num)
{
    var_type test_var;

    P1->OUT |= BIT0;// turn RED LED on

    test_var = num + 7;

    P1->OUT &= ~BIT0;
    return test_var;
}

static var_type test_mul_2(var_type num)
{
    var_type test_var;

    P1->OUT |= BIT0;// turn RED LED on

    test_var = num * 2;

    P1->OUT &= ~BIT0;
    return test_var;
}

static var_type test_mul_3(var_type num)
{
    var_type test_var;

    P1->OUT |= BIT0;// turn RED LED on

    test_var = num * 3;

    P1->OUT &= ~BIT0;
    return test_var;
}

static var_type test_div_3(var_type num)
{
    var_type test_var;

    P1->OUT |= BIT0;// turn RED LED on

    test_var = num / 3;

    P1->OUT &= ~BIT0;
    return test_var;
}

static var_type test_sin(var_type num)
{
    var_type test_var;

    P1->OUT |= BIT0;// turn RED LED on

    test_var = sin(num);

    P1->OUT &= ~BIT0;
    return test_var;
}

static var_type test_sqrt(var_type num)
{
    var_type test_var;

    P1->OUT |= BIT0;// turn RED LED on

    test_var = sqrt(num);

    P1->OUT &= ~BIT0;
    return test_var;
}

static var_type test_abs(var_type num)
{
    var_type test_var;

    P1->OUT |= BIT0;// turn RED LED on

    test_var = abs(num);

    P1->OUT &= ~BIT0;
    return test_var;
}

void exec_timing_main(void)
{
    int digit;
    var_type main_var;

    delay_set_dco(FREQ_3_0_MHz);
    lcd_init();
    keypadinit();

    while (1) {
        Clear_LCD();
        Home_LCD();
        P1->SEL1 &= ~BIT0;  //set P1.0 as simple I/O
        P1->SEL0 &= ~BIT0;
        P1->DIR |= BIT0;    //set P1.0 as output

        P2->SEL1 &= ~BIT0;  //set P2.0 as simple I/O
        P2->SEL0 &= ~BIT0;
        P2->DIR |= BIT0;  //set P2.0 as output pins

        Write_string_LCD("use key to test");
        lcd_position_set(0x40); // print fn typ[e on line 2

        digit = keypad_get_digit();
        switch(digit) {
        case 1:
            Write_string_LCD("assign");
            P2->OUT |= BIT0;  // turn on Blue LED
            main_var = test_assign(15);
            P2->OUT &= ~BIT0;
            break;
        case 2:
            Write_string_LCD("add 1");
            P2->OUT |= BIT0;  // turn on Blue LED
            main_var = test_add_1(15);
            P2->OUT &= ~BIT0;
            break;
        case 3:
            Write_string_LCD("add 7");
            P2->OUT |= BIT0;  // turn on Blue LED
            main_var = test_add_7(15);
            P2->OUT &= ~BIT0;
            break;
        case 4:
            Write_string_LCD("multiply by 2");
            P2->OUT |= BIT0;  // turn on Blue LED
            main_var = test_mul_2(15);
            P2->OUT &= ~BIT0;
            break;
        case 5:
            Write_string_LCD("multiply by 3");
            P2->OUT |= BIT0;  // turn on Blue LED
            main_var = test_mul_3(15);
            P2->OUT &= ~BIT0;
            break;
        case 6:
            Write_string_LCD("divide by 3");
            P2->OUT |= BIT0;  // turn on Blue LED
            main_var = test_div_3(15);
            P2->OUT &= ~BIT0;
            break;
        case 7:
            Write_string_LCD("sin()");
            P2->OUT |= BIT0;  // turn on Blue LED
            main_var = test_sin(15);
            P2->OUT &= ~BIT0;
            break;
        case 8:
            Write_string_LCD("sqrt()");
            P2->OUT |= BIT0;  // turn on Blue LED
            main_var = test_sqrt(15);
            P2->OUT &= ~BIT0;
            break;
        case 9:
            Write_string_LCD("abs()");
            P2->OUT |= BIT0;  // turn on Blue LED
            main_var = test_abs(15);
            P2->OUT &= ~BIT0;
            break;
          default:
            break;
        }
        main_var++;
        delay_sec(5);

    }

}
