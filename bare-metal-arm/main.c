#include "MKL46Z4.h"
#include "lcd.h"

// LED (RG)
// LED_GREEN = PTD5 (pin 98)
// LED_RED = PTE29 (pin 26)

// SWICHES
// RIGHT (SW1) = PTC3 (pin 73)
// LEFT (SW2) = PTC12 (pin 88)

// Enable IRCLK (Internal Reference Clock)
// see Chapter 24 in MCU doc
int sw1_check=0, sw2_check=0;

void irclk_ini()
{
  MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
  MCG->C2 = MCG_C2_IRCS(0); //0 32KHZ internal reference clock; 1= 4MHz irc
}

void delay(void)
{
  volatile int i;

  for (i = 0; i < 1000000; i++);
}

// RIGHT_SWITCH (SW1) = PTC3
// LEFT_SWITCH (SW2) = PTC12

// LED_GREEN = PTD5
void led_green_ini()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
  PORTD->PCR[5] = PORT_PCR_MUX(1);
  GPIOD->PDDR |= (1 << 5);
  GPIOD->PSOR = (1 << 5);
}

void led_green_toggle()
{
  GPIOD->PTOR = (1 << 5);
}

// LED_RED = PTE29
void led_red_ini()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
  PORTE->PCR[29] = PORT_PCR_MUX(1);
  GPIOE->PDDR |= (1 << 29);
  GPIOE->PSOR = (1 << 29);
}

void led_red_toggle(void)
{
  GPIOE->PTOR = (1 << 29);
}

// LED_RED = PTE29
// LED_GREEN = PTD5
void leds_ini()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;
  PORTD->PCR[5] = PORT_PCR_MUX(1);
  PORTE->PCR[29] = PORT_PCR_MUX(1);
  GPIOD->PDDR |= (1 << 5);
  GPIOE->PDDR |= (1 << 29);
  // both LEDS off after init
  GPIOD->PSOR = (1 << 5);
  GPIOE->PSOR = (1 << 29);
}

//comun a ambos botones
void sws_ini(){
  SIM->COPC = 0;             
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  NVIC_SetPriority(31, 0);  
  NVIC_EnableIRQ(31);  
}

// RIGHT_SWITCH (SW1) = PTC3
void sw1_ini()
{
  PORTC->PCR[3] |= PORT_PCR_MUX(1); 
  PORTC->PCR[3] |= PORT_PCR_PE_MASK;
  PORTC->PCR[3] |= PORT_PCR_PS_MASK;

  PORTC->PCR[3] |= PORT_PCR_IRQC(0xA);    
}

// LEFT_SWITCH (SW2) = PTC12
void sw2_ini()
{
  PORTC->PCR[12] |= PORT_PCR_MUX(1); 
  PORTC->PCR[12] |= PORT_PCR_PE_MASK;
  PORTC->PCR[12] |= PORT_PCR_PS_MASK;

  PORTC->PCR[12] |= PORT_PCR_IRQC(0xA);   
}

// Hit condition: (else, it is a miss)
// - Left switch matches red light
// - Right switch matches green light

void PORTDIntHandler(void) {
  int pressed_switch = PORTC->ISFR;
  PORTC->ISFR = 0xFFFFFFFF; // Clear IRQ

  // SW1
  if(pressed_switch == (0x8)) {
    ++sw1_check;
  }
  // SW2
  if(pressed_switch == (0x1000)) {
    ++sw2_check;
  }
}

int main(void)
{
	int hits=0, misses=0;
	//NVIC_EnableIRQ(31);
  irclk_ini(); // Enable internal ref clk to use by LCD

  lcd_ini();
  leds_ini();
  led_green_ini();
  led_red_ini();
  sws_ini();
  sw1_ini();
  sw2_ini();

  // 'Random' sequence :-)
  volatile unsigned int sequence = 0x32B14D98,
    index = 0;
//sw2=rojo sw1=verde
  while (index < 32) {
    if (sequence & (1 << index)) { //odd
		// Switch on green led
		led_green_toggle();
		delay();
		if (sw1_check && !sw2_check) hits++;
		else misses++;
		sw1_check=0;
		sw2_check=0;
		led_green_toggle();
    } else { //even
		// Switch on red led
		led_red_toggle();
		delay();
		if (sw2_check && !sw1_check) hits++;
		else misses++;
		sw1_check=0;
		sw2_check=0;
		led_red_toggle();
    }
    lcd_display_time(hits,misses);
    index++;
  }

  // Stop game and show blinking final result in LCD: hits:misses
  //set lcd blinkyibg mode 
	LCD->AR =
		LCD_AR_BLINK(1) |
		LCD_AR_BRATE(0xAA);
	//result
	lcd_display_time(hits,misses);
  while (1) 

  return 0;
}
