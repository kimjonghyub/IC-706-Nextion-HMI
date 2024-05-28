
#define PACK_TIMEOUT 1000
#define BUFFER_SIZE 520 //serial data
char HMI;
char freq0[8];
char dispmenu[13];
String vfoab;
String modeset;
char VfoData;
String ModeData;
String FreqData;
String DispmenuData;
String freq1;
String freq2;
String freq3;
String freq4;
String menu1;
String menu2;
String menu3;
String menu4;
String dispmenu1;
String dispmenu2;

char menu[11];
String menudata;
char mch[2];
String mchdata;
String prevRigdata = "";
String ts;
String meter;
String smeter;
uint16_t scaleValue;

int pttPin = 36;
//int pwrPin = 39;
int pwrPin = 13;
int pwr_lastButtonState = LOW; 
int ptt_lastButtonState = LOW; 
int pwr_buttonState = 0; 
int ptt_buttonState = 0;
int outputPtt = 32;
//int outputPwr = 2;
int outputPwr = 19;
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
  //Serial1.begin(19200, SERIAL_8N1, 16, 17); //IC-706 rx16 tx 17
  //Serial2.begin(9600, SERIAL_8N1, 5, 18); //nextion rx5 rx18
 
  Serial1.begin(19200, SERIAL_8N1, 18, 23); //ic706-19200 ic2720-9600
  Serial2.begin(115200, SERIAL_8N1, 5, 21); //IO5 RX, IO21 TX 5 21 
  xTaskCreatePinnedToCore(&button_task, "button_task", 1000, NULL, 1, &button_task_,1);
  xTaskCreatePinnedToCore(&send_keepalive,"keep alive",5000,NULL,1,&send_keepalive_,1);
  

}

void send_keepalive(void *param)
{
   for(;;){

  // uint8_t msg[] = {0xFE,0x04,0x01,0xFD}; //IC-2720
     uint8_t msg[] = {0xFE,0x0B,0x00,0xFD}; //IC-706
     Serial1.write(msg,4);
     vTaskDelay(100/ portTICK_RATE_MS);
    
  }
}


 
 


void button_task(void *param) {
 
  for(;;){
  pwr_buttonState = digitalRead(pwrPin);
   if (pwr_buttonState != pwr_lastButtonState) {
    
    if(pwr_buttonState == LOW){
    digitalWrite(outputPwr, HIGH);
    //Serial.println("POWER ON");
    } else {
  
    if (pwr_buttonState == HIGH) { 
    digitalWrite(outputPwr, LOW);
    } 
    //Serial.println("POWER OFF");
    }
    
    pwr_lastButtonState = pwr_buttonState;
   }
    
    
     vTaskDelay(10);
  }
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
    PanelDecode();   
    rigscreen();
    //Serial.write(rx_buf,rx_len);
    rx_len = 0;

    }

    if (Serial.available()) { 
    char receivedChar = Serial.read(); 
    Serial1.write(receivedChar); 
    }
    
   
    if (Serial2.available()) { 
    HMI = Serial2.read(); 
    Serial1.write(HMI); 
    static String inputString = "";
    inputString += HMI;
    //Serial.print(inputString);
    if(inputString.startsWith("+")){
      digitalWrite(outputPwr, HIGH);
    }else {
  
    if (inputString.startsWith("-")) { 
    digitalWrite(outputPwr, LOW);
    } 
    }
    inputString = "";
    } 
 
}

void sendCommand(String command) {
    Serial2.printf(command.c_str());
    for (int i = 0; i < 3; i++) {
        Serial2.write(0xff);
    }
}

void endText(){
  for(int i = 0; i < 3; i++) {
        Serial2.write(0xff);
    }
}


