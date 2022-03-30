//####################################################
//#   encoder_module
//#   Copyright (c) 2022 Tomas Picha
//#   programmed by: Tomas Picha
//####################################################


// DESCRTIPTION: -   

// HW: Aruino nano

/*----------------------- DEPENDENCES ----------------------------------*/


/*----------------------- DEFINITION -----------------------------------*/
byte counter1_channel1 = A0;
byte counter1_channel2 = A1;
byte counter2_channel1 = A2;
byte counter2_channel2 = A3;

int enc_counter_1 = 0; // value of first encoder
int enc_counter_2 = 0; // value of secondencoder

byte encoder1_status = 0;
byte encoder2_status = 0;


byte encoder_status_tabel [] = {};



void setup () {
  serial_initial();
  pins_initial();
}


void loop (){
  encoder_read();


}