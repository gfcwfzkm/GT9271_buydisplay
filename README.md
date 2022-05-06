# GT9271_buydisplay
Capacitive touchscreen driver for the RA8876 1024x600 10.1" large display, from buydisplay.com (https://www.buydisplay.com/serial-spi-i2c-10-1-inch-tft-lcd-module-dislay-w-ra8876-optl-touch-panel)


Example code:
```c
touch_t touchDisplay;
int main(void)
{
    // ... Init uC and I2C Driver
    tp_initStruct(&touchDisplay, RA8876_WIDTH, RA8876_HEIGHT, &twiMaster, &TWI_InterfacePrepare,
        &TWI_InterfaceSendBytes,&TWI_InterfaceGetBytes, &TWI_InterfaceFinish);
    if (tp_initChip(&touchDisplay))
    {
        // Chip couldn't be initialized!
        while(1){}
    }
 
    // ... somewhere down in the while loop
 
    tp_isTouched(&touchDisplay);
    if (touchDisplay.touchAvailable)
    {
        tp_readTouch(&touchDisplay);
        if (touchDisplay.touchPoints)
        {
            // Draw both a cursor and a colourful dot for each touch point
            RA8876_graphicCursorEnable();
            RA8876_graphicCursorSelect(0);
            RA8876_graphicCursorCoords(touchDisplay.sampledTouch[0].x, touchDisplay.sampledTouch[0].y);
            for (uint8_t i = 0; i < touchDisplay.touchPoints; i++)
            {
                RA8876_fillCircle(
                    touchDisplay.sampledTouch[i].x,
                    touchDisplay.sampledTouch[i].y,
                    (touchDisplay.sampledTouch->s >= 4) ? touchDisplay.sampledTouch->s >> 2 : touchDisplay.sampledTouch->s,
                    colorplate[i]);
            }
        }
        else
        {
            //RA8876_graphicCursorSelect(2);
        }
    }
}
```
