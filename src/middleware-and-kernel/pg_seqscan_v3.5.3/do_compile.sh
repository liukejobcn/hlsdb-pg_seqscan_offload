#!/bin/sh
	nohup faketime '2021-03-01 13:00:00' make all TARGET=sw_emu > sw_emu.txt &	
	nohup faketime '2021-03-01 13:00:00' make all TARGET=hw_emu > hw_emu.txt &	
	nohup faketime '2021-03-01 13:00:00' make all TARGET=hw > hw.txt &	
