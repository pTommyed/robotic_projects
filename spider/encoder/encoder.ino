//####################################################
//#   encoder_module
//#   Copyright (c) 2022 Tomas Picha
//#   programmed by: Tomas Picha
//####################################################


// DESCRTIPTION: -   sampling frequency 5 kHz
//               -   timer 0 -- sampling freq
//               -   CAN bus implamantaton (library)

// HW: Aruino nano, mcp2515

/*----------------------- DEPENDENCES ----------------------------------*/
#include <mcp_can.h> // CAN lib
#include <SPI.h> // lib for SPI bus
#include <stdint.h> //due to wdt
#include <avr/wdt.h> // wdt lib


/*----------------------- DEFINITION -----------------------------------*/
byte counter1_channelA = A0;
byte counter1_channelB = A1;
byte counter2_channelA = A2;
byte counter2_channelB = A3;

byte cs_pin = 10; // SPI CS pin number

int enc_counter_1 = 0; // value of first encoder
int enc_counter_2 = 0; // value of secondencoder

byte encoder1_status = 0;
byte encoder2_status = 0;

static int encoder_status_tabel [] = {0,-1,1,10,1,0,10,-1,-1,10,0,1,10,1,-1,0};


bool timer0_flag = 0;
bool nmt_recive_flag = 0;


MCP_CAN CAN(cs_pin);

static int can_adress_pdo = 131; //transmit encoder counters values
static int can_adress_hb = 130; //  transmit HB adress
static int can_adress_nmt = 0; // Sync adress
int  can_message_id_recive; //can message of receive message
byte buf_recive[8]; //buffer of receive message
byte buf_transmit_hb[2] = {0,0}; //buffer of transmit message
byte buf_transmit_pdo[8]; //buffer of transmit message
unsigned char len = 0; // lenght of receive message

byte cycle_counter = 0; 

byte read_message_error = 1;
byte clear_message_error = 2;

/*----------------------- SETUP -----------------------------------*/
void setup () {
  serial_initial();
  pins_initial();
  encoders_init();
  can_initial();
  timer0_initial();
  wdt_enable(WDTO_500MS);  //wdt reset_15MS; 30MS; 60ms; 120MS; 250MS; 500MS; 1S; 2S; 4S; 8S
}

/*----------------------- LOOP -----------------------------------*/
void loop (){
  counter_cycle();
  timer0_flag_check ();
  can_message_recive_check ();
  buf_transmit_hb_clear();
  wdt_reset (); // Reset watchdog counter
}