#define MAX_BUFF_LEN 1024
#define HEADER_SIZE 2

#define CMD_ERR "!"
#define CMD_ACK ">"
#define DEBUG_MSG "D"
#define DATA_ARRIVAL "<"

// enum SERIAL_RSPS {
//   CMD_ERR,
//   CMD_ACK,
//   DEBUG_MSG,
//   DATA_ARRIVAL
// };

enum SERIAL_CODES {
  RSV,
  SET_PWM_FREQ,
  SET_PWM_DUTY,
  SET_SERVO_POS,
  SET_IIC_ADDR,
  SET_IIC_CLK,
  SET_IIC_MODE,
  SET_IIC_VOLT, 
  SCAN,
  SEND_IIC,
  COUNT
};