void rigscreen(){
    
      String str1(freq0);
      String dot = ".";
      FreqData = freq0; 
      freq1 = FreqData.substring(0,3);
      freq2 = FreqData.substring(3,6);
      freq3 = FreqData.substring(6,8);
      freq4 = FreqData.substring(8,9);
      String freq = freq1 + dot + freq2 + dot + freq3;
      
      String str2(menu);
      menudata = menu;
      menu1 = menudata.substring(0,2);
      menu2 = menudata.substring(2,5);
      menu3 = menudata.substring(5,8);
      menu4 = menudata.substring(8,11);
         
      String str3(dispmenu);
      DispmenuData = dispmenu;
      dispmenu1 = DispmenuData.substring(0,2);
      dispmenu2 = DispmenuData.substring(2,13);
      
      String str4(mch);
      mchdata = mch;
      String off = "POWER OFF";
      String rigdata;
      String index = "RIGDATA";
      //rigdata = index + FreqData + vfoab + modeset + menu + mchdata + ts + meter + smeter;
        if(rx_buf[19] == 0x00){
        rigdata = index + FreqData;
        } else {
        rigdata = index + freq + vfoab + modeset + menu + mchdata + ts + meter + smeter;  
        }
      
       Serial.println(rigdata);
       /*
       if (rigdata != prevRigdata) {
       Serial.println(rigdata);
        //Serial2.println(rigdata);
        prevRigdata = rigdata; 
        }
       */
       
       
       Serial2.printf("t0.txt=\"%s\"",freq);
       endText();
       Serial2.printf("t11.txt=\"%s\"",freq4);
       endText();
       Serial2.printf("t4.txt=\"%s\"",vfoab);
       endText();
       Serial2.printf("t10.txt=\"%s\"",modeset);
       endText();
       Serial2.printf("t1.txt=\"%s\"",mchdata);
       endText();
       Serial2.printf("t6.txt=\"%s\"",menu1);
       endText();
       Serial2.printf("t7.txt=\"%s\"",menu2);
       endText();
       Serial2.printf("t8.txt=\"%s\"",menu3);
       endText();
       Serial2.printf("t9.txt=\"%s\"",menu4);
       endText();
       
       if(rx_buf[0] == 0x00){
       Serial2.printf("t0.txt=\"%s\"",off);
       endText(); 
       }       
       if(rx_buf[19] == 0x00){
       sendCommand("vis t17,1");
       Serial2.printf("t17.txt=\"%s\"",dispmenu2);
       endText();
       }else {
       sendCommand("vis t17,0"); 
       }

       Serial2.print("scale.val=");
       Serial2.print(scaleValue);
       endText();
              
       if(meter == "ALC"){
       sendCommand("vis alc,1");
       }else if(meter == "SWR"){
       sendCommand("vis swr,1");
       }else if(meter == "PO "){
       sendCommand("vis po,1");
       }

       //if(rx_buf[17] == 0xC0 || rx_buf[17] == 0xC1){
       if(ts == "10hz "){
       sendCommand("vis p1,0");
       sendCommand("vis p2,0");
       sendCommand("vis p3,0");
       sendCommand("vis t11,1");
       //}else if(rx_buf[17] == 0xC8 || rx_buf[17] == 0xC9){
       }else if(ts == "Khz  "){
       sendCommand("vis p1,1");
       sendCommand("vis p2,0");
       sendCommand("vis p3,0");
       sendCommand("vis t11,0");
       //}else if(rx_buf[17] == 0xD0 || rx_buf[17] == 0xD1){
       }else if(ts == "Mhz  "){
       sendCommand("vis p1,0");
       sendCommand("vis p2,1");
       sendCommand("vis p3,0");
       sendCommand("vis t11,0");
       //}else if(rx_buf[17] == 0xF0 || rx_buf[17] == 0xF1){
       }else if(ts == "10Mhz"){
       sendCommand("vis p1,0");
       sendCommand("vis p2,1");
       sendCommand("vis p3,1");
       sendCommand("vis t11,0");
       }
       
       if(rx_buf[4] == 0x08 || rx_buf[4] == 0x09 || rx_buf[4] == 0x0A || 
          rx_buf[4] == 0x28 || rx_buf[4] == 0x29 || rx_buf[4] == 0x2A ||
          rx_buf[4] == 0x48 || rx_buf[4] == 0x49 || rx_buf[4] == 0x4A){
       sendCommand("b8.pco=63488");
       }else if(rx_buf[4] != 0x00 || rx_buf[4] != 0x20){
       sendCommand("b8.pco=65535");
       }

       if(rx_buf[4] == 0x01 || rx_buf[4] == 0x09 || rx_buf[4] == 0x0A || 
          rx_buf[4] == 0x21 || rx_buf[4] == 0x29 || rx_buf[4] == 0x41 ||
          rx_buf[4] == 0x41 || rx_buf[4] == 0x49){
       sendCommand("b14.pco=2016");
       }else if(rx_buf[4] == 0x02 || rx_buf[4] == 0x0A || rx_buf[4] == 0x22 || 
                rx_buf[4] == 0x2A || rx_buf[4] == 0x4A || rx_buf[4] == 0x42){
       sendCommand("b14.pco=63488");
       } else if(rx_buf[4] != 0x00 || rx_buf[4] != 0x01 || rx_buf[4] !=0x02){
       sendCommand("b14.pco=65535");
       }
       
       if(rx_buf[4] == 0x20 || rx_buf[4] == 0x21 || rx_buf[4] == 0x22 ||
          rx_buf[4] == 0x28 || rx_buf[4] == 0x29 || rx_buf[4] == 0x2A ){
       sendCommand("vis rx,1");
       }else {
        if(rx_buf[4] !=0 || rx_buf[4] != 0x20){
       sendCommand("vis rx,0");
       }
       }
       
       if(rx_buf[4] == 0x40 || rx_buf[4] == 0x41 || rx_buf[4] == 0x42 ||
          rx_buf[4] == 0x48 || rx_buf[4] == 0x49 || rx_buf[4] == 0x4A ){
       sendCommand("vis tx,1");
       }else {
        if(rx_buf[4] !=0 || rx_buf[4] != 0x40){
       sendCommand("vis tx,0");
       
       }
       }

       if(rx_buf[20] == 0x00){
       sendCommand("vis nb,0");
       sendCommand("vis vox,0");
       sendCommand("vis bk,0");
       sendCommand("vis comp,0");
       sendCommand("vis fagc,0");
       } else {
        if(rx_buf[20] == 0x04 || rx_buf[20] == 0x24 || rx_buf[20] == 0x44 || 
           rx_buf[20] == 0x07 || rx_buf[20] == 0x12 || rx_buf[20] == 0x22 ||
           rx_buf[20] == 0x64 || rx_buf[20] == 0x67 || rx_buf[20] == 0x75 ||
           rx_buf[20] == 0x93 || rx_buf[20] == 0x21 || rx_buf[20] == 0x27 ||
           rx_buf[20] == 0x47){
         sendCommand("vis comp,1");
        }else if(rx_buf[20] != 0x00 || rx_buf[20] != 0x20 || rx_buf[20] != 0x40 ||
                 rx_buf[20] != 0x03 || rx_buf[20] != 0x08 || rx_buf[20] != 0x18){
          sendCommand("vis comp,0"); 
        }
        
        if(rx_buf[20] == 0x20 || rx_buf[20] == 0x24 || rx_buf[20] == 0x60 || 
           rx_buf[20] == 0x23 || rx_buf[20] == 0x28 || rx_buf[20] == 0x38 ||
           rx_buf[20] == 0x64 || rx_buf[20] == 0x67 || rx_buf[20] == 0x75 ||
           rx_buf[20] == 0x93 || rx_buf[20] == 0x21 || rx_buf[20] == 0x27 ||
           rx_buf[20] == 0x63){
            
         sendCommand("vis vox,1"); 
        }else if(rx_buf[20] != 0x00 || rx_buf[20] != 0x04 || rx_buf[20] != 0x40 ||
                 rx_buf[20] != 0x03 || rx_buf[20] != 0x08 || rx_buf[20] != 0x18 ){
          sendCommand("vis vox,0"); 
        }
        
        if(rx_buf[20] == 0x40 || rx_buf[20] == 0x44 || rx_buf[20] == 0x60 || 
           rx_buf[20] == 0x43 || rx_buf[20] == 0x48 || rx_buf[20] == 0x58 ||
           rx_buf[20] == 0x64 || rx_buf[20] == 0x67 || rx_buf[20] == 0x75 ||
           rx_buf[20] == 0x93 || rx_buf[20] == 0x21 || rx_buf[20] == 0x63 || 
           rx_buf[20] == 0x47){
         sendCommand("vis nb,1"); 
        }else if(rx_buf[20] != 0x00 || rx_buf[20] != 0x04 || rx_buf[20] != 0x20 ||
                 rx_buf[20] != 0x03 || rx_buf[20] != 0x08 || rx_buf[20] != 0x18 ){
          sendCommand("vis nb,0"); 
        }
        
        if(rx_buf[20] == 0x03 || rx_buf[20] == 0x07 || rx_buf[20] == 0x23 || 
           rx_buf[20] == 0x43 || rx_buf[20] == 0x11 || rx_buf[20] == 0x21 ||
           rx_buf[20] == 0x27 || rx_buf[20] == 0x67 || rx_buf[20] == 0x75 ||
           rx_buf[20] == 0x93 || rx_buf[20] == 0x63 || rx_buf[20] == 0x47){
         sendCommand("vis fagc,1"); 
        }else if(rx_buf[20] != 0x00 || rx_buf[20] != 0x04 || rx_buf[20] != 0x20 ||
                 rx_buf[20] != 0x40 || rx_buf[20] != 0x08 || rx_buf[20] != 0x18 ){
          sendCommand("vis fagc,0"); 
        }
        
        if(rx_buf[20] == 0x08 || rx_buf[20] == 0x12 || rx_buf[20] == 0x28 || 
           rx_buf[20] == 0x48 || rx_buf[20] == 0x11 || rx_buf[20] == 0x26 ||
           rx_buf[20] == 0x32 || rx_buf[20] == 0x72 || rx_buf[20] == 0x75 ||
           rx_buf[20] == 0x93 ){
          sendCommand("vis bk,1"); 
         
        }else if(rx_buf[20] != 0x00 || rx_buf[20] != 0x04 || rx_buf[20] != 0x20 ||
                 rx_buf[20] != 0x40 || rx_buf[20] != 0x03 || rx_buf[20] != 0x18 ){
          sendCommand("vis bk,0"); 
        }
        
        if(rx_buf[20] == 0x18 || rx_buf[20] == 0x22 || rx_buf[20] == 0x38 || 
           rx_buf[20] == 0x58 || rx_buf[20] == 0x21 || rx_buf[20] == 0x26 ||
           rx_buf[20] == 0x42 || rx_buf[20] == 0x82 || rx_buf[20] == 0x85 ||
           rx_buf[20] == 0x93 ){
         sendCommand("vis bk,1"); 
         Serial2.printf("bk.txt=\"F-BK\"");
         endText();
        }else if(rx_buf[20] != 0x00 || rx_buf[20] != 0x04 || rx_buf[20] != 0x20 ||
                 rx_buf[20] != 0x40 || rx_buf[20] != 0x03 || rx_buf[20] != 0x08 ){
          sendCommand("vis bk,0"); 
        }
        
       }
        
       
}



