#include <avr/wdt.h>

int loop_delay = 300; //задержка главного цикла
int input_LU = 2; // вывод для получения LU
int output_LU = A0; // вывод для передачи LU
volatile int imp_cnt = 0; // счётчик импульсов от домофона
volatile int imp_cnt_bkp = 0; // предыдущее значение счетчика импульсов
int hall_relay = 4; // номер вывода для активации реле подъезда 
int local_relay_LN = 3; // управление локальным реле
int local_relay_LU = A1; // управление локальным реле
int talk_flag = 0;
long loop_cnt = 0;

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
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  // 2 подъезд
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  // 3 подъезд
  pinMode(6, OUTPUT);
  digitalWrite(6, HIGH);
  // 4 подъезд
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
  // 5 подъезд
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  // 6 подъезд
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  // 7 подъезд
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  // 8 подъезд
  pinMode(11, OUTPUT);
  digitalWrite(11, HIGH);
  // 9 подъезд
  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);
  // 10 подъезд
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  
  attachInterrupt(digitalPinToInterrupt(input_LU), LU_increment, FALLING);

  Serial.begin(115200);
  while (!Serial) {
    delay(100);
  }
  Serial.println("loaded");
}

void LU_increment() {
  //пришел сигнал сброса после окончания разговора
  if(talk_flag == 1) {
    imp_cnt_bkp++;
    return;
  }
  else {
    imp_cnt_bkp = imp_cnt;
  }
  imp_cnt++;
}

void LU_reset() {
    Serial.println("Выполняем LU_reset");
    digitalWrite(output_LU, LOW);
    delay(300);
    digitalWrite(output_LU, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    digitalWrite(8, HIGH);
    digitalWrite(9, HIGH);
    digitalWrite(10, HIGH);
    digitalWrite(11, HIGH);
    digitalWrite(12, HIGH);
    digitalWrite(13, HIGH);
    digitalWrite(local_relay_LN, HIGH);
    digitalWrite(local_relay_LU, HIGH);
    imp_cnt_bkp = imp_cnt = talk_flag = loop_cnt = 0;
}
void send_LU(int cnt) {
  //Serial.println("Выполняем send_LU");
  digitalWrite(local_relay_LU, LOW);
  digitalWrite(output_LU, LOW);
  delay(300);
  digitalWrite(output_LU, HIGH);
  delay(60);
  for( int i = 0; i <= cnt; i++ ) {
    digitalWrite(output_LU, LOW);
    delay(6);
    digitalWrite(output_LU, HIGH);
    delay(6);
  }
}

void loop() {
  wdt_reset();
  loop_cnt++;
  if(loop_cnt > 1800000) {
    LU_reset();
  }
  //пришел сигнал сброса по окончани разговора
  if(imp_cnt_bkp > imp_cnt) {
    LU_reset();
  }
  if(imp_cnt == 0) {
    delay(3);
    return; 
  }
  delay(600);
  if(imp_cnt == imp_cnt_bkp and talk_flag == 0) {
    imp_cnt = imp_cnt_bkp = (imp_cnt - 1);
    ///////// test !!!!!!!!!!!!
    Serial.println(imp_cnt);
    //imp_cnt = imp_cnt_bkp = 0;
    //return;
    ///////// test!!!!!!!!!!!!!!!!!
    talk_flag = 1;
    //1 подъезд ( 1 - 25 )
    if(imp_cnt < 26) {
      //Serial.println("Подъезд 1");
      send_LU(imp_cnt);
      hall_relay = 4;
    }
    //2 подъезд ( 26 - 59 )
    else if(imp_cnt >= 26 and imp_cnt < 60) {
      send_LU( (imp_cnt) );
      hall_relay = 5;
    }
    //3 подъезд ( 60 - 83 )
    else if(imp_cnt >= 60 and imp_cnt < 84) {
      send_LU( (imp_cnt) );
      hall_relay = 6;
    }
    //4 подъезд ( 84 - 99 )
    else if(imp_cnt >= 84 and imp_cnt < 100) {
      send_LU( (imp_cnt) );
      hall_relay = 7;
    }
    //5 подъезд ( 100 - 115 )
    else if(imp_cnt >= 100 and imp_cnt < 116) {
      send_LU( (imp_cnt - 100) );
      hall_relay = 8;
    }
    //6 подъезд ( 116 - 139 )
    else if(imp_cnt >= 116 and imp_cnt < 140) {
      send_LU( (imp_cnt - 100) );
      hall_relay = 9;
    }
    //7 подъезд ( 140 - 173 )
    else if(imp_cnt >= 140 and imp_cnt < 174) {
      send_LU( (imp_cnt - 100) );
      hall_relay = 10;
    }
    //8 подъезд ( 174 - 197 )
    else if(imp_cnt >= 174 and imp_cnt < 198) {
      send_LU( (imp_cnt - 100) );
      hall_relay = 11;
    }
    //9 подъезд ( 198 - 216 )
    else if(imp_cnt >= 198 and imp_cnt < 217) {
      if(imp_cnt < 200) {
        send_LU( (imp_cnt - 100) );
      }
      else {
        send_LU( (imp_cnt - 200) );
      }
      hall_relay = 12;
    }
    //10 подъезд ( 217 - 235 )
    else if(imp_cnt >= 217 and imp_cnt < 236) {
      send_LU( (imp_cnt - 200) );
      hall_relay = 13;
    }
    // офис УК (8 подъезд кв 36 на коммутаторе)
    else if(imp_cnt == 236) {
      send_LU( (imp_cnt - 200) );
      hall_relay = 11;
    }
    imp_cnt = imp_cnt_bkp = 0;
    digitalWrite(hall_relay, LOW);
    delay(30);
    digitalWrite(local_relay_LN, LOW);
    //Serial.println("Реле!!!!");
  }
  imp_cnt_bkp = imp_cnt;
}
