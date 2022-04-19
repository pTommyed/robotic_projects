//####################################################
//#   encoder_module
//#   Copyright (c) 2022 Tomas Picha
//#   programmed by: Tomas Picha
//####################################################


// DESCRTIPTION: -   sampling frequency 5 kHz
//               -   timer 0 -- sampling freq
//               -   timer 2 -- printing values freq

// HW: Aruino nano

/*----------------------- DEPENDENCES ----------------------------------*/

/*----------------------- DEFINITION -----------------------------------*/
byte counter1_channelA = A0;
byte counter1_channelB = A1;
byte counter2_channelA = A2;
byte counter2_channelB = A3;

int enc_counter_1 = 0; // value of first encoder
int enc_counter_2 = 0; // value of secondencoder

byte encoder1_status = 0;
byte encoder2_status = 0;

static int encoder_status_tabel [] = {0,-1,1,10,1,0,10,-1,-1,10,0,1,10,1,-1,0};


bool timer0_flag = 0;
bool timer2_flag = 0;
int timer2_counter = 0; // count of timer2 interrupt -- TIMER2_COMPA_vect 
static int timer2_final_number = 25; // final count of timer2_counter to 20 Hz



/*----------------------- SETUP -----------------------------------*/
void setup () {
  serial_initial();
  pins_initial();
  encoders_init();
  timer0_initial();
  timer2_initial();
}

/*----------------------- LOOP -----------------------------------*/
void loop (){
  if (timer0_flag == 1) {
    timer0_flag = 0;
    
    encoder_read();
    encoders_counting();
    //serial_prnt();
  }

  if (timer2_flag == 1) {
    timer2_flag = 0;
    timer2_counter = 0;

    serial_prnt();
  } 
}