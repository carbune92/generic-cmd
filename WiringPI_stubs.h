#ifndef __WIRINGPI_STUBS_H__
#define __WIRINGPI_STUBS_H__

#include <cstdio>

inline int wiringPiI2CSetup(int dev) {  printf("wiringPiI2CSetup %d\n", dev); return 1; }

inline int wiringPiI2CReadReg8(int fd, int reg) { printf("wiringPiI2CReadReg8 %d %d\n", fd, reg); return 5; }

inline int wiringPiI2CReadReg16(int fd, int reg) { printf("wiringPiI2CReadReg16 %d %d\n", fd, reg); return 6; }

inline int wiringPiI2CWriteReg8(int fd, int reg, int data) { printf("wiringPiI2CWriteReg8 %d %d %d\n", fd, reg, data); return 0; }

inline int wiringPiI2CWriteReg16(int fd, int reg, int data) { printf("wiringPiI2CWriteReg16 %d %d %d\n", fd, reg, data); return 0; }

#endif