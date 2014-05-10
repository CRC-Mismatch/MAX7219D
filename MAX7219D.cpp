/* 
 * File:   MAX7219D.cpp
 * Author: mismatch
 * 
 * Created on May 9, 2014, 12:21 AM
 */

#include "MAX7219D.h"

volatile uint8_t* MAX7219D::findPort(uint8_t pN, uint8_t& pB) {
    volatile uint8_t * p;
    uint8_t b;
    if (pN > 13 && pN < 15) {
        p = &PORTC;
        b = pN - 14;
    } else if (pN > 7 && pN < 13) {
        p = &PORTB;
        b = pN - 8;
    } else if (pN < 8) {
        p = &PORTD;
        b = pN;
    } else {
        pB = 0;
        return 0;
    }
    pB = b;
    return p;
}

volatile uint8_t* MAX7219D::findDDR(uint8_t pN) {
    volatile uint8_t * p;
    uint8_t b;
    if (pN > 13 && pN < 15) {
        p = &DDRC;
        b = pN - 14;
    } else if (pN > 7 && pN < 13) {
        p = &DDRB;
        b = pN - 8;
    } else if (pN < 8) {
        p = &DDRD;
        b = pN;
    } else {
        return 0;
    }
    return p;
}

void MAX7219D::sOut(uint8_t addr, uint8_t data) {
    switch (addr) {
        case 1:
            addr = 2;
            break;
        case 2:
            addr = 1;
            break;
        case 3:
            addr = 4;
            break;
        case 4:
            addr = 3;
            break;
    }
    *pCS &= ~(1 << bCS);
    *pClk &= ~(1 << bClk);
    shiftOut(pDataN, pClkN, MSBFIRST, addr);
    shiftOut(pDataN, pClkN, MSBFIRST, data);
    *pCS |= 1 << bCS;
}

MAX7219D::MAX7219D(uint8_t clk, uint8_t cs, uint8_t data) {
    pDataN = data;
    pClkN = clk;
    pClk = findPort(clk, bClk);
    pCS = findPort(cs, bCS);
    pData = findPort(data, bData);
    volatile uint8_t* d = findDDR(clk);
    *d |= 1 << bClk;
    d = findDDR(cs);
    *d |= 1 << bCS;
    d = findDDR(data);
    *d |= 1 << bData;
    *pClk &= ~(1 << bClk);
    *pCS |= 1 << bCS;
    *pData &= ~(1 << bData);
    bright = 0;
    _b.o = false;
    _b.g = false;
    _b.y = false;
    _b.t = false;
    _b.a = false;
    _b.test = false;
    memset(n, 0, 4);
    sOut(A_SCL, 3);
    sOut(A_TST, 0);
    sOut(A_INT, 0);
    sOut(A_DCM, 0);
    sOut(A_D00, B_03);
    sOut(A_D01, B_02);
    sOut(A_D02, B_01);
    sOut(A_D03, B_00);
    sOut(A_OFF, 1);
}

void MAX7219D::showTime(int t) {
    for (uint8_t i = 0; i < 4; i++) {
        show(i, t % 10);
        t /= 10;
    }
}

void MAX7219D::showMins(uint8_t t) {
    for (uint8_t i = 0; i < 2; i++) {
        show(i, t % 10);
        t /= 10;
    }
}

void MAX7219D::showHour(uint8_t t) {
    for (uint8_t i = 2; i < 4; i++) {
        show(i, t % 10);
        t /= 10;
    }
}

void MAX7219D::show(uint8_t dig, const uint8_t nm) {
    uint8_t num = 0;
    switch (nm) {
        case 0:
            num = B_00;
            break;
        case 1:
            num = B_01;
            break;
        case 2:
            num = B_02;
            break;
        case 3:
            num = B_03;
            break;
        case 4:
            num = B_04;
            break;
        case 5:
            num = B_05;
            break;
        case 6:
            num = B_06;
            break;
        case 7:
            num = B_07;
            break;
        case 8:
            num = B_08;
            break;
        case 9:
            num = B_09;
            break;
        default:
            num = B_MN;
            break;
    }
    switch (dig) {
        case 0:
            num |= _b.g << 7;
            break;
        case 1:
            num |= _b.y << 7;
            break;
        case 2:
            num |= _b.t << 7;
            break;
        case 3:
            num |= _b.a << 7;
            break;
    }
    sOut(dig + 1, num);
    n[dig] = num;
}