void PanelDecode() {
  if((rx_buf[0] == 0xFE) && (rx_buf[1] == 0x60))
     {
    // 주파수 값 설정
    for (int i = 0; i < 9; ++i) {
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
      case 0x03: modeset = "FM-T"; break;
      case 0x06: modeset = "WFM "; break;
      case 0x00: modeset = "LSB "; break;
      case 0x80: modeset = "USB "; break;
      case 0x40: modeset = "CW  "; break;
      case 0x60: modeset = "CW-R"; break;
      case 0x10: modeset = "RTTY"; break;
      case 0x08: modeset = "AM  "; break;
    }
   

    // 주파수 단위 설정
      
        if(rx_buf[18] == 0x00) {
        switch (rx_buf[17]) {
        case 0xC1: ts = "10hz "; break;
        case 0xC9: ts = "Khz  "; break;
        case 0xD1: ts = "Mhz  "; break;
        case 0xF1: ts = "10Mhz"; break;
        }
    } else {
        switch (rx_buf[17]) {
        case 0xC0: ts = "10hz "; break;
        case 0xC8: ts = "Khz  "; break;
        case 0xD0: ts = "Mhz  "; break;
        case 0xF0: ts = "10Mhz"; break;
        }
    }
    
    
    // 미터 설정
    switch (rx_buf[21]) {
      case 0x24: meter = "PO "; break;
      case 0x28: meter = "ALC"; break;
      case 0x33: meter = "SWR"; break;
    }

    // 신호 미터 설정
    if (rx_buf[22] == 0x00 && rx_buf[23] == 0x00) {
    smeter = "00";
    scaleValue=0;
  } else if (rx_buf[22] == 0x40 && rx_buf[23] == 0x00) {
    smeter = "01";
    scaleValue=3;
  } else if (rx_buf[22] == 0x60 && rx_buf[23] == 0x00) {
    smeter = "02";
    scaleValue=9;
  } else if (rx_buf[22] == 0x70 && rx_buf[23] == 0x00) {
    smeter = "03";
    scaleValue=15;
  } else if (rx_buf[22] == 0x68 && rx_buf[23] == 0x00) {
    smeter = "04";
    scaleValue=21;
  } else if (rx_buf[22] == 0x7C && rx_buf[23] == 0x00) {
    smeter = "05";
    scaleValue=27;
  } else if (rx_buf[22] == 0x7E && rx_buf[23] == 0x00) {
    smeter = "06";
    scaleValue=33;
  } else if (rx_buf[22] == 0x7F && rx_buf[23] == 0x00) {
    smeter = "07";
    scaleValue=39;
  } else if (rx_buf[22] == 0x7F && rx_buf[23] == 0x80) {
    smeter = "08";
    scaleValue=45;
  } else if (rx_buf[22] == 0x7F && rx_buf[23] == 0xC0) {
    smeter = "09";
    scaleValue=51;
  } else if (rx_buf[22] == 0x7F && rx_buf[23] == 0xE0) {
    smeter = "10";
    scaleValue=58;
  } else if (rx_buf[22] == 0x7F && rx_buf[23] == 0xF0) {
    smeter = "11";
    scaleValue=66;
  } else if (rx_buf[22] == 0x7F && rx_buf[23] == 0xF8) {
    smeter = "12";
    scaleValue=74;
  } else if (rx_buf[22] == 0x7F && rx_buf[23] == 0xFC) {
    smeter = "13";
    scaleValue=81;
  } else if (rx_buf[22] == 0x7F && rx_buf[23] == 0xFF && rx_buf[24] == 0x0E) {
    smeter = "14";
    scaleValue=89;
  } else if (rx_buf[22] == 0x7F && rx_buf[23] == 0xFF && rx_buf[24] == 0x0F) {
    smeter = "15";
    scaleValue=100;
  }

   // 셋업메뉴 값 설정
   for (int i = 0; i < 13; ++i) {
     dispmenu[i] = rx_buf[24 + i];
   }
   
   
  
 }
}
