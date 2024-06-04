#include <avr/wdt.h>

int loop_delay = 300; //задержка главного цикла
int input_LU = 2; // вывод для получения LU
int output_LU = A0; // вывод для передачи LU
volatile int imp_cnt = 0; // счётчик импульсов от домофона
volatile int imp_cnt_bkp = 0; // предыдущее значение счетчика импульсов
int hall_relay = 4; // номер вывода для активации реле подъезда 
int local_relay_LN = 3; // управление локальным реле
int local_relay_LU = A1; // управление локальным реле
volatile int talk_flag = 0;
volatile uint32_t timer;
long loop_cnt = 0;
int max_apt1 = 25*2;
int max_apt2 = 59*2;
int max_apt3 = 83*2;
int max_apt4 = 99*2;
int max_apt5 = 115*2;
int max_apt6 = 139*2;
int max_apt7 = 173*2;
int max_apt8 = 197*2;
int max_apt9 = 216*2;
int max_apt10 = 235*2;

void setup() {
  wdt_enable(WDTO_8S); //включаем watchdog
  pinMode(input_LU, INPUT_PULLUP);
  pinMode(output_LU, OUTPUT);
  digitalWrite(output_LU, HIGH);
//инициализируем управляющие выводы
  //локальное реле
  pinMode(local_relay_LN, OUTPUT);
  digitalWrite(local_relay_LN, HIGH);
  pinMode(local_relay_LU, OUTPUT);
  digitalWrite(local_relay_LU, HIGH);
  // 1 подъезд
  pinMode(4, INPUT_PULLUP);
  // 2 подъезд
  pinMode(5, INPUT_PULLUP);
  // 3 подъезд
  pinMode(6, INPUT_PULLUP);
  // 4 подъезд
  pinMode(7, INPUT_PULLUP);
  // 5 подъезд
  pinMode(8, INPUT_PULLUP);
  // 6 подъезд
  pinMode(9, INPUT_PULLUP);
  // 7 подъезд
  pinMode(10, INPUT_PULLUP);
  // 8 подъезд
  pinMode(11, INPUT_PULLUP);
  // 9 подъезд
  pinMode(12, INPUT_PULLUP);
  // 10 подъезд
  pinMode(13, INPUT_PULLUP);

  delay(2500);
  
  attachInterrupt(digitalPinToInterrupt(input_LU), LU_increment, CHANGE);

  //Serial.begin(115200);
  //while (!Serial) {
  // delay(100);
  //}
  //Serial.println("loaded");
}

void LU_increment() {
  //пришел сигнал сброса после окончания разговора
  if(talk_flag == 1) {
    if( (micros() - timer) < 380000 and (micros() - timer) > 230000) {
      imp_cnt_bkp++;
      return;
    }
    timer = micros();
  }
  else { 
    imp_cnt_bkp  = imp_cnt;
  }
  if( (micros() - timer) < 300 and (micros() - timer) > 100) {
    imp_cnt++;
  }
  timer = micros();
}

void LU_reset(int reason) {
    //Serial.print("Выполняем LU_reset ");
    //Serial.println(reason);
    digitalWrite(output_LU, LOW);
    delay(300);
    digitalWrite(output_LU, HIGH);
    pinMode(4, INPUT_PULLUP);
    pinMode(5, INPUT_PULLUP);
    pinMode(6, INPUT_PULLUP);
    pinMode(7, INPUT_PULLUP);
    pinMode(8, INPUT_PULLUP);
    pinMode(9, INPUT_PULLUP);
    pinMode(10, INPUT_PULLUP);
    pinMode(11, INPUT_PULLUP);
    pinMode(12, INPUT_PULLUP);
    pinMode(13, INPUT_PULLUP);
    digitalWrite(local_relay_LN, HIGH);
    digitalWrite(local_relay_LU, HIGH);
    delay(2500);
    imp_cnt_bkp = imp_cnt = talk_flag = loop_cnt = 0;
}
void send_LU(int cnt, int hall_relay) {
  //Serial.println("Выполняем send_LU");
  digitalWrite(local_relay_LU, LOW);
  pinMode(hall_relay, OUTPUT);
  digitalWrite(hall_relay, LOW);
  delay(5);
  digitalWrite(output_LU, LOW);
  delay(300);
  digitalWrite(output_LU, HIGH);
  delay(50);
  for( int i = 1; i <= cnt; i++ ) {
    digitalWrite(output_LU, LOW);
    delayMicroseconds(180);
    digitalWrite(output_LU, HIGH);
    delayMicroseconds(280);
  }
}

