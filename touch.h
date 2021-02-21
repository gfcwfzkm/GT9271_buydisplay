/*
 * touch.h
 *
 * Created: 07.01.2021 10:36:15
 *  Author: M02875
 */ 


#ifndef TOUCH_H_
#define TOUCH_H_

#include <inttypes.h>

enum touch_error {
	TOUCH_NO_ERROR	= 0,
	TOUCH_IOERR		= 1
};

typedef struct tp_point{
	uint16_t x;
	uint16_t y;
	uint16_t s;
} tp_point_t;

typedef struct touch
{
	uint8_t touchAvailable:1;
	uint8_t touchPoints:4;
	tp_point_t sampledTouch[10];		// 10-points touch capabilities - data is in here.
	void *ioInterface;					// Pointer to the IO/Peripheral Interface library
	// Any return value by the IO interface functions have to return zero when successful or
	// non-zero when not successful.
	uint8_t (*startTransaction)(void*);	// Prepare the IO/Peripheral Interface for a transaction
	uint8_t (*sendBytes)(void*,			// Send data function pointer: InterfacePointer,
	uint8_t,		// Address of the PortExpander (8-Bit Address Format!),
	uint8_t*,		// Pointer to send buffer,
	uint8_t);		// Amount of bytes to send
	uint8_t (*getBytes)(void*,			// Get data function pointer:InterfacePointer,
	uint8_t,		// Address of the PortExpander (8-Bit Address Format!),
	uint8_t*,		// Pointer to receive buffer,
	uint8_t);		// Amount of bytes to receive
	uint8_t (*endTransaction)(void*);	// Finish the transaction / Release IO/Peripheral
} touch_t;

void tp_initStruct(touch_t* inst, uint16_t dispWidth, uint16_t dispHeight,
				void *ioInterface, uint8_t (*startTransaction)(void*),
				uint8_t (*sendBytes)(void*,uint8_t,uint8_t*,uint8_t),
				uint8_t (*getBytes)(void*,uint8_t,uint8_t*,uint8_t),
				uint8_t (*endTransaction)(void*));

enum touch_error tp_initChip(touch_t *inst);

enum touch_error tp_isTouched(touch_t *inst);

enum touch_error tp_readTouch(touch_t *inst);

#endif /* TOUCH_H_ */