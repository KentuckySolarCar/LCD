# MPLAB IDE generated this makefile for use with GNU make.
# Project: LCD1.mcp
# Date: Fri Jun 21 17:05:36 2013

AS = MPASMWIN.exe
CC = mcc18.exe
LD = mplink.exe
AR = mplib.exe
RM = rm

LCD1.cof : LCD_JF_MODBPS_test1.o ECANPoll.o functions_Serial_JF.o
	$(LD) /p18F4480 /l"C:\MCC18\lib" "LCD_JF_MODBPS_test1.o" "ECANPoll.o" "functions_Serial_JF.o" /u_CRUNTIME /z__MPLAB_BUILD=1 /o"LCD1.cof" /M"LCD1.map" /W

LCD_JF_MODBPS_test1.o : LCD_JF_MODBPS_test1.c ../../../../../MCC18/h/spi.h ../../../../../MCC18/h/stdlib.h ECANPoll.h ../../../../../MCC18/h/usart.h ../../../../../MCC18/h/stdio.h ../../../../../MCC18/h/i2c.h ../../../../../MCC18/h/string.h ../../../../../MCC18/h/delays.h LCD_JF_MODBPS_test1.c ../../../../../MCC18/h/p18F4480.h ../../../../../MCC18/h/pconfig.h ../../../../../MCC18/h/p18cxxx.h ../../../../../MCC18/h/p18f4480.h ecanpoll.def ../../../../../MCC18/h/stdarg.h ../../../../../MCC18/h/stddef.h functions_Serial_JF.h LCD_JF_BPS.def
	$(CC) -p=18F4480 /i"C:\MCC18\h" "LCD_JF_MODBPS_test1.c" -fo="LCD_JF_MODBPS_test1.o" -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-

ECANPoll.o : ECANPoll.c ecanpoll.h ECANPoll.c ecanpoll.def ../../../../../MCC18/h/p18cxxx.h ../../../../../MCC18/h/p18f4480.h
	$(CC) -p=18F4480 /i"C:\MCC18\h" "ECANPoll.c" -fo="ECANPoll.o" -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-

functions_Serial_JF.o : functions_Serial_JF.c ../../../../../MCC18/h/usart.h ../../../../../MCC18/h/stdio.h functions_Serial_JF.c ../../../../../MCC18/h/pconfig.h ../../../../../MCC18/h/p18cxxx.h ../../../../../MCC18/h/p18f4480.h ../../../../../MCC18/h/stdarg.h ../../../../../MCC18/h/stddef.h
	$(CC) -p=18F4480 /i"C:\MCC18\h" "functions_Serial_JF.c" -fo="functions_Serial_JF.o" -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-

clean : 
	$(RM) "LCD_JF_MODBPS_test1.o" "ECANPoll.o" "functions_Serial_JF.o" "LCD1.cof" "LCD1.hex"

