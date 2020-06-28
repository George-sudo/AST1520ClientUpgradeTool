#include "checkout.h"


quint16 crc16_ccitt(const char *buf, int len)
{
    register int counter;
    register unsigned short crc = 0;
    for( counter = 0; counter < len; counter++)
    crc = (crc<<8) ^ crc16tab[((crc>>8) ^ *(char *)buf++)&0x00FF];
    return crc;
}

/* 发送方调用,将所有的数据累加之后(溢出丢弃) 取反.返回值
 *
 */
uint8_t TX_CheckSum(uint8_t *buf, uint16_t len) //buf为数组，len为数组长度
{
    uint16_t i;
    uint8_t ret = 0;

    for(i=0; i<len; i++)
    {
        ret += *(buf++);
    }
    ret = ~ret;
    return ret;
}

/*
 *
 */
uint8_t RX_CheckSum(uint8_t *buf, uint16_t len) //buf为数组，len为数组长度
{
    uint16_t i;
    uint8_t ret = 0;

    for(i=0; i<len; i++)
    {
        ret += *(buf++);
    }
    return ret+1;
}
