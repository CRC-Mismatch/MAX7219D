/* 
 * File:   MAX7219D.h
 * Author: mismatch
 *
 * Created on May 9, 2014, 12:21 AM
 */

#ifndef MAX7219D_H
#define	MAX7219D_H

#include <Arduino.h>

#define mapB(X) (X - 10) * 15 / 490

#define A_NOP 0x00
#define A_D00 0x01
#define A_D01 0x02
#define A_D02 0x03
#define A_D03 0x04
#define A_D04 0x05
#define A_D05 0x06
#define A_D06 0x07
#define A_D07 0x08
#define A_DCM 0x09
#define A_INT 0x0A
#define A_SCL 0x0B
#define A_OFF 0x0C
#define A_TST 0x0F
#define B_01 B00110000
#define B_02 B01101101
#define B_03 B01111001
#define B_04 B00110011
#define B_05 B01011011
#define B_06 B01011111
#define B_07 B01110000
#define B_08 B01111111
#define B_09 B01111011
#define B_00 B01111110
#define B_AA B01110111
#define B_AB B00011111
#define B_AC B00001101
#define B_AD B00111101
#define B_AE B01001111
#define B_AF B01000111
#define B_AG B01011110
#define B_AH B00110111
#define B_AI B_01
#define B_AJ B00111100
#define B_AK 0
#define B_AL B00001110
#define B_AM 0
#define B_AN B00010101
#define B_AO B00011101
#define B_AP B01100111
#define B_AQ 0
#define B_AR 0
#define B_AS B_05
#define B_AT B00001111
#define B_AU B00111110
#define B_AV B00011100
#define B_AW 0
#define B_AX 0
#define B_AY B_04
#define B_AZ B_02
#define B_MN B00000001

#define SINK OUTPUT
#define SOURCE OUTPUT
#define VCC true
#define GND false
#define ON true
#define OFF false

struct bits {
    uint8_t o : 1;
    uint8_t g : 1;
    uint8_t y : 1;
    uint8_t t : 1;
    uint8_t a : 1;
    uint8_t test : 1;
};

class MAX7219D {
public:
	MAX7219D(uint8_t clk, uint8_t cs, uint8_t dout);

        void showTime(int t);
        void showMins(uint8_t t);
        void showHour(uint8_t t);
        void show(const uint8_t dig, const uint8_t n);
        void tick();
        void tick(uint8_t to);
        void alarm(uint8_t to);
        void yellow(uint8_t to);
        void green(uint8_t to);
        void test();
        void on();
        void off();
        void allOff();
        void setBrightness(int b);
        void showString(char * str);
private:
        volatile uint8_t* findDDR(uint8_t pN);
        volatile uint8_t* findPort(uint8_t pN, uint8_t & pB);
        
        volatile uint8_t *pClk, *pCS, *pData;
        uint8_t bClk, bCS, bData, pDataN, pClkN;
        uint8_t n[4];
        bits _b;
        int bright;
        void sOut(uint8_t addr, uint8_t data);
        
};

#endif	/* MAX7219D_H */

