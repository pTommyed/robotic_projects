/*----------------------serial-initialization------------------------------------*/
void serial_initial(){
  Serial.begin(2000000); 
  while (!Serial) {
      ; 
  }
}

/*----------------------pins-initialization------------------------------------*/
void pins_initial(){

  pinMode(interrupt_pin2, INPUT_PULLUP); // settings of external pullup event
  pinMode(interrupt_pin3, INPUT_PULLUP); // settings of external pullup event
  pinMode(interrupt_pin18, INPUT_PULLUP); // settings of external pullup event
  pinMode(interrupt_pin19, INPUT_PULLUP); // settings of external pullup event

}

/*----------------------interrupts-initialization------------------------------------*/
void interrupts_initial(){

  attachInterrupt(0, interrupt_0, RISING);
  attachInterrupt(1, interrupt_1, RISING);
  attachInterrupt(2, interrupt_2, RISING);
  attachInterrupt(3, interrupt_3, RISING);
  
}

/*----------------------interrupt_0------------------------------------*/
void interrupt_0(){

  if(digitalRead(interrupt_pin3)==LOW) { // checking direction of encoder 1
    enc_counter_1 ++;
  }else{
    enc_counter_1 --;
  }
  
}

/*----------------------interrupt_1------------------------------------*/
void interrupt_1(){

  if(digitalRead(interrupt_pin2)==LOW) { // checking direction of encoder 1
    enc_counter_1--;
  }else{
    enc_counter_1++;
  }
  
}

/*----------------------interrupt_2------------------------------------*/
void interrupt_2(){

  if(digitalRead(interrupt_pin19)==LOW) { // checking direction of encoder 2
    enc_counter_2 ++;
  }else{
    enc_counter_2 --;
  }
  
}

/*----------------------interrupt_3------------------------------------*/
void interrupt_3(){

  if(digitalRead(interrupt_pin18)==LOW) { // checking direction of encoder 2
    enc_counter_2--;
  }else{
    enc_counter_2++;
  }
  
}