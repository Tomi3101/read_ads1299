/*
 * ads1299.h
 *
 *  Created on: Jun 14, 2023
 *      Author: Fede
 */

#ifndef ADS1299_H_
#define ADS1299_H_



/*
 * INCLUDES
 */
#include <stdint.h>
#include <stdarg.h> 				//Libreria para crear funciones variádicas

/*
 * DEFINES
 */

#define ADS_NCH 8

#define MUX_SHORT 	1
#define MUX_NORMAL 	0
#define MUX_SHORT 	1
#define MUX_TEST 	5

#define GAIN_1		0
#define GAIN_2		1
#define GAIN_4		2
#define GAIN_6		3
#define GAIN_8		4
#define GAIN_12		5
#define GAIN_24		6

#define RATE_250	6
#define RATE_500	5
#define RATE_1k		4
#define RATE_2k		3
#define RATE_4k		2
#define RATE_8k		1
#define RATE_16k	0


/*
 * TYPEDEFS
 */

typedef enum {	  // Comandos del ADS1299
	WAKEUP = 0x02,    // Wake-up from standby mode
	STANDBY = 0x04,   // Enter standby mode
	RESET_ADS = 0x06, // Reset the device
	START = 0x08,     // Start and restart (synchronize) conversions
	STOP = 0x0A,      // Stop conversion
	RDATAC = 0x10,    // Enable Read Data Continuous mode
	SDATAC = 0x11,    // Stop Read Data Continuously mode
	RDATA = 0x12      // Read data by command; supports multiple read back
}cmd;

typedef enum {	  // Comandos del ADS1299
	ID = 0,
	CONFIG1,    // Wake-up from standby mode
	CONFIG2,   // Enter standby mode
	CONFIG3, // Reset the device
	LOFF,
	CH1SET,
	CH2SET,
	CH3SET,
	CH4SET,
	CH5SET,
	CH6SET,
	CH7SET,
	CH8SET,
	BIAS_SENSP,
	BIAS_SENSN,
	LOFF_SENSP,
	LOFF_SENSN,
	LOFF_FLIP,
	LOFF_SATP,
	LOFF_STATN,
	GPIO,
	MISC1,
	MISC2,
	CONFIG4
}reg;



typedef struct{
	uint32_t head;
	int32_t ch[8];
} dato_t;

typedef struct{
	int mux:3;
	int srb_connect:1;
	int gain:3;
	int power_down:1;
}canal_bits_t;

typedef union{
	canal_bits_t bits;
	uint8_t byte;
}conf_canal_t;

typedef struct{
	int dr:3;
	int config1_7a3:5;
}config1_bits_t;

typedef union{
	config1_bits_t bits;
	uint8_t byte;
}conf_config1_t;

/*
 * DECLARACIONES
 */

void send_command(cmd comando);
void read_register(uint8_t r, uint8_t n, uint8_t *pRData);
void write_register(uint8_t r, uint8_t n, ...);
void get_data(uint8_t *pRData);
int32_t convertTo32Bit(uint8_t *byteArray);
void cambiar_config(void);
void cambiar_rate(uint8_t valor_rate);
void cambiar_gain(uint8_t valor_gain);
void cambiar_mux(uint8_t valor_mux);
void interpretar_comandos(char *comandos);
void get_data_32Bit(void);
void send_dato(void);
void ADS_init(void);
uint8_t recibir_comandos(void);

#endif /* ADS1299_H_ */
