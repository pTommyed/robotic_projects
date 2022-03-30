/*----------------------serial-initialization------------------------------------*/
void serial_initial(){
  Serial.begin(9600); 
  while (!Serial) {
      ; 
  }
}


/*----------------------pins-initialization------------------------------------*/
void pins_initial(){
  
  pinMode (counter1_channel1,INPUT);
  pinMode (counter1_channel2,INPUT);
  pinMode (counter2_channel1,INPUT);
  pinMode (counter2_channel2,INPUT);

}

/*----------------------encoder_read------------------------------------*/
void encoder_read(){
  byte temp = 0;

  encoder1_status = encoder1_status & B11;
  encoder1_status = encoder1_status << 2;

  encoder2_status = encoder2_status & B11;
  encoder2_status = encoder2_status << 2;
  
  temp = digitalRead(counter1_channel1);
  temp = temp << 1;
  encoder1_status = encoder1_status | temp;
  encoder1_status = encoder1_status | digitalRead(counter1_channel2);
  
  temp = 0;

  temp = digitalRead(counter2_channel1);
  temp = temp << 1;
  encoder2_status = encoder2_status | temp;
  encoder2_status = encoder2_status | digitalRead(counter2_channel2);

  }

/*----------------------interrupt_0------------------------------------
void interrupt_0(){

  if(digitalRead(interrupt_pin3)==LOW) { // checking direction of encoder 1
    enc_counter_1 ++;
  }else{
    enc_counter_1 --;
  }
  
}

/*----------------------interrupt_1------------------------------------
void interrupt_1(){

  if(digitalRead(interrupt_pin2)==LOW) { // checking direction of encoder 1
    enc_counter_1--;
  }else{
    enc_counter_1++;
  }
  
}*/