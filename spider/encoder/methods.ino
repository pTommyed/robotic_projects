/*----------------------serial-initialization------------------------------------*/
void serial_initial(){
  Serial.begin(9600); 
  while (!Serial) {
      ; 
  }
}


/*----------------------pins-initialization------------------------------------*/
void pins_initial(){
  pinMode (counter1_channelA,INPUT);
  pinMode (counter1_channelB,INPUT);
  pinMode (counter2_channelA,INPUT);
  pinMode (counter2_channelB,INPUT);
}

/*----------------------encoder_read------------------------------------*/
void encoder_read(){
  byte temp = 0;

  encoder1_status = encoder1_status & B11;
  encoder1_status = encoder1_status << 2;

  encoder2_status = encoder2_status & B11;
  encoder2_status = encoder2_status << 2;
  
  temp = digitalRead(counter1_channelA);
  temp = temp << 1;
  encoder1_status = encoder1_status | temp;
  encoder1_status = encoder1_status | digitalRead(counter1_channelB);
  
  temp = 0;

  temp = digitalRead(counter2_channelA);
  temp = temp << 1;
  encoder2_status = encoder2_status | temp;
  encoder2_status = encoder2_status | digitalRead(counter2_channelB);
  }

  /*----------------------encoders_counting------------------------------------*/
void encoders_counting(){
  enc_counter_1 = enc_counter_1 + encoder_status_tabel [encoder1_status];
  enc_counter_2 = enc_counter_2 + encoder_status_tabel [encoder2_status];
  }

/*--------------------------interupt_function_timer_1----------------*/
ISR(TIMER1_COMPA_vect)  {
  timer1_flag = 1;
}


/*--------------------------timer1-initialization------------------------------------------*/
void timer1_initial() {
  cli(); // disable all interrupts

  OCR1A = 50; // compareregister 16MHz/64/5kHz = 50 (max 255)
  TCCR1A |= (1 << CS10) | (1 << CS11) | (0 << CS12);    // 64 prescaler 
  TIMSK1 |= (1 <<  OCIE1A);   // enable timer interrupt ( interrupt is active- when is value of TCNT1 (value of counter) in match with OCR1A)
  
  sei();    // enable all interrupts
}