void loop() {
  wdt_reset();
  loop_cnt++;
  if(loop_cnt > 1800000) {
    LU_reset(0);
  }
  //пришел сигнал сброса по окончани разговора
 if(imp_cnt_bkp > imp_cnt) {
    LU_reset(1);
 }
  if(imp_cnt == 0) {
    delay(1);
    return; 
  }
  delay(500);
  if(imp_cnt == imp_cnt_bkp and talk_flag == 0) {
    if(imp_cnt < 2) {
      LU_reset(2);
      return;
    }
    imp_cnt = imp_cnt_bkp = (imp_cnt + 1)/2;
    //1 подъезд ( 1 - 25)
    if(imp_cnt <= max_apt1) {
      //Serial.println("hall 1");
      hall_relay = 4;
      send_LU(imp_cnt, hall_relay);
    }
    //2 подъезд ( 26 - 59 * 2 )
    else if(imp_cnt > max_apt1 and imp_cnt <= max_apt2) {
      //Serial.println("hall 2");
      hall_relay = 5;
      send_LU(imp_cnt, hall_relay);
    }
    //3 подъезд ( 60 - 83 )
    else if(imp_cnt > max_apt2 and imp_cnt <= max_apt3) {
      //Serial.println("hall 3");
      hall_relay = 6;
      send_LU(imp_cnt, hall_relay);
    }
    //4 подъезд ( 84 - 99 )
    else if(imp_cnt > max_apt3 and imp_cnt <= max_apt4) {
      //Serial.println("hall 4");
      hall_relay = 7;
      send_LU(imp_cnt, hall_relay);
    }
    //5 подъезд ( 100 - 115 )
    else if(imp_cnt > max_apt4 and imp_cnt <= max_apt5) {
      //Serial.println("hall 5");
      hall_relay = 8;
      send_LU((imp_cnt - 100*2), hall_relay);
    }
    //6 подъезд ( 116 - 139 )
    else if(imp_cnt > max_apt5 and imp_cnt <= max_apt6) {
      //Serial.println("hall 6");
      hall_relay = 9;
      send_LU((imp_cnt - 100*2), hall_relay);
    }
    //7 подъезд ( 140 - 173 )
    else if(imp_cnt > max_apt6 and imp_cnt <= max_apt7) {
      //Serial.println("hall 7");
      hall_relay = 10;
      send_LU((imp_cnt - 100*2), hall_relay);
    }
    //8 подъезд ( 174 - 197 )
    else if(imp_cnt > max_apt7 and imp_cnt <= max_apt8) {
      //Serial.println("hall 8");
      hall_relay = 11;
      send_LU((imp_cnt - 100*2), hall_relay);
    }
    //9 подъезд ( 198 - 216 )
    else if(imp_cnt > max_apt8 and imp_cnt < max_apt9) {
      //Serial.println("hall 9");
      hall_relay = 12;
      if(imp_cnt < 200*2) {
        send_LU((imp_cnt - 100*2), hall_relay);
      }
      else {
        send_LU((imp_cnt - 200*2), hall_relay);
      }
    }
    //10 подъезд ( 217 - 235 )
    else if(imp_cnt >= max_apt9 and imp_cnt <= max_apt10) {
      //Serial.println("hall 10");
      hall_relay = 13;
      send_LU((imp_cnt - 200*2), hall_relay);
    }
    // офис УК (8 подъезд кв 236 на коммутаторе)
    else if(imp_cnt == 236*2) {
      //Serial.println("office");
      hall_relay = 11;
      send_LU((imp_cnt - 200*2), hall_relay);
    }
    else if(imp_cnt > max_apt10) {
      LU_reset(3);
      return;
    }
    talk_flag = 1;
    //Serial.println(imp_cnt);
    digitalWrite(local_relay_LN, LOW);
    imp_cnt = imp_cnt_bkp = 0;
    //Serial.println("Реле!!!!");
  }
  imp_cnt_bkp = imp_cnt;
}