void MAX7219D::tick() {
    n[2] ^= 1 << 7;
    _b.t ^= 1;
    sOut(A_D02, n[2]);
}

void MAX7219D::tick(uint8_t to) {
    n[2] = (to == true) ? n[2] | (1 << 7) : n[2] & ~(1 << 7);
    _b.t = to;
    sOut(A_D02, n[2]);
}

void MAX7219D::alarm(uint8_t to) {
    n[3] = (to == true) ? n[3] | (1 << 7) : n[3] & ~(1 << 7);
    _b.a = to;
    sOut(A_D03, n[3]);
}

void MAX7219D::yellow(uint8_t to) {
    n[1] = (to == true) ? n[1] | (1 << 7) : n[1] & ~(1 << 7);
    _b.y = to;
    sOut(A_D01, n[1]);
}

void MAX7219D::green(uint8_t to) {
    n[0] = (to == true) ? n[0] | (1 << 7) : n[0] & ~(1 << 7);
    _b.g = to;
    sOut(A_D00, n[2]);
}

void MAX7219D::test() {
    _b.test ^= 1;
    sOut(A_TST, _b.test);
}

void MAX7219D::on() {
    _b.o = false;
    sOut(A_OFF, ~(_b.o));
}

void MAX7219D::off() {
    _b.o = true;
    sOut(A_OFF, ~(_b.o));
}

void MAX7219D::allOff() {
    memset(n, 0, 4);
    _b.g = 0;
    _b.y = 0;
    _b.t = 0;
    _b.a = 0;
    _b.test = 0;
    sOut(A_D00, 0);
    sOut(A_D01, 0);
    sOut(A_D02, 0);
    sOut(A_D03, 0);
}

void MAX7219D::setBrightness(int b) {
    bright = mapB(b);
    sOut(A_INT, bright);
}

void MAX7219D::showString(char * str) {
    for (int i = 3; i >= 0; i--) {
        switch (str[3 - i]) {
            case 'a':
                sOut(i + 1, B_AA);
                break;
            case 'b':
                sOut(i + 1, B_AB);
                break;
            case 'c':
                sOut(i + 1, B_AC);
                break;
            case 'd':
                sOut(i + 1, B_AD);
                break;
            case 'e':
                sOut(i + 1, B_AE);
                break;
            case 'f':
                sOut(i + 1, B_AF);
                break;
            case 'g':
                sOut(i + 1, B_AG);
                break;
            case 'h':
                sOut(i + 1, B_AH);
                break;
            case 'i':
                sOut(i + 1, B_AI);
                break;
            case 'j':
                sOut(i + 1, B_AJ);
                break;
            case 'l':
                sOut(i + 1, B_AL);
                break;
            case 'n':
                sOut(i + 1, B_AN);
                break;
            case 'o':
                sOut(i + 1, B_AO);
                break;
            case 'p':
                sOut(i + 1, B_AP);
                break;
            case 'q':
                sOut(i + 1, B_AQ);
                break;
            case 'r':
                sOut(i + 1, B_AR);
                break;
            case 's':
                sOut(i + 1, B_AS);
                break;
            case 't':
                sOut(i + 1, B_AT);
                break;
            case 'u':
                sOut(i + 1, B_AU);
                break;
            case 'v':
                sOut(i + 1, B_AV);
                break;
            case 'y':
                sOut(i + 1, B_AY);
                break;
            case 'z':
                sOut(i + 1, B_AZ);
                break;
            case '-':
            case 0xFF:
            default:
                sOut(i + 1, B_MN);
                break;
        }
    }
}
