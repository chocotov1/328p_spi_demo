//
// Nick Gammon: http://www.gammon.com.au/spi
// 

#include <SPI.h>

byte spi_relay_ss_pin = A0;

void setup (void){

  pinMode(spi_relay_ss_pin, OUTPUT);
  digitalWrite(spi_relay_ss_pin, HIGH);  // ensure SS stays high for now

  // Put SCK, MOSI, SS pins into output mode
  // also put SCK, MOSI into LOW state, and SS into HIGH state.
  // Then put SPI hardware into Master mode and turn SPI on
  SPI.begin();

  // Slow down the master a bit
  //SPI.setClockDivider(SPI_CLOCK_DIV8); // 1 MHz
  //SPI.setClockDivider(SPI_CLOCK_DIV4); // 2 MHz
}

void spi_relay_msg(uint8_t id, float volt, float temperature){
  digitalWrite(spi_relay_ss_pin, LOW);

  // Slow down the master a bit
  SPI.setClockDivider(SPI_CLOCK_DIV8); // 1 MHz
  
  uint8_t buf_size = 30;
  char buf[buf_size]{0};

  itoa(id, buf, 10);
  for (uint8_t i = 0; i < buf_size; i++){
     if (buf[i] == 0){
       break;
     }

     SPI.transfer(buf[i]);
  }

  SPI.transfer(':');
  SPI.transfer(' ');

  dtostrf(volt, 4, 2, buf);
  for (uint8_t i = 0; i < buf_size; i++){
     if (buf[i] == 0){
       break;
     }

     SPI.transfer(buf[i]);
  }
  SPI.transfer('V');
  SPI.transfer(',');
  SPI.transfer(' ');

  dtostrf(temperature, 4, 1, buf);
  for (uint8_t i = 0; i < buf_size; i++){
     if (buf[i] == 0){
       break;
     }

     SPI.transfer(buf[i]);
  }
  SPI.transfer('C');

  SPI.transfer(10); // ascii "\n" (end of sequence)

  // return to faster SPI
  SPI.setClockDivider(SPI_CLOCK_DIV2); // 4 MHz
  
  digitalWrite(spi_relay_ss_pin, HIGH);
}

void loop(){

  char c;

  // enable Slave Select
  digitalWrite(spi_relay_ss_pin, LOW);

  // send test string
  for (const char * p = "Hello, world!\n"; c = *p; p++){
    SPI.transfer(c);
  }
  // disable Slave Select
  digitalWrite(spi_relay_ss_pin, HIGH);

  //spi_relay_msg(1, 1.23, 10.2){

  delay(2000);
} 
