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
  while(CAN.init_Filt(0, 0, can_adress_nmt) != 0){ //nmt adress
    Serial.print("-");
    delay(10);
  }
  while(CAN.init_Filt(1, 0, can_adress_sync) != 0){ // sync adress
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
  timer0_counter = timer0_counter + 1;
  if (timer0_counter == timer0_1s_count) {
    hb_flag = 1;
    timer0_counter = 0;
  }
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

  pinMode(led_preoperation, OUTPUT);
  pinMode(led_operation, OUTPUT);
}

/*----------------------preoperation_led_settings------------------------------------*/
void preoperation_led_settings() {
  digitalWrite(led_operation, LOW);
  digitalWrite(led_preoperation, HIGH);
}

/*----------------------operation_led_settings------------------------------------*/
void operation_led_settings() {
  digitalWrite(led_preoperation, LOW);
  digitalWrite(led_operation, HIGH);
}

/*----------------------bootup_led_indication------------------------------------*/
void bootup_led_indication() {
  for (int temp = 0;temp < 5; temp++) {
    wdt_reset (); // Reset watchdog counter
    digitalWrite(led_operation, LOW);
    digitalWrite(led_preoperation, LOW);
    delay(100);
    digitalWrite(led_operation, HIGH);
    digitalWrite(led_preoperation, HIGH);
    delay(100);
  }
}

/*----------------------switch_to_preoperation------------------------------------*/
void switch_to_preoperation() {
  sending_can_message (can_adress_hb, buf_transmit_hb, len_hb);
  timer0_counter =0;
  hb_flag = 0;
  state_node = preoperation;
  buf_transmit_hb[0] = preoperation_transmit;
  preoperation_led_settings();
}

/*-------------------------- timer0_flag_status_check  -------------------------*/

void timer0_flag_check () {
  if (timer0_flag == 1) {
    timer0_flag = 0;
    
    encoder_read();
    encoders_counting();
    //serial_prnt_encoders();
    pdo_encoders_message_create();
  }
}

/*-------------------------- hb_flag_status_check  -------------------------*/

void hb_flag_check () {
  if (hb_flag == 1) {
    hb_flag = 0;
    sending_can_message (can_adress_hb, buf_transmit_hb, len_hb);

    error_check ();
  }
}

/*-------------------------- error_check  -------------------------*/

void error_check () {
  if (buf_transmit_pdo_erros[0] == 1) {
    sending_can_message (can_adress_pdo_error, buf_transmit_pdo_erros, len_pdo_errors);
    buf_transmit_pdo_erros_clear ();
  }
}

/*-------------------------- buf_transmit_pdo_erros_clear  -------------------------*/

void buf_transmit_pdo_erros_clear () {
  for (int temp = 0; temp < len_pdo_errors; temp++) {
    buf_transmit_pdo_erros [temp] = 0;
  }
}

/*-------------------------- can_message_recive_check -------------------------*/

void can_message_recive_check () {
  if (CAN.checkReceive() == CAN_MSGAVAIL) {
    sync_message_rec_flag = 0;
    can_message_recive();
    if (can_message_id_recive == can_adress_sync) {
      sync_message_rec_flag = 1;
    } else if (can_message_id_recive == can_adress_nmt) {
        if ((buf_recive[1] ==0x00)||(buf_recive[1] == id_node)){ // pozn. zeptat se na jaká tam má být adresa 
          switch (buf_recive[0]) {
            case operation_recive:
              state_node = operation;
              buf_transmit_hb [0] = operation_transmit;
              operation_led_settings();
              break;
            case preoperation_recive:
              state_node = preoperation;
              buf_transmit_hb [0] = preoperation_transmit;
              preoperation_led_settings();
              break;
            case reset_node_recive:
              state_node = reset;
              buf_transmit_hb [0] = bootup_transmit;
              break;
            case reset_comunication_recive:
              state_node = reset;
              buf_transmit_hb [0] = bootup_transmit;
              break;
          }
        }
      }
    //can_msg_to_serial();
  }
}

/*-------------------------- can_message_clear -------------------------*/

void can_message_clear() {
  while (CAN.clearMsg() != MCP2515_OK) {
    if (timer0_flag == 1) {
      buf_transmit_pdo_erros[2] = clear_message_error;
      buf_transmit_pdo_erros[0] = 1;
      return;
    }
  }
}

/*-------------------------- can message recive -------------------------*/

void can_message_recive() {
  while (CAN.readMsgBuf(&len, buf_recive) != CAN_OK) {
    if (timer0_flag == 1) {
      buf_transmit_pdo_erros[1] = read_message_error;
      buf_transmit_pdo_erros[0] = 1;
      return;
    }
  }
  can_message_id_recive = CAN.getCanId();
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

/*-------------------------- Sending CAN message to APU -------------------------*/

void sending_can_message (int can_adress, byte *message_buffer, byte lenght_message) {
  while (CAN.sendMsgBuf(can_adress, 0, lenght_message, message_buffer) != 0) {
    if (timer0_flag == 1) {
      buf_transmit_pdo_erros[3] = send_message_error;
      buf_transmit_pdo_erros[0] = 1;
      return;
    }  
  }
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
  if(abs(encoder_status_tabel[encoder1_status]) != 10) {
    enc_counter_1 = enc_counter_1 + encoder_status_tabel[encoder1_status];
  } else {
    buf_transmit_pdo_erros [4] = encoder_skip_error; 
    buf_transmit_pdo_erros[0] = 1;
  }
  
  if(abs(encoder_status_tabel[encoder2_status]) != 10) {
    enc_counter_2 = enc_counter_2 + encoder_status_tabel[encoder2_status];
  } else {
    buf_transmit_pdo_erros [4] = encoder_skip_error; 
    buf_transmit_pdo_erros[0] = 1;
  }
}

/*----------------------pdo_encoders_message_create------------------------------------*/
void pdo_encoders_message_create() {
  buf_transmit_pdo_encoders[7] = (enc_counter_1 >> 24) & 0xFF;
  buf_transmit_pdo_encoders[6] = (enc_counter_1 >> 16) & 0xFF;
  buf_transmit_pdo_encoders[5] = (enc_counter_1 >> 8) & 0xFF;
  buf_transmit_pdo_encoders[4] = enc_counter_1  & 0xFF;

  buf_transmit_pdo_encoders[3] = (enc_counter_2 >> 24) & 0xFF;
  buf_transmit_pdo_encoders[2] = (enc_counter_2 >> 16) & 0xFF;
  buf_transmit_pdo_encoders[1] = (enc_counter_2 >> 8) & 0xFF;
  buf_transmit_pdo_encoders[0] = enc_counter_2  & 0xFF;
}