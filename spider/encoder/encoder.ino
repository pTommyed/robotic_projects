//####################################################
//#   encoder_module
//####################################################


// DESCRTIPTION: -   

// HW: Aruino MEGA 2560

/*----------------------- DEPENDENCES ----------------------------------*/


/*----------------------- DEFINITION -----------------------------------*/
int enc_counter_1 = 0; // value of first encoder
int enc_counter_2 = 0; // value of secondencoder

byte interrupt_pin2 = 2; //number of input pin
byte interrupt_pin3 = 3; //number of input pin
byte interrupt_pin18= 18; //number of input pin
byte interrupt_pin19 = 19; //number of input pin



void setup () {
  serial_initial();
  pins_initial();
  interrupts_initial();
}


void loop (){

  Serial.print(enc_counter_1);
  Serial.print("___");
  Serial.println(enc_counter_2);

}