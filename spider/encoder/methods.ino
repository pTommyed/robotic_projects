/*----------------------serial-initialization------------------------------------*/
void serial_initial() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
}

/*-----------------------CAN initialization--------------------------------*/
void can_initial(){
  Serial.println("Setup can..");
  while(CAN.begin(CAN_500KBPS, MCP_8MHz) != CAN_OK){
    Serial.print(".");
    delay(1000);
  }

  can_mask_initial();
  can_filters_initial();
  Serial.print("\nCAN init ok!!\r\n");

}

/*-----------------------CAN Mask initialization--------------------------------*/
void can_mask_initial(){
  while(CAN.init_Mask(0, 0, 0xffff) != 0){
    Serial.print("-");
    delay(109);
  }
  while(CAN.init_Mask(1, 0, 0xffff) != 0) {
    Serial.print("-");
    delay(100);
  } 
}

/*-----------------------CAN filters initialization--------------------------------*/
void can_filters_initial(){
  while(CAN.init_Filt(0, 0, 0x00) != 0){
    Serial.print("-");
    delay(10);
  }
  while(CAN.init_Filt(1, 0, 0x00) != 0){
    Serial.print("-");
    delay(100);
  }
  
  
  
  while(CAN.init_Filt(2, 0, 0x00) != 0){
    Serial.print("-");
    delay(100);
  }
  while(CAN.init_Filt(3, 0, 0x00) != 0){
    Serial.print("-");
    delay(100);
  }
  while(CAN.init_Filt(4, 0, 0x00) != 0){
    Serial.print("-");
    delay(100);
  }
  while(CAN.init_Filt(5, 0, 0x00) != 0){
    Serial.print("-");
    delay(100);
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

/*--------------------------interupt_function_timer_0----------------*/
ISR(TIMER0_COMPA_vect) {
  timer0_flag = 1;
}

/*----------------------encoders_initialization------------------------------------*/
void encoders_init() {
  encoder_read();
}

/*----------------------pins-initialization------------------------------------*/
void pins_initial() {
  pinMode(counter1_channelA, INPUT);
  pinMode(counter1_channelB, INPUT);
  pinMode(counter2_channelA, INPUT);
  pinMode(counter2_channelB, INPUT);
}


/*-------------------------- timer0_flag_status_check  -------------------------*/

void timer0_flag_check () {
  if (timer0_flag == 1) {
    timer0_flag = 0;
    
    encoder_read();
    encoders_counting();
    //serial_prnt_encoders();
  }
}

/*-------------------------- can_message_recive_check -------------------------*/

void can_message_recive_check () {
  if (CAN.checkReceive() == CAN_MSGAVAIL) {
    can_message_recive();
    can_message_clear();
    can_msg_to_serial();
    serial_prnt_buf_transmit_hb();
  }
}

/*-------------------------- can_message_clear -------------------------*/

void can_message_clear() {
  while (CAN.clearMsg() != MCP2515_OK) {
    if (timer0_flag == 1) {
      buf_transmit_hb[1] = clear_message_error;
      return;
    }
  }
}

/*-------------------------- can message recive -------------------------*/

void can_message_recive() {
  can_message_id_recive = CAN.getCanId();
    if (can_message_id_recive == can_adress_nmt) {
     nmt_recive_flag = 1;
     while (CAN.readMsgBuf(&len, buf_recive) != CAN_OK) {
       if (timer0_flag == 1) {
         buf_transmit_hb[1] = read_message_error;
         return;
       }
     }
    }
}

/*----------------------serial-print_recive_can_message------------------------------------*/

void can_msg_to_serial(){
      Serial.println("-----------------------------------------");
      Serial.print("can address: ");
      Serial.print(can_message_id_recive);
      Serial.print("; lenght: ");
      Serial.println(len);
      for (int i=0;i<len;i++){
        Serial.print(buf_recive[i]);
        Serial.print("\t");
      }
      Serial.println("");
      Serial.println("-----------------------------------------");
}

/*----------------------serial-print_encoders------------------------------------*/
void serial_prnt_encoders() {
  Serial.print(enc_counter_1);
  Serial.print(" ## ");
  Serial.println(enc_counter_2);
}

/*----------------------serial-print_buf_transmit_hb------------------------------------*/
void serial_prnt_buf_transmit_hb() {
  Serial.print(buf_transmit_hb[0]);
  Serial.print("##");
  Serial.println(buf_transmit_hb[1]);
}

/*---------------------------- counter_cycle--------------------------------------------------------------------------------------------------*/

void counter_cycle(){
  buf_transmit_hb [0] = cycle_counter;
  if (cycle_counter == 255){
    cycle_counter = 1;
  } else {
     cycle_counter = cycle_counter + 1;
    }
}

/*--------------------------- buf_transmit_hb_clear----------------------------------------------------------------------------------*/

void buf_transmit_hb_clear(){
  buf_transmit_hb [0] = 0;
  buf_transmit_hb [1] = 0;
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

/*----------------------encoders_counting------------------------------------*/
void encoders_counting() {
  enc_counter_1 = enc_counter_1 + encoder_status_tabel[encoder1_status];
  enc_counter_2 = enc_counter_2 + encoder_status_tabel[encoder2_status];
}
