
#define PACK_TIMEOUT 500
#define BUFFER_SIZE 512 //serial data

char freq0[8];
String vfoab;
String modeset;
char VfoData;
String ModeData;
String FreqData;
String freq1;
String freq2;
String freq3;
String freq4;
char menu[11];
String menudata;
char mch[2];
String mchdata;
char sig[1];
String sigdata;
String prevRigdata = "";
String led0;
String led1;
String led2;
String led3;
String led4;
String led5;
String led6;
String ts;
String meter;
String smeter;

int pttPin = 36;
int pwrPin = 39;
int pwr_lastButtonState = LOW; 
int ptt_lastButtonState = LOW; 
int pwr_buttonState = 0; 
int ptt_buttonState = 0;
int outputPtt = 32;
int outputPwr = 2;

uint8_t tx_buf[BUFFER_SIZE];
uint16_t tx_len=0;
uint8_t rx_buf[BUFFER_SIZE];
uint16_t rx_len=0;
uint8_t hmi_buf[8];
uint16_t hmi_len=0;


void setup();
void button_task(void *param);
void send_keepalive(void *param);
void rigscreen();
void PanelDecode();



TaskHandle_t button_task_;
TaskHandle_t send_keepalive_;

void setup() {
 
  
  pinMode(pttPin, INPUT);
  pinMode(pwrPin, INPUT);
  pinMode(outputPtt, OUTPUT);
  pinMode(outputPwr, OUTPUT);
  digitalWrite(outputPwr,LOW);
  digitalWrite(outputPtt,LOW);
  Serial.begin(115200);
  Serial1.begin(19200, SERIAL_8N1, 16, 17); //IC-706 rx16 tx 17
  Serial2.begin(9600, SERIAL_8N1, 5, 18); //nextion rx5 rx18
   
  xTaskCreatePinnedToCore(&button_task, "button_task", 1000, NULL, 1, &button_task_,1);
  xTaskCreatePinnedToCore(&send_keepalive,"keep alive",1000,NULL,1,&send_keepalive_,1);

}

void send_keepalive(void *param)
{
   for(;;){

  // uint8_t msg[] = {0xFE,0x04,0x01,0xFD}; //IC-2720
     uint8_t msg[] = {0xFE,0x0B,0x00,0xFD}; //IC-706
     Serial1.write(msg,4);
     vTaskDelay(150/ portTICK_RATE_MS);
    
  }
}

void button_task(void *param) {
  const unsigned long longPressDuration = 500;
  unsigned long pwr_buttonPressStartTime = 0; 
  for(;;){
  pwr_buttonState = digitalRead(pwrPin);
   if (pwr_buttonState != pwr_lastButtonState) {
    
    if(pwr_buttonState == LOW){
    pwr_buttonPressStartTime = millis();
    Serial.println("POWER ON");
    } else {
    unsigned long buttonPressDuration = millis() - pwr_buttonPressStartTime;
    if (buttonPressDuration >= longPressDuration) { // 1초 이상 눌렸다면
    digitalWrite(outputPwr, HIGH); // GPIO 22에 신호 출력
    delay(1000);
    digitalWrite(outputPwr, LOW);
    } 
    Serial.println("POWER OFF");
    }
    
    pwr_lastButtonState = pwr_buttonState;
   }
 }
  vTaskDelay(10);
}

void loop() {
 
  if(Serial1.available()) {
 
    while(1) {
    
      if(Serial1.available()) {
        rx_buf[rx_len] = (char)Serial1.read(); // read char from UART
        if(rx_len<BUFFER_SIZE-1) {
          rx_len++;
        }
      } 
     else {
       
       delayMicroseconds(PACK_TIMEOUT);
        if(!Serial1.available()) {
       break;
       }
     }
    
    }
    
    if (rx_buf[0] == 0xFE && rx_buf[3] == 0xFD) {
    if (rx_buf[1] == 0x00 && rx_buf[2] == 0x01) {
        digitalWrite(outputPtt, HIGH);
        //Serial.println("TX");
    } else if (rx_buf[1] == 0x00 && rx_buf[2] == 0x00) {
        digitalWrite(outputPtt, LOW);
        //Serial.println("RX");
    }
}
     
    PanelDecode();   
    rigscreen();
    //Serial.write(rx_buf, rx_len);
    rx_len = 0;
    }
  
    
 
}


