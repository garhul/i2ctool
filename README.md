

## Available commands

|CMD Byte | Description | Data |
|:----:|:----------------- |---------- |
|0x01|  Set PWM Frequency  | 2 bytes [frequency] |
|0x02|  Set PWM Duty Cycle | 1 byte [duty_cycle]
|0x03|  Set Servo position | 1 byte [pos]
|0x04|  Set I2C Address | 1 byte [addr] (7 bits)
|0x05|  Set I2C Clock frequency | 4 bytes
|0x06|  Set I2C Mode (requires restart) | Master 0x00 (default), Slave 0xFF
|0x07|  Set I2C Bus Voltage| 5V: 0xFF, 3.3V 0X00 (default)
|0x08|  Scan I2C Bus | null
|0x09|  Send i2c Frame |  1B [addr] [message]
-----------------------
## Serial communication

### The commands received by the device are structured in the following fashion :

>[header][data]

### Header is structured in the following way :
  - bits 0 to 5 describe the cmd
  - bits 6 to 15 describe the data length (max 1024 bytes)


### Messages from the device :

Messages from the device will conform to the following format
and their ending is marked by a line jump (`\n`)

>[type][data]

where type is one byte and characterizes the message type:

 type byte | description | taxonomy of data
|:----:|:------------- | :----------------
| 0x00 | `CMD_ERR` |   [CMD_HEADER][DATA?] 
| 0x01 | `CMD_ACK`  |  [CMD_HEADER][DATA?]
| 0x02 | `DEBUG_MSG`|  [MESSAGE_DATA]
| 0x03 | `DATA_ARRIVAL`| [ADDR_BYTE][MESSAGE_DATA]

-----------------------




