/*
 * touch.c
 *
 * Created: 07.01.2021 10:36:26
 *  Author: gfcwfzkm
 */ 

#include "touch.h"

#define Key_Down 0x01
#define Key_Up   0x00

#define MERGE(hi,lo)	(((uint16_t)hi << 8) | lo)

#define WRITE_ADD	0xba
#define READ_ADD	0xbb
#define GTP_READ_COOR_ADDR    0x814e
#define GTP_REG_SLEEP         0x8040
#define GTP_REG_SENSOR_ID     0x814a
#define GTP_REG_CONFIG_DATA   0x8047
#define GTP_REG_VERSION       0x8440

uint8_t recBuf[80];
uint16_t dispW;
uint16_t dispH;
/* config data for the touch chip. Due to lack of datasheet, no idea what it does? */
const __memx uint8_t GTP_CFG_DATA[188]=
{
	0x80,0x47,0x63,			// Register Config Data Address + config version 'c'
	0x00,0x04,0x58,0x02,	// Set Screen Width and Height (LowByte + HighByte)
	0x0A,					// Amount of Touch Inputs / Numbers (10 in this case)
	0x3D,0x00,
	0x01,0x08,0x28,0x0F,0x50,0x32,0x03,0x05,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x17,
	0x19,0x1D,0x14,0x90,0x2F,0x89,0x23,0x25,
	0xD3,0x07,0x00,0x00,0x00,0x02,0x03,0x1D,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x19,0x32,0x94,0xD5,0x02,	
	0x07,0x00,0x00,0x04,0xA2,0x1A,0x00,0x90,
	0x1E,0x00,0x80,0x23,0x00,0x73,0x28,0x00,
	0x68,0x2E,0x00,0x68,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x16,0x15,0x14,0x11,0x10,0x0F,0x0E,0x0D,
	0x0C,0x09,0x08,0x07,0x06,0x05,0x04,0x01,
	0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x29,0x28,
	0x27,0x26,0x25,0x24,0x23,0x22,0x21,0x20,
	0x1F,0x1E,0x1C,0x1B,0x19,0x14,0x13,0x12,
	0x11,0x10,0x0F,0x0E,0x0D,0x0C,0x0A,0x08,
	0x07,0x06,0x04,0x02,0x00,0xFF,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x71,0x01
};


uint8_t _tp_readReg(touch_t* inst, uint16_t regAddr, uint8_t *dBuf, uint8_t length)
{
	enum touch_error tp_err = TOUCH_NO_ERROR;
	
	
	
	return tp_err;
}

uint8_t _tp_writeReg(touch_t* inst, uint16_t regAddr, uint8_t *dBuf, uint8_t length)
{
	enum touch_error tp_err = TOUCH_NO_ERROR;
	
	tp_err |= inst->startTransaction(inst->ioInterface);
	/* Start Transaction (setting Address, reading Register)
	 * but only if the driver doesn't respond with an error (anything non-0) */
	if (!tp_err)
	{
		/*tmp_err |= inst->sendBytes(inst->ioInterface,
				mcp_Address, mcp_Buffer, MCP23_BUF_SIZE_WR);*/
	}
	// End Transaction, no matter if it failed or was successful
	tp_err |= inst->endTransaction(inst->ioInterface);
	
	return tp_err;
}

void tp_initStruct(touch_t* inst, uint16_t dispWidth, uint16_t dispHeight,
				void *ioInterface, uint8_t (*startTransaction)(void*),
				uint8_t (*sendBytes)(void*,uint8_t,uint8_t*,uint8_t),
				uint8_t (*getBytes)(void*,uint8_t,uint8_t*,uint8_t),
				uint8_t (*endTransaction)(void*))
{
	inst->touchPoints = 0;
	inst->touchAvailable = 0;
	for (uint8_t i = 0; i < 10; i++)
	{
		inst->sampledTouch[i].x = 0;
		inst->sampledTouch[i].y = 0;
	}
	dispH = dispHeight;
	dispW = dispWidth;
	inst->ioInterface = ioInterface;
	inst->startTransaction = startTransaction;
	inst->sendBytes = sendBytes;
	inst->getBytes = getBytes;
	inst->endTransaction = endTransaction;
}

enum touch_error tp_initChip(touch_t *inst)
{
	enum touch_error tp_err = TOUCH_NO_ERROR;
	uint8_t retry = 0;
	uint8_t cfg_buffer[188];
	
