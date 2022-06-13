#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "rt_config.h"

void GPIO_Configure(const tGPIOInit *conf, uint32_t count);

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */
