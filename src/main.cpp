#include "defaults.h"
#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>

#define PWM_PIN D4
#define SERVO_PIN D5


#define SCL D2
#define SCK D1

#define VPIN D3

byte I2C_MODE = 0x00;
byte I2C_ADDR = 0x01;
Servo sv;

void log(const char * flag, String msg) {
  Serial.write(flag);
  Serial.println(msg);
}
void i2cRcvHandler(int msg_size) {  
  char* buffer = "";
  
  for(int i = 0; i< msg_size; i++){
    buffer += Wire.read();    
  }
  
  log(DATA_ARRIVAL, String(buffer));
}

// Handle a request from master
void i2cReqHandler() {
  log(DATA_ARRIVAL, String("Request from master"));
}

void initAsMaster() {
  if (I2C_MODE != 0x00) {
    log(CMD_ERR, "I2C mode already initialized. Restart first");
    return;
  }
  I2C_MODE = 0x01;
  Wire.begin();
}

void initAsSlave() {
  I2C_MODE = 0x02;
  Wire.begin(I2C_ADDR);
  Wire.onReceive(i2cRcvHandler);
  Wire.onRequest(i2cReqHandler);
}

void scanBus() {
  byte error, address;  
  log(DEBUG_MSG, "Scanning...");
  char* devices = "";

  for(address = 1; address < 127; address++ ) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0) {
      log(DEBUG_MSG, "I2C device found" + String(address, HEX));
      devices += address;
    } else if (error == 4) {
      log(DEBUG_MSG, "Unknown error at address " + String(address,HEX));      
    }    
  }

  Serial.println("scan>" + String(devices));
}


void i2c_send(byte addr, byte frame[]) {
  log(DEBUG_MSG, "Sending to i2c");
  Wire.beginTransmission(addr);  
  for (int i = 0; i < sizeof(frame); i++){
    Wire.write(frame[i]);
  }
  Wire.endTransmission();
}

void parseCmd(byte cmd, byte data[]) {
  switch (cmd) {
    case SET_PWM_FREQ: {
      int f = data[0] | (data[1] << 8);
      analogWriteFreq(f);
      log(DEBUG_MSG, String("Pwm frequency set to ") + String(f) + String("hz"));
      break;
    }

    case SET_PWM_DUTY: {
      analogWrite(PWM_PIN, data[0]);
      log(DEBUG_MSG, String("Pwm duty set to ") + String(data[0]));
      break;
    }

    case SET_SERVO_POS: {
      sv.write(data[0]);
      break;
    }

    case SET_IIC_ADDR: {
      I2C_ADDR = data[0];
      log(DEBUG_MSG, String("I2C address set to ") + String(data[0]));    
      break;
    }
    
    case SET_IIC_CLK: {
      uint32_t clk = data[0] | data[1] << 8 | data[2] <<16 | data[3] <<24;
      Wire.setClock(clk);
      log(DEBUG_MSG, String("I2C clock set to ") + String(clk));    
      break;
    }

    //TODO:: make it restart and use the stored eeprom addr
    case SET_IIC_MODE: {
      if (data[0] == 0) {
        initAsMaster();
        log(DEBUG_MSG, "Initialized I2C bus as master");
      } else if (data[0] == 0xFF) {        
        initAsSlave();
        log(DEBUG_MSG, "Initialized I2C bus as slave on addr: " + String(I2C_ADDR, HEX));
      } else { 
        log(CMD_ERR, String("I2C mmode not valid: ") + String(data[0], HEX)); 
      }
      break;
    }

    case SET_IIC_VOLT: {
      if (data[0] == 0) {
        digitalWrite(VPIN, 1);
        log(DEBUG_MSG, "I2C voltage set to 3.3v");
      } else if (data[0] == 0xFF) {        
        digitalWrite(VPIN, 0);
        log(DEBUG_MSG, "I2C voltage set to 5v");
      } else { 
        log(CMD_ERR, String("I2C voltage not valid: ") + String(data[0], HEX)); 
      }
      break;
    }

    case SCAN: {
      scanBus();
      break;
    }

    default:
     log(CMD_ERR, String("Command ") + String(cmd, HEX) + String(" not recognized"));
  }
}



void setup(void) {  
  // sv.attach(SERVO_PIN);
  Serial.begin(230400);
  pinMode(PWM_PIN, OUTPUT);
  pinMode(SERVO_PIN, OUTPUT);
  log(DEBUG_MSG, String("Device Ready"));
}

void loop(void) {  
  static byte buffer[MAX_BUFF_LEN + HEADER_SIZE];
  static byte data[MAX_BUFF_LEN];
  static byte cmd = 0;
  static int cursor = 0;
  static int length = 0;

  if (Serial.available() > 0) {         
    log(DEBUG_MSG, String(cursor));
    buffer[cursor] = Serial.read();
    cursor++;

    if (cursor > 0x01) { //header ready
      if (cursor == 0x02) {
        cmd = buffer[0] & 0b00111111;
        length = buffer[1] << 2 | buffer[0] >> 6;
        
        log(DEBUG_MSG, String("Header complete "));
        log(DEBUG_MSG, String("CMD ") + String(cmd ,HEX));
        log(DEBUG_MSG, String("Message length ") + String(length ,HEX));
      }

      if (cursor == (length + HEADER_SIZE)) {
        for (int i = 2; i <= length + HEADER_SIZE; i++) {
          data[i] = buffer[i];
        }
        log(CMD_ACK, String(cmd) + String((char *) data));
        parseCmd(cmd, data);
        cursor = 0;
        length = 0;        
        memset(data, 0, MAX_BUFF_LEN);
        return;
      }
    }
  }
}