	inst->touchPoints = 0;
	for (retry = 0; retry < 10; retry++)
	{
		inst->sampledTouch[retry].x = 0;
		inst->sampledTouch[retry].y = 0;		
	}
	
	for (retry = 0; retry < 186; retry++)
	{
		cfg_buffer[retry] = GTP_CFG_DATA[retry];
	}
	
	for (retry = 0; retry < 5; retry++)
	{
		tp_err = TOUCH_NO_ERROR;
		tp_err |= inst->startTransaction(inst->ioInterface);
		tp_err |= inst->sendBytes(inst->ioInterface, WRITE_ADD, cfg_buffer, 188);
		tp_err |= inst->endTransaction(inst->ioInterface);
		
		// If transmission was successful, break out of the loop!
		if (tp_err == TOUCH_NO_ERROR)	break;
		// Else clear the error buffer and try 5 more times
	}
	
	return tp_err;
}

enum touch_error tp_isTouched(touch_t *inst)
{
	enum touch_error tp_err = TOUCH_NO_ERROR;
	uint8_t checkFlag[3] = {0x81,0x4e,0};
	
	tp_err |= inst->startTransaction(inst->ioInterface);
	tp_err |= inst->sendBytes(inst->ioInterface, WRITE_ADD, checkFlag, 2);
	tp_err |= inst->endTransaction(inst->ioInterface);
	checkFlag[0] = 0;
	tp_err |= inst->startTransaction(inst->ioInterface);
	tp_err |= inst->getBytes(inst->ioInterface, READ_ADD, checkFlag, 1);
	tp_err |= inst->endTransaction(inst->ioInterface);
	
	if (checkFlag[0] & 0x80)
	{
		inst->touchAvailable = 1;
	}
	else
	{
		inst->touchAvailable = 0;
	}
	
	return tp_err;
}

enum touch_error tp_readTouch(touch_t *inst)
{
	enum touch_error tp_err = TOUCH_NO_ERROR;
	uint8_t loopCnt;
	uint8_t touchPoint = 0;
	uint8_t checkFlag[3] = {0x81,0x4E,0};
	
	/* Register im Touch-Chip anwählen */
	tp_err |= inst->startTransaction(inst->ioInterface);
	tp_err |= inst->sendBytes(inst->ioInterface, WRITE_ADD, checkFlag, 2);
	tp_err |= inst->endTransaction(inst->ioInterface);
	/* Auslesen wieviel touch punkte überhaupt regristriert werden müssen */
	tp_err |= inst->startTransaction(inst->ioInterface);
	tp_err |= inst->getBytes(inst->ioInterface, READ_ADD, &checkFlag[2], 1);
	tp_err |= inst->endTransaction(inst->ioInterface);
	
	inst->touchPoints = checkFlag[2] & 0x0F;
	if (inst->touchPoints != 0)
	{
		/* Register im Touch-Chip anwählen */
		tp_err |= inst->startTransaction(inst->ioInterface);
		tp_err |= inst->sendBytes(inst->ioInterface, WRITE_ADD, checkFlag, 2);
		tp_err |= inst->endTransaction(inst->ioInterface);
		
		/* Bis zu 80 Bytes aus dem Chip auslesen */
		tp_err |= inst->startTransaction(inst->ioInterface);
		tp_err |= inst->getBytes(inst->ioInterface, READ_ADD, recBuf, inst->touchPoints * 8);
		tp_err |= inst->endTransaction(inst->ioInterface);
	
		for (loopCnt = 0; loopCnt < inst->touchPoints; loopCnt++)
		{
			touchPoint = loopCnt * 8;
			inst->sampledTouch[loopCnt].x = dispW - MERGE(recBuf[3+touchPoint],recBuf[2 +touchPoint]);
			inst->sampledTouch[loopCnt].y = dispH - MERGE(recBuf[5+touchPoint],recBuf[4+touchPoint]);
			inst->sampledTouch[loopCnt].s = MERGE(recBuf[7+touchPoint],recBuf[6+touchPoint]);
		}
	}
	
	checkFlag [2] = 0;
	/* Touch-Bit löschen */
	tp_err |= inst->startTransaction(inst->ioInterface);
	tp_err |= inst->sendBytes(inst->ioInterface, WRITE_ADD, checkFlag, 3);
	tp_err |= inst->endTransaction(inst->ioInterface);		
	return tp_err;
}