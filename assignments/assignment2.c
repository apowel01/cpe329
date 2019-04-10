// Write a short script that turns LED1 off and on and
// and or changes the color of LED2

main(void) {
  // turn LED1 on
  P1 -> DIR |= 1;
  P1 -> DIR |= 2;
  P1 -> DIR |= BIT0

  P1 -> SEL0 &=~ BIT0;
  P1 -> SEL1 &=~ BIT0;

  while(1) {
    P1 -> OUT ^= BIT0;
    for(i = 655535; i > 0; i--)
  }

  // turn LED1 off

  // change color of LED2
}
