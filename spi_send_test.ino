//
// Nick Gammon: http://www.gammon.com.au/spi
// 

#include <SPI.h>

byte spi_slave_ss_pin = A0;

void setup (void){

  pinMode(spi_slave_ss_pin, OUTPUT);
  digitalWrite(spi_slave_ss_pin, HIGH);  // ensure SS stays high for now

  // Put SCK, MOSI, SS pins into output mode
  // also put SCK, MOSI into LOW state, and SS into HIGH state.
  // Then put SPI hardware into Master mode and turn SPI on
  SPI.begin();

  // Slow down the master a bit
  //SPI.setClockDivider(SPI_CLOCK_DIV8); // 1 MHz
  //SPI.setClockDivider(SPI_CLOCK_DIV4); // 2 MHz
}

void loop(){

  char c;

  // enable Slave Select
  digitalWrite(spi_slave_ss_pin, LOW);

  // send test string
  for (const char * p = "Hello, world!\n"; c = *p; p++){
    SPI.transfer(c);
  }
  // disable Slave Select
  digitalWrite(spi_slave_ss_pin, HIGH);

  delay(2000);
} 
