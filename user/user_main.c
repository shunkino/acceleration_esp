/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2014/1/1, v1.0 create this file.
 *******************************************************************************/
#include "ets_sys.h"
#include "osapi.h"
#include "driver/i2c_master.h"
#include "user_interface.h"
#include "user_devicefind.h"
#include "user_webserver.h"
#if ESP_PLATFORM
#include "user_esp_platform.h"
#endif

#define DEV_ADDR 0x1D
#define REG_ADDR 0x32
/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
 *******************************************************************************/
uint8_t i, j, ack, x, y, z;
uint8_t d[256];	
int temp[6];

void read_reg(uint8_t reg_addr) {
	i2c_master_start();
	i2c_master_writeByte((DEV_ADDR << 1) + 0);
	ack = i2c_master_checkAck();
	if (!ack) {
		os_printf("addr not ack when tx write command \n");
		i2c_master_stop();
		/* return false; */
	} 
	i2c_master_writeByte(reg_addr);
	ack = i2c_master_checkAck();
	if (!ack) {
		os_printf("register addr not ack when tx write command \n");
		i2c_master_stop();
		/* return false; */
	}
	os_delay_us(40000);
	/* i2c_master_send_ack(); */
	i2c_master_stop();
	i2c_master_start();
	i2c_master_writeByte((DEV_ADDR << 1) + 1);
	ack = i2c_master_checkAck();
	if (!ack) {
		os_printf("read device addr not ack when tx write command \n");
		i2c_master_stop();
		/* return false; */
	}
	os_delay_us(40000);
	for (i = 0; i < 6; ++i) {
		temp[i] = i2c_master_readByte();
		if (i == 5) {
			i2c_master_send_nack();	
		} else {
			i2c_master_send_ack();	
		}	
	}
	x = temp[1] << 8 | temp[0];
	y = temp[3] << 8 | temp[2];
	z = temp[5] << 8 | temp[4];
	os_printf("x:%d y:%d z:%d \n", x, y, z);
	i2c_master_stop();
}

void write_to(uint8_t reg_addr, uint8_t val) {
	i2c_master_start();
	i2c_master_writeByte((DEV_ADDR << 1) + 0);
	ack = i2c_master_checkAck();
	if (!ack) {
		os_printf("write device addr not ack when tx write command \n");
		i2c_master_stop();
	}
	os_delay_us(40000);
	i2c_master_writeByte(reg_addr);
	ack = i2c_master_checkAck();
	if (!ack) {
		os_printf("write register addr not ack when tx write command \n");
		i2c_master_stop();
	}
	os_delay_us(40000);
	i2c_master_writeByte(val);
	ack = i2c_master_checkAck();
	if (!ack) {
		os_printf("write val not ack when tx write command \n");
		i2c_master_stop();
	}
	i2c_master_stop();
}

void user_init(void) {
	os_printf("SDK version:%s\n", system_get_sdk_version());
	i2c_master_gpio_init();	
	write_to(0x2d, 0x08);
	while(true) {
		read_reg(REG_ADDR);
	}
}
