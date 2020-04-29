#include "mbed.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include <math.h>
#define UINT14_MAX        16383
// FXOS8700CQ I2C address
#define FXOS8700CQ_SLAVE_ADDR0 (0x1E<<1) // with pins SA0=0, SA1=0
#define FXOS8700CQ_SLAVE_ADDR1 (0x1D<<1) // with pins SA0=1, SA1=0
#define FXOS8700CQ_SLAVE_ADDR2 (0x1C<<1) // with pins SA0=0, SA1=1
#define FXOS8700CQ_SLAVE_ADDR3 (0x1F<<1) // with pins SA0=1, SA1=1
// FXOS8700CQ internal register addresses
#define FXOS8700Q_STATUS 0x00
#define FXOS8700Q_OUT_X_MSB 0x01
#define FXOS8700Q_OUT_Y_MSB 0x03
#define FXOS8700Q_OUT_Z_MSB 0x05
#define FXOS8700Q_M_OUT_X_MSB 0x33
#define FXOS8700Q_M_OUT_Y_MSB 0x35
#define FXOS8700Q_M_OUT_Z_MSB 0x37
#define FXOS8700Q_WHOAMI 0x0D
#define FXOS8700Q_XYZ_DATA_CFG 0x0E
#define FXOS8700Q_CTRL_REG1 0x2A
#define FXOS8700Q_M_CTRL_REG1 0x5B
#define FXOS8700Q_M_CTRL_REG2 0x5C
#define FXOS8700Q_WHOAMI_VAL 0xC7
#define PI 3.14159265
I2C i2c( PTD9,PTD8);
Ticker time_up;
Serial pc(USBTX, USBRX);
DigitalOut led(LED1);
InterruptIn btn(SW2);
float t[3];
float init[3];
EventQueue eventQueue;
EventQueue blinkQueue;
int m_addr = FXOS8700CQ_SLAVE_ADDR1;
float sum_distance = 0;
void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len);
void FXOS8700CQ_writeRegs(uint8_t * data, int len);
float CalculateDistance() {
    float distance = 9.8 * sqrt(t[0] * t[0] + t[1] * t[1] + t[2] * t[2]) * 0.1 * 0.1;
    return distance;
}
void Blink(){
    led = !led;
}

void Logger() {
    for(int i = 0; i < 100; i++) {
        
        if(i % 5 == 0) led = !led;
        float distance;
        int flag;
        pc.printf("%1.4f %1.4f %1.4f\r\n", t[0], t[1], t[2]);
        distance = CalculateDistance();
        sum_distance += distance;
        if (sum_distance > 5) {
            flag = 1;
            pc.printf("%d\r\n", flag);
        }
        else {
            flag = 0;
            pc.printf("%d\r\n", flag);
        }
        wait(0.1);
    }
    return;
}

void btn_fall_irq() {
    sum_distance = 0;
    eventQueue.call(&Logger);
}


int main() {
    led = 1;
    Thread eventThread(osPriorityHigh);
    eventThread.start(callback(&eventQueue, &EventQueue::dispatch_forever));
    btn.fall(&btn_fall_irq);

    uint8_t data[2], res[6];
    int16_t acc16;
    
    

    // Enable the FXOS8700Q

    FXOS8700CQ_readRegs( FXOS8700Q_CTRL_REG1, &data[1], 1);
    data[1] |= 0x01;
    data[0] = FXOS8700Q_CTRL_REG1;
    FXOS8700CQ_writeRegs(data, 2);

    FXOS8700CQ_readRegs(FXOS8700Q_OUT_X_MSB, res, 6);

    acc16 = (res[0] << 6) | (res[1] >> 2);
    if (acc16 > UINT14_MAX/2)
        acc16 -= UINT14_MAX;
    init[0] = ((float)acc16) / 4096.0f;

    acc16 = (res[2] << 6) | (res[3] >> 2);
    if (acc16 > UINT14_MAX/2)
            acc16 -= UINT14_MAX;
    init[1] = ((float)acc16) / 4096.0f;

    acc16 = (res[4] << 6) | (res[5] >> 2);
    if (acc16 > UINT14_MAX/2)
            acc16 -= UINT14_MAX;
    init[2] = ((float)acc16) / 4096.0f;
    
    while (true) {

        FXOS8700CQ_readRegs(FXOS8700Q_OUT_X_MSB, res, 6);

        acc16 = (res[0] << 6) | (res[1] >> 2);
        if (acc16 > UINT14_MAX/2)
            acc16 -= UINT14_MAX;
        t[0] = ((float)acc16) / 4096.0f;

        acc16 = (res[2] << 6) | (res[3] >> 2);
        if (acc16 > UINT14_MAX/2)
             acc16 -= UINT14_MAX;
        t[1] = ((float)acc16) / 4096.0f;

        acc16 = (res[4] << 6) | (res[5] >> 2);
        if (acc16 > UINT14_MAX/2)
                acc16 -= UINT14_MAX;
        t[2] = ((float)acc16) / 4096.0f;

    }    
}

void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len) {
    char t = addr;
    i2c.write(m_addr, &t, 1, true);
    i2c.read(m_addr, (char *)data, len);
}

void FXOS8700CQ_writeRegs(uint8_t * data, int len) {
    i2c.write(m_addr, (char *)data, len);
}