/*
 * i2c.h
 *
 *  Created on: 30. 3. 2018
 *      Author: mirek
 */

#ifndef I2C_H_2
#define I2C_H_2

#define LPI2C_MTDR_CMD_TX				(0b000)
#define LPI2C_MTDR_CMD_RX				(0b001)
#define LPI2C_MTDR_CMD_STOP				(0b010)
#define LPI2C_MTDR_CMD_RX_DIS			(0b011)
#define LPI2C_MTDR_CMD_START			(0b100)
#define LPI2C_MTDR_CMD_START_NACK		(0b101)
#define LPI2C_MTDR_CMD_START_HS			(0b110)
#define LPI2C_MTDR_CMD_START_NACK_HS	(0b111)

#endif /* I2C_H_ */