void rigscreen(){
    
      String str(freq0);
      String dot = ".";
      FreqData = freq0; 
      freq1 = FreqData.substring(0,3);
      freq2 = FreqData.substring(3,6);
      freq3 = FreqData.substring(6,8);
      freq4 = FreqData.substring(8,9);
      String freq = freq1 + dot + freq2 + dot + freq3;
      String sub(menu);
      menudata = menu;
      menu1 = menudata.substring(0,2);
      menu2 = menudata.substring(2,5);
      menu3 = menudata.substring(5,8);
      menu4 = menudata.substring(8,11);
      
      String mchstr(mch);
      mchdata = mch;
      String rigdata;
      String index = "RIGDATA";
      rigdata = index + freq + vfoab + modeset + menu + mchdata + ts + meter + smeter;
      
       if (rigdata != prevRigdata) {
        //Serial.println(rigdata);
        //Serial2.println(rigdata);
        prevRigdata = rigdata; 
        }
       
       Serial2.printf("t0.txt=\"%s\"",freq);
       for(int i=0; i<3; i++){
       Serial2.write(0xff); 
       }
       Serial2.printf("t11.txt=\"%s\"",freq4);
       for(int i=0; i<3; i++){
       Serial2.write(0xff); 
       }
       Serial2.printf("t4.txt=\"%s\"",vfoab);
       for(int i=0; i<3; i++){
       Serial2.write(0xff); 
       }
       Serial2.printf("t10.txt=\"%s\"",modeset);
       for(int i=0; i<3; i++){
       Serial2.write(0xff); 
       }
       Serial2.printf("t1.txt=\"%s\"",mchdata);
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       Serial2.printf("t6.txt=\"%s\"",menu1);
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       Serial2.printf("t7.txt=\"%s\"",menu2);
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       Serial2.printf("t8.txt=\"%s\"",menu3);
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       Serial2.printf("t9.txt=\"%s\"",menu4);
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }  
              
       if(meter == "ALC"){
       Serial2.printf("vis alc,1");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       }else if(meter == "SWR"){
       Serial2.printf("vis swr,1");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       }else if(meter == "PO "){
       Serial2.printf("vis po,1");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       }

       if(ts == "10hz"){
       Serial2.printf("vis p1,0");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       Serial2.printf("vis p2,0");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       Serial2.printf("vis p3,0");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       Serial2.printf("vis t11,1");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       
       }else if(ts == "Khz"){
       Serial2.printf("vis p1,1");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       Serial2.printf("vis p2,0");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       Serial2.printf("vis p3,0");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       Serial2.printf("vis t11,0");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       }else if(ts == "Mhz"){
       Serial2.printf("vis p1,0");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       Serial2.printf("vis p2,1");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       Serial2.printf("vis p3,0");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       Serial2.printf("vis t11,0");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       }else if(ts == "10Mhz"){
       Serial2.printf("vis p1,0");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       Serial2.printf("vis p2,1");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       Serial2.printf("vis p3,1");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       Serial2.printf("vis t11,0");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       }
       if(rx_buf[4] == 0x08 || rx_buf[4] == 0x09 || rx_buf[4] == 0x0A || 
          rx_buf[4] == 0x28 || rx_buf[4] == 0x29 || rx_buf[4] == 0x2A ||
          rx_buf[4] == 0x48 || rx_buf[4] == 0x49 || rx_buf[4] == 0x4A){
       Serial2.printf("b8.pco=63488");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       }else if(rx_buf[4] != 0x00 || rx_buf[4] != 0x20){
       Serial2.printf("b8.pco=65535");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       } 
       }

       if(rx_buf[4] == 0x01 || rx_buf[4] == 0x09 || rx_buf[4] == 0x0A || 
          rx_buf[4] == 0x21 || rx_buf[4] == 0x29 || rx_buf[4] == 0x41 ||
          rx_buf[4] == 0x41 || rx_buf[4] == 0x49){
       Serial2.printf("b14.pco=2016");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       }else if(rx_buf[4] == 0x02 || rx_buf[4] == 0x0A || rx_buf[4] == 0x22 || 
                rx_buf[4] == 0x2A || rx_buf[4] == 0x4A || rx_buf[4] == 0x42){
       Serial2.printf("b14.pco=63488");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       } else if(rx_buf[4] != 0x00 || rx_buf[4] != 0x01 || rx_buf[4] !=0x02){
       Serial2.printf("b14.pco=65535");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       } 
       }
       
       if(rx_buf[4] == 0x20 || rx_buf[4] == 0x21 || rx_buf[4] == 0x22 ||
          rx_buf[4] == 0x28 || rx_buf[4] == 0x29 || rx_buf[4] == 0x2A ){
       Serial2.printf("vis rx,1");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       }else {
        if(rx_buf[4] !=0 || rx_buf[4] != 0x20){
        Serial2.printf("vis rx,0");
        for(int i=0; i<3; i++){
        Serial2.write(0xff);
        }
       }
       }
       
       if(rx_buf[4] == 0x40 || rx_buf[4] == 0x41 || rx_buf[4] == 0x42 ||
          rx_buf[4] == 0x48 || rx_buf[4] == 0x49 || rx_buf[4] == 0x4A ){
       Serial2.printf("vis tx,1");
       for(int i=0; i<3; i++){
       Serial2.write(0xff);
       }
       }else {
        if(rx_buf[4] !=0 || rx_buf[4] != 0x40){
          Serial2.printf("vis tx,0");
        for(int i=0; i<3; i++){
        Serial2.write(0xff);
        }
        }
       }
        
       
}



