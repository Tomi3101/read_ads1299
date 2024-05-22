/*
 * ads1299_driver.h
 *
 *  Created on: May 17, 2024
 *      Author: tomas
 */

#ifndef INC_ADS1299_DRIVER_H_
#define INC_ADS1299_DRIVER_H_

#include <stdint.h>
#include <stdarg.h> 				//Libreria para crear funciones variádicas
#include "ads1299.h"


void send_command_driver(cmd comando_d);
void read_register_driver(uint8_t rreg_d,uint8_t n_d,uint8_t *pRData_d,uint8_t *pTData_d);
void write_register_driver(uint8_t wreg_d,uint8_t n_d,va_list lista_d);
void get_data_driver(uint8_t *pRData,uint8_t *pTxData);
void cambiar_config_driver(conf_canal_t conf_canal_d,conf_config1_t conf_config1_d);
void send_dato_driver(dato_t *dato_d);
void ADS_init_driver(uint8_t *pRxData_d,conf_canal_t conf_canal_d,conf_config1_t conf_config1_d);

#endif /* INC_ADS1299_DRIVER_H_ */
