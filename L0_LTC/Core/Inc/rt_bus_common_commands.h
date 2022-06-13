/*
 * rt_bus_common_commands.h
 *
 *  Created on: Nov 9, 2017
 *      Author: LeventK
 */

#ifndef SRC_RT_BUS_COMMON_COMMANDS_H_
#define SRC_RT_BUS_COMMON_COMMANDS_H_

#include "rt_bus_proto.h"

void rt_bus_cmd_process();

extern const tBusCommand 		commands[];
extern const int gCommandCount;

#endif /* SRC_RT_BUS_COMMON_COMMANDS_H_ */
