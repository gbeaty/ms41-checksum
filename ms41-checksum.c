#include <stdio.h>
#include <stdlib.h>

struct MS41File {
	unsigned char *contents;
	unsigned short size;
};

struct Check {
	unsigned short num, recorded, calculated, size;
	unsigned int addr;
};

struct Checks {
	struct Check* start;
	unsigned int num;
};

unsigned short getWord(unsigned char* contents, unsigned int addr) {
	return (contents[addr] << 8) + contents[addr+1];
}

unsigned short swapWord(unsigned char* contents, unsigned int addr) {
	return contents[addr] + (contents[addr+1] << 8);
}

void updateCRC(unsigned char *contents, unsigned int addr, unsigned int crc )
{
	contents[addr++] = crc & 0xff;
	contents[addr] = (crc >> 8) & 0xff;
}

static const unsigned short crc16tab[256]= {
	0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
	0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
	0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
	0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
	0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
	0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
	0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
	0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
	0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
	0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
	0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
	0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
	0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
	0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
	0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
	0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
	0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
	0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
	0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
	0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
	0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
	0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
	0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
	0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
	0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
	0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
	0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
	0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
	0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
	0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
	0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
	0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

unsigned short crc16(const void *buf, int len, unsigned short initial)
{
	register int counter;
	register unsigned short crc = initial;
	const char *p = (const char *)buf;
	for( counter = 0; counter < len; counter++)
	{
		crc = (crc >> 8) ^ crc16tab[(crc ^ *p++)& 0xff];
	}
	return crc;
}

void printUsage() {
	printf("Usage: e36-checksum <bin file path> <corrected checksum output path>\n");
}

void printCheck(struct Check c) {
	printf("Checksum %02d at 0x%05x Value: %04x Comp: %04x BlockSize: %03x (Checksum ", c.num, c.addr + c.size, c.recorded, c.calculated, c.size);	
		if(c.calculated == c.recorded) {
			printf("OK)\n");
		} else {
			printf("MISMATCH)\n");
		}
}

void check(unsigned char* contents, unsigned int size, struct Checks cs) {

	unsigned short initialValue = getWord(contents, 0x000E);
	printf("Found initial Value: 0x%x\n\n", initialValue);

	unsigned short addr = 0x0000;
	unsigned short count = 1;

	int i;
	for(i=0; i<cs.num; i++) {
		struct Check *c = &cs.start[i];
		(*c).num = count;
		(*c).addr = addr;
		unsigned short checksumAddr = swapWord(contents, addr);
		(*c).recorded = swapWord(contents, checksumAddr);
		unsigned short nextChecksumAddr = checksumAddr + 2;
		(*c).size = checksumAddr - addr;

		if(checksumAddr == 0xFFFF) {
			return;
		}

		(*c).calculated = crc16(contents + addr, (*c).size, initialValue);

		if( c->calculated != c->recorded )
			updateCRC( contents, checksumAddr, c->calculated );
		addr = nextChecksumAddr;
		count++;
	}
}

struct Checks checkFile(struct MS41File *f) {

	struct Checks cs;
	cs.num = 0;
	cs.start = NULL;
	unsigned char *contents = f->contents;
	int size = f->size;

	unsigned short addr = 0x0000;
	while(1) {
		unsigned short checksumAddr = swapWord(contents, addr);
		if(checksumAddr == 0xFFFF)
			break;

		if(addr >= (size - 1)) {
			printf("Error parsing file: Unexpected EOF (%x).\n", addr);
			return cs;
		}
		if(checksumAddr <= addr) {
			printf("Error parsing file: The next checksum addr (%x) must come after the last (%x).\n", checksumAddr, addr);
			return cs;
		}

		addr = checksumAddr + 2;
		cs.num++;
	}

	cs.start = (struct Check *)malloc(sizeof(struct Check) * cs.num);

	check(contents, size, cs);

	return cs;	
}

int LoadFile( struct MS41File *f, char *path )
{
	FILE *fp = fopen( path, "rb" );
	if( fp == NULL ) {
		printf( "File '%s' not found.\n", path );
		return -1;
	}


	fseek(fp, 0L, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	if(size < 0x000F) {
		printf("Unexpected EOF - file is too short.\n");
		return -1;
	}

	f->size = size;
	f->contents = (unsigned char *)malloc(size);
	fread(f->contents, 1, size, fp);
	fclose(fp);

	return 0;
}
void main(int argc, char* argv[]) {
	if(argc != 3) {
		printUsage();
		return;
	}

	struct MS41File file;

	char* inpath = argv[1];
	char* outpath = argv[2];

	if( LoadFile( &file, inpath ) < 0 ) {
		printf("File '%s' not found.\n", inpath);
		printUsage();
		return;
	}

	struct Checks cs = checkFile( &file );
	unsigned int i;
	for(i=0; i<cs.num; i++) {
		printCheck(cs.start[i]);
	}

	FILE *fp = fopen( outpath, "wb" );
	if( fp == NULL ) {
		printf("Unable to to open output file '%s'\n", outpath );
		printUsage();
		return;
	}
	fwrite( file.contents, 1, file.size, fp );
	fclose( fp );
}
