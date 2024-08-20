#ifndef WISDOM_GATEWAY_MODULE_H
#define WISDOM_GATEWAY_MODULE_H

// Note: Any commands can fail if modem is busy
// TODO: add way to read return messages from gw_core

void gateway_init(void);

// Issues modem startup command
void gateway_start(void);

// Issues modem shutdown command
void gateway_stop(void);

// For sending packed data buffers
void gateway_send(void *data, uint size);

#endif // WISDOM_GATEWAY_MODULE_H
