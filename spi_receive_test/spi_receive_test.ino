#include <Tiny4kOLED_Wire.h>

#include <SPI.h>

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

char buf[100];
volatile byte pos;
volatile boolean process_it;
volatile bool spi_stc_vect_debug = false;

void setup() {
  // disable ADC
  ADCSRA = 0;
   
  // put your setup code here, to run once:

  oled.begin();

  oled.setContrast(100); // 0: dim, 255: bright
  //oled.setFont(FONT6X8);
  oled.setFont(FONT8X16);
  // big numbers
  //oled.setFont(FONT16X32DIGITS);

  oled.setVcomhDeselectLevel(0); // darker screen, bigger contrast range 
  oled.setPrechargePeriod(0,0);  // darker screen, bigger contrast range

  //oled.switchDisplayFrame();
  oled.clear();

  oled.switchFrame();
  oled.clear();
  oled.on();

  // measured i2c clock frequency with oscilloscope:

  // MiniCore: 8 mhz atmega328p
  //TWBR = 255; // 15 KHz
  //TWBR = 128; // 30 KHz
  //TWBR = 64;  // 56 KHz
  //TWBR = 32;  // 100 KHz (default value)
  //TWBR = 16;  // 168 KHz
  //TWBR = 8;   // 250 KHz
  TWBR = 0;   // 500 KHz

  // ATTinyCore: can't change clock frequency
  // TWBR not defined
  // Wire.cpp: TwoWire::setClock(uint32_t clock): to be implemented
  // 8 MHz attiny 85: 100 KHz
  // 1 MHz attiny 85: 50 KHz

  //
  // Nick Gammon: http://www.gammon.com.au/spi
  // 
  // turn on SPI in slave mode
  SPCR |= bit(SPE);

  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  
  // get ready for an interrupt 
  pos = 0;   // buffer empty
  process_it = false;

  // now turn on interrupts
  SPI.attachInterrupt();

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

   wdt_reset();
   noInterrupts();
   // wdt sleep settings:
   // 10.8 Watchdog Timer
   WDTCSR |= 1<<WDCE | 1<<WDE;

   // Table 10-3. Watchdog Timer Prescale Select
   //WDTCSR = 1<<WDP3 | 1<<WDP0;             // 8 seconds
   //WDTCSR = 1<<WDP3;                       // 4 seconds
   //WDTCSR = 1<<WDP2 | 1<<WDP1 | 1<<WDP0;   // 2 seconds
   WDTCSR = 1<<WDP2 | 1<<WDP1;               // 1 seconds
   //WDTCSR = WDTO_30MS;
   //WDTCSR = WDTO_15MS;

   WDTCSR |= 1<<WDIE;    // WDIE: Watchdog Interrupt Enable
   //WDTCSR |= 1<<WDE;   // WDE : Watchdog System Reset Enable

   // PIN change interrupt
   //PCICR = 1<<PCIE2;  // PORTD: digital pins 0 to 7
   //PCICR = 1<<PCIE1;  // PORTC: analog input pins
   PCICR = 1<<PCIE0;    // PORTB: digital pin 8 to 13

   //12.2.6 PCMSK2 – Pin Change Mask Register 2
   // • Bit 7..0 – PCINT23..16: Pin Change Enable Mask 23..16
   //PCMSK2 = 0;
   //12.2.7 PCMSK1 – Pin Change Mask Register 1
   // • Bit 6..0 – PCINT14..8: Pin Change Enable Mask 14..8
   //PCMSK1 = 1<<PCINT8; // A0 / PCINT8
   // 12.2.8 PCMSK0 – Pin Change Mask Register 0
   //• Bit 7..0 – PCINT7..0: Pin Change Enable Mask 7..0
   //PCMSK0 = 1<<PCINT1; // 9: PB1 / PCINT1
   PCMSK0 = 1<<PCINT2;   // 10: PB2 / PCINT2
   
   interrupts();

  delay(200);
  oled.setCursor(0,0);
  oled.print("spi receive test");
  //debug_spi_activity();
}

ISR(WDT_vect) {
}

ISR(PCINT0_vect){
}

void go_to_sleep(){
  // reset wdt counter
  wdt_reset();

  //sleep_mode: Put the device into sleep mode, taking care of setting the SE bit before, and clearing it afterwards.
  sleep_mode();

  // reenable interrupt mode
  WDTCSR |= 1<<WDIE;
}

// SPI interrupt routine
ISR (SPI_STC_vect){
  //spi_stc_vect_debug = true;
  byte c = SPDR;  // grab byte from SPI Data Register
  
  // add to buffer if room
  if (pos < (sizeof (buf) - 1))
    buf [pos++] = c;
    
  // example: newline means time to process buffer
  if (c == '\n')
    process_it = true;
      
}

void print_update(){
  oled.clear();
  oled.setCursor(0,0);
  oled.print(buf);
  //oled.print(count);
}

void print_spi(){
  oled.setCursor(0,0);

  oled.print("MISO: ");
  oled.print(MISO);
  oled.print(", ");

  oled.print("SS: ");
  oled.print(SS);
  oled.print(", ");
  oled.print("SCK: ");
  oled.print(SCK);
}

void debug_spi_activity(){
  oled.setCursor(0, 2);
  oled.print("spi_stc_vect");
}

void loop() {
  // put your main code here, to run repeatedly:
  static uint32_t count{0};
  
  count++;
  
  //ltoa(count, buf, 10);
  //print_update();
  //delay(1000);
  
  if (process_it){
    oled.on();
    buf[pos] = 0;  
    //Serial.println (buf);
    print_update();
    pos = 0;
    process_it = false;
    spi_stc_vect_debug = false;

    go_to_sleep();
    //delay(500);
    oled.off();
  }


  //go_to_sleep();

  if (PINB & 1<<PB2){
     go_to_sleep();
  }

/*
  if (spi_stc_vect_debug){ 
    debug_spi_activity();
  }
*/
}
