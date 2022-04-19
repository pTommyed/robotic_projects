/*----------------------serial-initialization------------------------------------*/
void serial_initial() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
}

/*----------------------serial-print------------------------------------*/
void serial_prnt() {
  Serial.print(enc_counter_1);
  Serial.print(" ## ");
  Serial.println(enc_counter_2);
}

/*----------------------pins-initialization------------------------------------*/
void pins_initial() {
  pinMode(counter1_channelA, INPUT);
  pinMode(counter1_channelB, INPUT);
  pinMode(counter2_channelA, INPUT);
  pinMode(counter2_channelB, INPUT);
}

/*----------------------encoder_read------------------------------------*/
void encoder_read() {
  byte temp_A = 0;
  byte temp_B = 0;

  encoder1_status = encoder1_status & B0011;
  encoder1_status = encoder1_status << 2;

  encoder2_status = encoder2_status & B0011;
  encoder2_status = encoder2_status << 2;

  temp_A = digitalRead(counter1_channelA);
  temp_A = temp_A & B0001;
  temp_A = temp_A << 1;
  encoder1_status = encoder1_status | temp_A;
  temp_B = digitalRead(counter1_channelB);
  temp_B = temp_B & B0001;
  encoder1_status = encoder1_status | temp_B;

  temp_A = 0;
  temp_B = 0;

  temp_A = digitalRead(counter2_channelA);
  temp_A = temp_A & B0001;
  temp_A = temp_A << 1;
  encoder2_status = encoder2_status | temp_A;
  temp_B = digitalRead(counter2_channelB);
  temp_B = temp_B & B0001;
  encoder2_status = encoder2_status | temp_B;
}

/*----------------------encoders_initialization------------------------------------*/
void encoders_init() {
  encoder_read();
}

/*----------------------encoders_counting------------------------------------*/
void encoders_counting() {
  enc_counter_1 = enc_counter_1 + encoder_status_tabel[encoder1_status];
  enc_counter_2 = enc_counter_2 + encoder_status_tabel[encoder2_status];
}

/*--------------------------interupt_function_timer_0----------------*/
ISR(TIMER0_COMPA_vect) {
  timer0_flag = 1;
}

/*--------------------------interupt_function_timer_2----------------*/
ISR(TIMER2_COMPA_vect) {
  timer2_counter = timer2_counter + 1;
  if (timer2_counter == timer2_final_number) {
    timer2_flag = 1;
  }
}


/*--------------------------timer0-initialization------------------------------------------*/
void timer0_initial() {
  cli();  // disable all interrupts

  OCR0A = 50;                                         // compareregister 16MHz/64/5kHz = 50 (max 255)
  TCCR0B |= (1 << CS00) | (1 << CS01) | (0 << CS02);  // 64 prescaler
  TIMSK0 |= (1 << OCIE0A);                            // enable timer interrupt ( interrupt is active- when is value of TCNT0 (value of counter) in match with OCR0A)

  sei();  // enable all interrupts
}

/*--------------------------timer2-initialization------------------------------------------*/
void timer2_initial() {
  cli();  // disable all interrupts

  OCR2A = 250;                                         // compareregister 16MHz/128/5kHz = 6250 / 250 = 25   (max 255)
  TCCR2B |= (1 << CS20) | (0 << CS21) | (1 << CS22);  // 128 prescaler
  TIMSK2 |= (1 << OCIE2A);                            // enable timer interrupt ( interrupt is active- when is value of TCNT2 (value of counter) in match with OCR2A)

  sei();  // enable all interrupts
}