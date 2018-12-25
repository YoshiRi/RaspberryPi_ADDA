ALL: da_output calibrate

da_output:dac8532_output.o
	gcc dac8532_output.c -o dac8532_output -lbcm2835 -lm

calibrate:calib.o
	gcc calib.c -o calib -lbcm2835 -lm


