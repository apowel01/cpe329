/*
Lecture 5.2
Author: Amberley Powell
Date: 5/1/2019
How to design an FSM in c
*/

// Classic vending machine example

enum {
    IDLE,
    COUNT_MONEY,
    DISPENSE,
    CHANGE
} state_type;

enum {
    input_money,
    make_select,
    coin_return
} event_type;

state_type current_state = IDLE;
event_type current_event;

uint32_t input_coin, total_money = 0;
uint32_t price, selection;

// would need similiar interrupts for the other events
void selection_ISR(void) {
    current_event = make_select;
    selection = get_keypad();
}

main (void)
{
    while (1) {
        switch (current_state) {
        case IDLE:
        if (current_event == input_money) {
            current_state = COUNT_MONEY;
        }
        break;

        case COUNT_MONEY:
        total_money += input_coin;
        display(total_money);
        if (current_event == make_select) {
            current_event = DISPENSE;
        }
        else if (current_event == coin_return) {
            current_state = CHANGE;
        }
        break;

        case DISPENSE:
        price = price_lookup(selection);
        if (price > total_money) { // else
            display(price);
            current_state = COUNT_MONEY;
        }
        else if (price < total_money) { // if
            dispense(selection);
            total_money -= price;
            current_state = CHANGE;
        }
        else if (price == total_money) { // else if
            dispense(selection);
            total_money = 0;
            current_state = IDLE;
        }
        break;

        case CHANGE:
        return_change(total_money);
        current_state = IDLE;
        total_money = 0;
        break;

        default:
        current_state = IDLE;
        }
    }
}
