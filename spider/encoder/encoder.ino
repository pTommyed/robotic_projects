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

enum states_node {
  bootup,
  preoperation,
  operation,
  reset,
};

states_node state_node = bootup;

static int bootup_transmit = 0x00;  //0x00
static int preoperation_transmit = 0x7f; //0x7f
static int operation_transmit = 0x85; //0x85

static int preoperation_recive = 0x80; //0x80
static int operation_recive = 0x01; //0x01
static int reset_node_recive = 0x81; //0x81
static int reset_comunication_recive = 0x82; //0x82


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
bool hb_flag = 0;
bool sync_message_rec_flag = 0;

int timer0_counter =0;
static int timer0_1s_count = 977;


MCP_CAN CAN(cs_pin);

static int id_node = 0x02; // 0x02 can adress of node
static int can_adress_pdo_encoder = 0x180 + id_node; //0x180 transmit encoder counters values
static int can_adress_pdo_error = 0x280 + id_node; //0x280 transmit errors codes
static int can_adress_hb = 0x700 + id_node; //0x700  transmit HB status
static int can_adress_nmt = 0x00; //0x00 change states
static int can_adress_sync = 0x50; //0X50 sync adress
int  can_message_id_recive; //can message of receive message
byte buf_recive[8]; //buffer of receive message
byte buf_transmit_hb [] = {bootup_transmit}; //buffer of transmit message
byte buf_transmit_pdo_encoders[8]; //buffer of transmit message encoders status
byte buf_transmit_pdo_erros [] = {0,0,0,0,0}; //buffer of transmit message errors status buf[]= {status-if any error then=1 else=0, read_message_error, clear_message_error, send_message_error, encoder_skip_error}
unsigned char len = 0; // lenght of receive message
byte len_transmit = 0; // lenght of transmit can message
static byte len_hb = 1;
static byte len_pdo_encoders = 8;
static byte len_pdo_errors = 5; 


byte read_message_error = 1;
byte clear_message_error = 2;
byte send_message_error = 3;
byte encoder_skip_error = 4;

/*----------------------- SETUP -----------------------------------*/
void setup () {
  serial_initial();
  pins_initial();
  encoders_init();
  can_initial();
  timer0_initial();
  wdt_enable(WDTO_500MS);  //wdt reset_15MS; 30MS; 60ms; 120MS; 250MS; 500MS; 1S; 2S; 4S; 8S
  sending_can_message (can_adress_hb, buf_transmit_hb, len_hb);
  state_node = preoperation;
}

/*----------------------- LOOP -----------------------------------*/
void loop (){
  switch (state_node) {
    case preoperation:
      hb_flag_check ();
      break;
    case operation:
      if (sync_message_rec_flag == 1) {
        sync_message_rec_flag = 0;
        sending_can_message (can_adress_pdo_encoder, buf_transmit_pdo_encoders, len_pdo_encoders);
      }
      hb_flag_check ();
      break;
    case reset:

      break;
  }
  
  timer0_flag_check (); 
  can_message_recive_check ();
  can_message_clear();
  wdt_reset (); // Reset watchdog counter
}