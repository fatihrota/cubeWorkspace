/*
 * conf.h
 *
 *  Created on: Nov 8, 2017
 *      Author: LeventK
 */

#ifndef CONF_H_
#define CONF_H_

#ifndef RT_BOOTLOADER
#include "conf_board.h"

extern tConfig gConfig;

void conf_save();
void conf_reload();
void conf_init();
#endif

#endif /* CONF_H_ */
