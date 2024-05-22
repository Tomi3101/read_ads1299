/*
 * ads1299.c
 *
 *  Created on: Feb 21, 2024
 *      Author: Fede
 */

#include "ads1299.h"
#include "ads1299_driver.h"
#include "main.h"
#include <string.h>
#include <stdlib.h>

#define MAX_RX_BUFFER_SIZE 100
#define COMMAND_DELIMITER '\n'

dato_t dato;
uint8_t config[23];
conf_canal_t conf_canal;
conf_config1_t conf_config1;
uint8_t pRxData[27] = { 0 };		// Array para recibir datos del ADS1299
uint8_t pTxData[27] = { 0 };		// Array para transmitir datos al ADS1299
char rxBuffer[MAX_RX_BUFFER_SIZE];
uint8_t rxData;
uint16_t rxBufferIndex = 0;

void send_command(cmd comando) {	// Función para enviar comandos al ADS1299
	send_command_driver(comando);
}

void read_register(uint8_t r, uint8_t n, uint8_t *pRData) {	// Función para leer registros del ADS1299
	uint8_t pTData[n];
	for (int i = 0; i < n; i++) {
		pTData[i] = 0;
		pRData[i] = 0;
	}
	uint8_t rreg = 0b00100000 | r;
	n--;
	read_register_driver(rreg,n,pRData,pTData);

}



void write_register(uint8_t r, uint8_t n, ...) {	// Función para escribir registros del ADS1299
	va_list lista;
	va_start(lista, (int )n);
	uint8_t wreg = 0b01000000 | r;
	n--;
	write_register_driver(wreg,n,lista);
	va_end(lista);

}

void get_data(uint8_t *pRData) {	// Función para leer datos del ADS1299
	get_data_driver(pRData,pTxData);
}

int32_t convertTo32Bit(uint8_t *byteArray) {
	int32_t result = 0;

	// Extract the sign bit (MSB) from the 3rd byte
	int8_t sign = (byteArray[0] & 0x80) ? 0xFF : 0x00;

	// Perform sign extension to create a 32-bit signed value
	result = (sign << 24) | (byteArray[0] << 16) | (byteArray[1] << 8) | byteArray[2];

	return result;
}

void cambiar_mux(uint8_t valor_mux) {
	conf_canal.bits.mux = valor_mux;
	cambiar_config();
}

void cambiar_gain(uint8_t valor_gain) {
	conf_canal.bits.gain = valor_gain;
	cambiar_config();
}

void cambiar_rate(uint8_t valor_rate) {
	conf_config1.bits.dr = valor_rate;
	cambiar_config();
}


void cambiar_config(void) {

	send_command(SDATAC);
	send_command(STOP);

	cambiar_config_driver(conf_canal, conf_config1);

	send_command(START);
	send_command(RDATAC);

}

void interpretar_comandos(char *comandos) {
	char *token;
	const char delimiters[] = " ";

	token = strtok(comandos, delimiters);
	while (token != NULL) {
		if (strcmp(token, "gain") == 0) {
			token = strtok(NULL, delimiters);
			if (token != NULL) {
				int valor = atoi(token);
				switch (valor) {
					case 1:
						cambiar_gain(GAIN_1);
						break;
					case 2:
						cambiar_gain(GAIN_2);
						break;
					case 4:
						cambiar_gain(GAIN_4);
						break;
					case 8:
						cambiar_gain(GAIN_8);
						break;
					case 12:
						cambiar_gain(GAIN_12);
						break;
					case 24:
						cambiar_gain(GAIN_24);
						break;
					default:
						cambiar_gain(GAIN_1);
						break;
				}

			}
		}
		else if (strcmp(token, "mux") == 0) {
			token = strtok(NULL, delimiters);
			if (token != NULL) {
				char valor = *token;
				switch (valor) {
					case 'n':
						cambiar_mux(MUX_NORMAL);
						break;
					case 's':
						cambiar_mux(MUX_SHORT);
						break;
					case 't':
						cambiar_mux(MUX_TEST);
						break;
					default:
						cambiar_mux(MUX_NORMAL);
						break;
				}
			}
		}
		else if (strcmp(token, "rate") == 0) {
			token = strtok(NULL, delimiters);
			if (token != NULL) {
				int valor = atoi(token);
				switch (valor) {
					case 250:
						cambiar_rate(RATE_250);
						break;
					case 500:
						cambiar_rate(RATE_500);
						break;
					case 1000:
						cambiar_rate(RATE_1k);
						break;
					case 2000:
						cambiar_rate(RATE_2k);
						break;
					case 4000:
						cambiar_rate(RATE_4k);
						break;
					case 8000:
						cambiar_rate(RATE_8k);
						break;
					case 16000:
						cambiar_rate(RATE_16k);
						break;
					default:
						cambiar_gain(RATE_250);
						break;

				}
			}
		}
		token = strtok(NULL, delimiters);
	}

}

void get_data_32Bit(){
	int i;
	get_data(pRxData);	// Se leen los datos del ADS1299

	for (i = 0; i < ADS_NCH; i++) {
		dato.ch[i] = convertTo32Bit(&pRxData[3 * i + 3]);
	}
}

void send_dato(){
	send_dato_driver(&dato);
}

void ADS_init(){
	dato.head = 0xA5A5A5A5;
	conf_config1.bits.config1_7a3 = 0b11010;
	conf_config1.bits.dr = RATE_250;
	conf_canal.bits.power_down = 0;
	conf_canal.bits.srb_connect = 0;
	conf_canal.bits.mux = MUX_NORMAL;
	conf_canal.bits.gain = GAIN_12;

	ADS_init_driver(pRxData,conf_canal,conf_config1);
}

uint8_t recibir_comandos(){
	if (rxData != '\r') {
		rxBuffer[rxBufferIndex++] = rxData;
		if (rxBufferIndex >= MAX_RX_BUFFER_SIZE) {		// Buffer overflow handling
			rxBufferIndex = 0;
		}
	}
	else {
		// Ignore carriage return '\r'
	}

	if (rxData == COMMAND_DELIMITER) {		// Command received, process it
		rxBuffer[rxBufferIndex] = '\0';		// Null-terminate the string
		interpretar_comandos(rxBuffer);		// Process command function
		rxBufferIndex = 0;
	}
	return rxData;
}
