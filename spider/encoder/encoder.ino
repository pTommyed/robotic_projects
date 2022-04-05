//####################################################
//#   encoder_module
//#   Copyright (c) 2022 Tomas Picha
//#   programmed by: Tomas Picha
//####################################################


// DESCRTIPTION: -   

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

static byte encoder_status_tabel [] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};


bool timer1_flag = 0;  



/*----------------------- SETUP -----------------------------------*/

void setup () {
  serial_initial();
  pins_initial();
  timer1_initial();
}

/*----------------------- LOOP -----------------------------------*/

void loop (){
  if (timer1_flag == 1) {
    timer1_flag = 0;
    encoder_read();
    encoders_counting();
  }

}