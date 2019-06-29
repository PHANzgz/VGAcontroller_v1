#include <stdio.h>

FILE* raw;
FILE* img;
FILE* imgTEXT;
unsigned char buffer[57600];  //buffer for image data
unsigned char rom[131072];   //rom data for 1Mbit

unsigned char getColor(int x, int y) {
	unsigned char R = (buffer[y*480 + x*3 + 0] >> 2) & 0b00110000;
	unsigned char G = (buffer[y*480 + x*3 + 1] >> 4) & 0b00001100;
	unsigned char B = (buffer[y*480 + x*3 + 2] >> 6) & 0b00000011;

	unsigned char color = R | G | B;

	return color;
}

unsigned int getAddress(int x, int y) {
	unsigned int address = ((0x24 + x) << 0) | ((0x09 + y) << 8);

	return address;
}

void main() {
	printf("Image dump: ");

	// Open image and copy to buffer
	//Image must be 160x120 and should be color mapped for better results
	raw = fopen("ayy.raw", "rb");  // r for read, b for binary
	fread(buffer, sizeof(buffer), 1, raw); // read image into buffer
	fclose(raw);


	//ROM

	//Erase entire EEPROM
	for (int address = 0; address < 131072; address++)
		rom[address] = 0x00;


	//Program image: Img starts at x=0x24 y=0x09 included and ends at x=0xC4 y=0x81 not included
	for (int x = 0; x < 160; x++) {
		for (int y = 0; y < 120; y++) {
			rom[getAddress(x, y)] = getColor(x, y);
			
		}
		
	}

	//Write control ROM to byte array in txt
	imgTEXT = fopen("imgTEXT.txt", "w");
	fprintf(imgTEXT, "{ ");
	for (int address = 0; address < sizeof(rom) - 1; address++) {
		fprintf(imgTEXT, "0x%X, ", rom[address]);
	}
	fprintf(imgTEXT, "0x%X ", rom[sizeof(rom)-1]);
	fprintf(imgTEXT, "};");
	fclose(imgTEXT);

	//Write IMG ROM output
	img = fopen("imageROM.rom", "wb");  // w for write, b for binary
	fwrite(rom, sizeof(rom), 1, img);
	fclose(img);

}