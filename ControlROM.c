#include <stdio.h>

#define HS 0
#define VS 1
#define NL 2
#define NF 3
#define CY 4


FILE* ctrl;
FILE* ctrlTEXT;
unsigned char rom[131072];   //rom data for 1Mbit

unsigned int getAddress(int x, int y) {
	unsigned int address = (x << 0) | (y << 8);

	return address;
}

void main() {
	//ROM
	printf("Control ROM ");
	//Erase entire EEPROM
	//All signals non-active: HS HIGH  VS HIGH  NL HIGH  NF HIGH  CY LOW
	for (int address = 0; address < 131072; address++)
		rom[address] = 0xEF;


	//Program VS, HS, NL, NF
	for (int y=0; y <= 131; y++) {
		for (int x = 0; x <= 199; x++) {
			unsigned int address = getAddress(x, y);

			if (x < 24) rom[address] &= ~(1 << HS); //HS is low during 23 cycles @6.25Mhz
			if (y < 1) rom[address] &= ~(1 << VS);  //VS is low during 1 cycle @6.25Mhz

			//Next line will reset X counter(sync reset) and count enable the Y counter
			if (x == 199) {
				//NextLine and CountY are actually the same thing but that saved me from adding another chip(inverter)
				rom[address-1] &= ~(1 << NL);
				rom[address-1] |= 1 << CY;
			}


			//Next frame will reset Y counter(sync reset)
			if (y == 131 && x == 199) rom[address-1] &= ~(1 << NF);

		}
	}
	
	//Write control ROM to byte array in txt
	ctrlTEXT = fopen("ctrlTEXT.txt", "w");
	fprintf(ctrlTEXT, "{ ");
	for (int address = 0; address < sizeof(rom) - 1; address++) {
		fprintf(ctrlTEXT, "0x%X, ", rom[address]);
	}
	fprintf(ctrlTEXT, "0x%X ", rom[sizeof(rom) - 1]);
	fprintf(ctrlTEXT, "}");
	fclose(ctrlTEXT);


	//Write control ROM output
	ctrl = fopen("controlROM.rom", "wb");  // w for write, b for binary
	fwrite(rom, sizeof(rom), 1, ctrl);
	fclose(ctrl);

}