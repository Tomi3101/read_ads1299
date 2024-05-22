/*
 * ads1299_driver.c
 *
 *  Created on: May 17, 2024
 *      Author: tomas
 */
#include "ads1299_driver.h"
#include "stm32f1xx_hal.h"
#include "main.h"


extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart3_tx;
extern SPI_HandleTypeDef hspi2;

void send_command_driver(cmd comando_d){
HAL_SPI_Transmit(&hspi2, &comando_d, 1, 100);
}

void read_register_driver(uint8_t rreg_d,uint8_t n_d,uint8_t *pRData_d,uint8_t *pTData_d){
	HAL_SPI_Transmit(&hspi2, &rreg_d, 1, 100);
	HAL_SPI_Transmit(&hspi2, &n_d, 1, 100);
	HAL_SPI_TransmitReceive(&hspi2, pTData_d, pRData_d, n_d + 1, 100);
}

void write_register_driver(uint8_t wreg_d,uint8_t n_d,va_list lista_d){
	int args;
	HAL_SPI_Transmit(&hspi2, &wreg_d, 1, 100);
	HAL_SPI_Transmit(&hspi2, &n_d, 1, 100);
	for (int i = 0; i <= n_d; i++) {
		args = va_arg(lista_d, int);
		HAL_SPI_Transmit(&hspi2, (uint8_t*) &args, 1, 100);
	}
}

void get_data_driver(uint8_t *pRData_d,uint8_t *pTxData_d){
	HAL_SPI_TransmitReceive(&hspi2, pTxData_d, pRData_d, 27, 100);
}

void cambiar_config_driver(conf_canal_t conf_canal_d,conf_config1_t conf_config1_d){
	write_register(CONFIG1, 1, (uint8_t) conf_config1_d.byte); // WREG CONFIG1 para 250 SPS
	write_register(CH1SET, 4, conf_canal_d.byte, conf_canal_d.byte, conf_canal_d.byte, conf_canal_d.byte);
	write_register(CH5SET, 4, conf_canal_d.byte, conf_canal_d.byte, conf_canal_d.byte, conf_canal_d.byte);
}

void send_dato_driver(dato_t *dato_d){
	HAL_UART_Transmit_DMA(&huart3, (uint8_t*) dato_d, sizeof(dato_t));
}

void ADS_init_driver(uint8_t *pRxData_d,conf_canal_t conf_canal_d,conf_config1_t conf_config1_d){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ADS_RST_GPIO_Port, ADS_RST_Pin, GPIO_PIN_RESET);

	int conf_correcta = 0;
	while (conf_correcta == 0) {

		// Inicialización del ADS1299

		HAL_GPIO_WritePin(ADS_RST_GPIO_Port, ADS_RST_Pin, GPIO_PIN_RESET); //
		HAL_GPIO_WritePin(ADS_PW_GPIO_Port, ADS_PW_Pin, GPIO_PIN_RESET); //
		HAL_Delay(50); //MiDelay(50);
		HAL_GPIO_WritePin(ADS_PW_GPIO_Port, ADS_PW_Pin, GPIO_PIN_SET); //
		// Inicializo el ads1299
		HAL_Delay(50); //MiDelay(50);
		HAL_GPIO_WritePin(ADS_RST_GPIO_Port, ADS_RST_Pin, GPIO_PIN_SET); //
		HAL_Delay(500); //MiDelay(50);
		HAL_GPIO_WritePin(ADS_RST_GPIO_Port, ADS_RST_Pin, GPIO_PIN_RESET); //
		HAL_Delay(100); //MiDelay(50);
		HAL_GPIO_WritePin(ADS_RST_GPIO_Port, ADS_RST_Pin, GPIO_PIN_SET); //
		HAL_Delay(500); //MiDelay(50);
		send_command(SDATAC); //	TxSPI(SDATAC);
		send_command(STOP);
		write_register(CONFIG1, 1, (uint8_t) conf_config1_d.byte);
		send_command(RESET);
		HAL_Delay(500); //MiDelay(50);
		send_command(SDATAC);
		send_command(STOP);
		HAL_Delay(5);
		write_register(CONFIG1, 1, (uint8_t) conf_config1_d.byte); // WREG CONFIG1 para 250 SPS
		write_register(CONFIG2, 1, 0xC0 | 0x15); // WREG CONFIG2 para configurar la señal de prueba interna
		write_register(CONFIG3, 1, 0xE8); // WREG CONFIG3 para referencia interna y sacar referencia por pin DRL_REF
		write_register(CH1SET, 4, conf_canal_d.byte, conf_canal_d.byte, conf_canal_d.byte, conf_canal_d.byte);
		write_register(CH5SET, 4, conf_canal_d.byte, conf_canal_d.byte, conf_canal_d.byte, conf_canal_d.byte);
		write_register(CONFIG4, 1, 0x00);
		HAL_Delay(10);
		read_register(ID, 13, pRxData_d);
		HAL_Delay(10);

		// Chequeo de configuración

		if ((pRxData_d[0] & 0x1F) == 0x1E) {
			conf_correcta = 1;
		}


		send_command(START); //START
		send_command(RDATAC); //RDATAC
	}
}