void PanelDecode() {
  if((rx_buf[0] == 0xFE) && (rx_buf[1] == 0x60))
     {
    // 주파수 값 설정
    for (int i = 0; i < 8; ++i) {
      freq0[i] = rx_buf[6 + i];
    }

    // 메뉴 값 설정
    if(rx_buf[23] == 0xFF){
     menu[0] = rx_buf[25];
     menu[1] = rx_buf[26];

     menu[2] = rx_buf[27];
     menu[3] = rx_buf[28];
     menu[4] = rx_buf[29];

     menu[5] = rx_buf[31];
     menu[6] = rx_buf[32];
     menu[7] = rx_buf[33];

     menu[8] = rx_buf[35];
     menu[9] = rx_buf[36];
     menu[10] = rx_buf[37];
     } else {
     menu[0] = rx_buf[24];
     menu[1] = rx_buf[25];

     menu[2] = rx_buf[26];
     menu[3] = rx_buf[27];
     menu[4] = rx_buf[28];

     menu[5] = rx_buf[30];
     menu[6] = rx_buf[31];
     menu[7] = rx_buf[32];

     menu[8] = rx_buf[34];
     menu[9] = rx_buf[35];
     menu[10] = rx_buf[36];
     }
     

    // MCH 값 설정
    mch[0] = rx_buf[15];
    mch[1] = rx_buf[16];

    // VFO 설정
    if (rx_buf[19] == 0x21 || rx_buf[19] == 0x25) vfoab = 'B';
    else if (rx_buf[19] == 0x41 || rx_buf[19] == 0x25) vfoab = 'A';
    else if (rx_buf[19] == 0x11) vfoab = 'M';

    // 모드 설정
    switch (rx_buf[18]) {
      case 0x02: modeset = "FM  "; break;
      case 0x06: modeset = "WFM "; break;
      case 0x00: modeset = "LSB "; break;
      case 0x80: modeset = "USB "; break;
      case 0x40: modeset = "CW  "; break;
      case 0x60: modeset = "CW-R"; break;
      case 0x10: modeset = "RTTY"; break;
      case 0x08: modeset = "AM  "; break;
    }
   

    // 주파수 단위 설정
    switch (rx_buf[17]) {
      case 0xC0: ts = "10hz "; break;
      case 0xC8: ts = "Khz  "; break;
      case 0xD0: ts = "Mhz  "; break;
      case 0xF0: ts = "10Mhz"; break;
    }

    // 미터 설정
    switch (rx_buf[21]) {
      case 0x24: meter = "PO "; break;
      case 0x28: meter = "ALC"; break;
      case 0x33: meter = "SWR"; break;
    }

    // 신호 미터 설정
    if (rx_buf[22] == 0x00 && rx_buf[23] == 0x00) {
    smeter = " 0";
  } else if (rx_buf[22] == 0x40 && rx_buf[23] == 0x00) {
    smeter = " 1";
  } else if (rx_buf[22] == 0x60 && rx_buf[23] == 0x00) {
    smeter = " 2";
  } else if (rx_buf[22] == 0x70 && rx_buf[23] == 0x00) {
    smeter = " 3";
  } else if (rx_buf[22] == 0x68 && rx_buf[23] == 0x00) {
    smeter = " 4";
  } else if (rx_buf[22] == 0x7C && rx_buf[23] == 0x00) {
    smeter = " 5";
  } else if (rx_buf[22] == 0x7E && rx_buf[23] == 0x00) {
    smeter = " 6";
  } else if (rx_buf[22] == 0x7F && rx_buf[23] == 0x00) {
    smeter = " 7";
  } else if (rx_buf[22] == 0x7F && rx_buf[23] == 0x80) {
    smeter = " 8";
  } else if (rx_buf[22] == 0x7F && rx_buf[23] == 0xC0) {
    smeter = " 9";
  } else if (rx_buf[22] == 0x7F && rx_buf[23] == 0xE0) {
    smeter = "10";
  } else if (rx_buf[22] == 0x7F && rx_buf[23] == 0xF0) {
    smeter = "11";
  } else if (rx_buf[22] == 0x7F && rx_buf[23] == 0xF8) {
    smeter = "12";
  } else if (rx_buf[22] == 0x7F && rx_buf[23] == 0xFC) {
    smeter = "13";
  } else if (rx_buf[22] == 0x7F && rx_buf[23] == 0xFF && rx_buf[24] == 0x0E) {
    smeter = "14";
  } else if (rx_buf[22] == 0x7F && rx_buf[23] == 0xFF && rx_buf[24] == 0x0F) {
    smeter = "15";
  }
 }
}
