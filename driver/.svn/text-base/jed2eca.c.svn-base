#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xo2_dev.h"

const XO2DevInfo_t XO2DevList[LATTICE_XO2_NUM_DEVS] = 
{
	// Name            Cfg pgs 
	//                  |     UFM pgs 
	//                  |       |   Cfg erase time  
	//                  |       |    |     UFM erase time
	//                  |       |    |      |     Trefresh time
	{"MachXO2-256",    575,     0,  700,    0,      1},
	{"MachXO2-640",   1152,   191,  1100, 600,      1},
	{"MachXO2-640U",  2175,   512,  1400, 700,      1},
	{"MachXO2-1200",  2175,   512,  1400, 700,      1},
	{"MachXO2-1200U", 3200,   639,  1900, 900,      2},
	{"MachXO2-2000",  3200,   639,  1900, 900,		2},
	{"MachXO2-2000U", 5760,   767,  3100, 1000,		3},
	{"MachXO2-4000",  5760,   767,  3100, 1000,		3},
	{"MachXO2-7000",  9216,  2046,  4800, 1600,		4}
}; 

#define TRUE 1
#define FALSE 0

#define MAX_LINE 1023
#define STX 0x02
#define ETX 0x03

enum
{
	COMMENT,
	FUSE_CHECKSUM,
	FUSE_DATA,
	END_DATA,
	FUSE_LIST,
	SECURITY_FUSE,
	FUSE_DEFAULT,
	FUSE_SIZE,
	USER_CODE,
	FEATURE_ROW,
	DONE
};

//===============================================================================
// These are copied from the driver code so sizes/enums match
//===============================================================================
void    convertFeatureRowToHexArray(unsigned char *p, FILE *fpOut, int cnt);
void    convertFuseToHexArray(unsigned char *p, FILE *fpOut, int cnt);
void    findDeviceType(char *pS, XO2Devices_t *pDevID);

int  convertBinaryField(char *p, unsigned int *pVal)
{
	unsigned int val;
	unsigned int i = 0;
	
	val = 0;
	while ((*p != '\0') && (i < 32))
	{
		val = (val<<1) | (*p - '0');
		++i;
	}
	*pVal = val;
	return(i/8);
}



void removeLastStar(unsigned char *p)
{
	int i;
	i = 0;
	while (*p != '\0')
	{
		++i;
		++p;
	}
	
	if (i == 0)
	    return;  // empty string
	    
	while ((*p != '*') && i)
	{
	    --p;
	    --i;
	}
	if (i)
	    *p = '\0';  // replace last '*' with NULL, end line at that spot
}

int cfg_data_start = 0;
int cfg_data_flag = 0;
FILE  * fpBin;

//int main(int argc, char *argv[])
int jed2eca(char * filename)
{
	FILE *fpIn, *fpOut;
	unsigned char line[MAX_LINE + 1];
	int pageCnt, UFMPageCnt, CfgPageCnt;
	int byteCnt;
	int compress;
	int done;
	int state = -1;
	int fuse_addr;
	int fuse_size;
	int erase_val;
	int security_fuse;
	unsigned int userCode;
	char *pName = "tmp";
	char outCFileStr[256];
	char outHFileStr[256];
	char outBinFileStr[256];
	char * path = "/ramDisk/";
	XO2Devices_t devID;


	devID =	MachXO2_1200;

	//fpIn = fopen(argv[1], "r");
	fpIn = fopen(filename, "r");
	if (fpIn == NULL)
	{
		perror("Opening input file");
		exit(-1);
	}

	//pName = argv[2];
	sprintf(outCFileStr, "%s%s.c", path, pName);
	sprintf(outHFileStr, "%s%s.h", path, pName);
	sprintf(outBinFileStr, "%s%s.bin", path, pName);

	fpOut = fopen(outCFileStr, "w");
	if (fpOut == NULL)
	{
		fclose(fpIn);
		perror("Opening output file");
		exit(-1);
	}

	fpBin = fopen(outBinFileStr, "wb");
	if( fpBin == NULL)
	{
		fclose(fpBin);
		perror("opening output file");
		exit(-1);
	}

	
	cfg_data_start = 1;
	pageCnt = 0;
	UFMPageCnt = 0;
	CfgPageCnt = 0;
	byteCnt = 0;
	compress = TRUE;
	fgets((char *)line, MAX_LINE, fpIn);
	if (line[0] != STX)
	{
		printf("ERROR!  Expected STX as first char!\nAborting.\n");
		fclose(fpIn);
		fclose(fpOut);
		exit(-2);
	}
	done = FALSE;
	while (!feof(fpIn)  && !done)
	{
		if (fgets((char *)line, MAX_LINE, fpIn) != NULL)
		{

			if ((line[0] == '0') || (line[0] == '1'))
				state = FUSE_DATA;
			else if (memcmp("NOTE", line, 4) == 0)
				state = COMMENT;
			else if (line[0] == 'G')
				state = SECURITY_FUSE;
			else if (line[0] == 'L')
				state = FUSE_LIST;
			else if (line[0] == 'C')
				state = FUSE_CHECKSUM;
			else if (line[0] == '*')
				state = END_DATA;
			else if (line[0] == 'D')
				state = FUSE_DEFAULT;
			else if (line[0] == 'U')
				state = USER_CODE;
			else if (line[0] == 'E')
				state = FEATURE_ROW;
			else if (memcmp("QF", line, 2) == 0)
				state = FUSE_SIZE;
			else if (line[0] == ETX)
				state = DONE;

			switch (state)
			{
				case FUSE_DATA:
					++pageCnt;
					if ((pageCnt == 1) && (fuse_addr == 0))
						fprintf(fpOut, "unsigned char %s_CfgData[] = { // Cfg Data Array\n", pName);
					else if (pageCnt == XO2DevList[devID].Cfgpages + 1)
						fprintf(fpOut, "};\n\nunsigned char %s_UFMData[] = { // UFM Data Array\n", pName);
					if (pageCnt <= XO2DevList[devID].Cfgpages)
					{
						cfg_data_flag = 1;
						++CfgPageCnt;
						convertFuseToHexArray(line, fpOut, CfgPageCnt);
					}
					else
					{
						cfg_data_flag = 0;
						++UFMPageCnt;
						convertFuseToHexArray(line, fpOut, UFMPageCnt);
					}
					break;

				case COMMENT:
					removeLastStar(line);
					fprintf(fpOut, "// %s\n", &line[5]);
					break;

				case FUSE_LIST:
					removeLastStar(line);
					sscanf((char *)&line[1], "%d", &fuse_addr);
					if (fuse_addr == 0)
					{
						fprintf(fpOut, "#include \"XO2_dev.h\"   // for XO2_JEDEC_t def\n\n");
						fprintf(fpOut, "// Flash Data Array Declarations\n");
					}
					break;

				case SECURITY_FUSE:
					removeLastStar(line);
					sscanf((char *)&line[1], "%d", &security_fuse);
					printf("Security Fuse: %x\n", security_fuse);
					break;

				case FUSE_DEFAULT:
					removeLastStar(line);
					sscanf((char *)&line[1], "%d", &erase_val);
					if (erase_val != 0)
						printf("WARNING!  DEFAULT ERASE STATE NOT 0!\n");
					break;

				case FUSE_SIZE:
					removeLastStar(line);
					sscanf((char *)&line[2], "%d", &fuse_size);
					break;

				case USER_CODE:   // This is informational only.  USERCODE is part of Config sector
					removeLastStar(line);
					if (line[1] == 'H')
						sscanf((char *)&line[2], "%x", &userCode);
					else
						convertBinaryField((char *)&line[1], &userCode);
					break;

				case FEATURE_ROW:
					// 2 consectutive rows.  1st starts with E and is 64 bits.  2nd line is 16 bits, ends in *
					fprintf(fpOut, "\n// Feature Row Data Structure\n");
					fprintf(fpOut, "XO2FeatureRow_t %s_FeatureRow = \n{\n", pName);
					fprintf(fpOut, "\t{");
					convertFeatureRowToHexArray(&line[1], fpOut, 8);
					fprintf(fpOut, "},  // Feature bits\n");

					fgets((char *)line, MAX_LINE, fpIn);
					removeLastStar(line);
					fprintf(fpOut, "\t{");
					convertFeatureRowToHexArray(&line[0], fpOut, 2);
					fprintf(fpOut, "}  // FEABITS\n};\n\n");

					break;

				case DONE:
					done = TRUE;
					break;

				case END_DATA:
					cfg_data_start = 0;
					fprintf(fpOut, "// *\n");
					break;

				case FUSE_CHECKSUM:
					fprintf(fpOut, "};  // End UFM Data\n");
					break;

				default:
					// do nothing
					break;

			}
			// Look for specific XO2 Device type and extract
			if ((state == COMMENT)  && (memcmp("DEVICE NAME:", &line[5], 12) == 0))
			{
				findDeviceType((char *)&line[17], &devID);
			}
		}
	}
	// Write the declaration and initialization of the JEDEC Data structure to the
	// C file for compilation.
	fprintf(fpOut, "\n\n// XO2 JEDEC Cfg/UFM Data Structure Declaration\n");
	fprintf(fpOut, "XO2_JEDEC_t XO2_JEDEC_%s = \n{\n", pName);
	fprintf(fpOut, "\t%d,     // DevID=%s\n", (int)devID, XO2DevList[devID].pName);
	fprintf(fpOut, "\t%d,     // total pageCnt\n", pageCnt);
	fprintf(fpOut, "\t%d,     // CfgDataSize (bytes)\n", CfgPageCnt * 16);
	fprintf(fpOut, "\t%d,     // UFMDataSize (bytes)\n", UFMPageCnt * 16);
	fprintf(fpOut, "\t0x%08x, // USERCODE\n", userCode);
	fprintf(fpOut, "\t0x%08x,  // SecurityFuses\n", security_fuse);
	fprintf(fpOut, "\t%s_CfgData,  // unsigned char *pCfgData \n", pName);
	fprintf(fpOut, "\t%s_UFMData,  // unsigned char *pUFMData\n", pName);
	fprintf(fpOut, "\t&%s_FeatureRow  // XO2FeatureRow_t *pFeatureRow\n", pName);

	fprintf(fpOut, "};\n");

	// Close the C file that holds the byte data in the structure
	fclose(fpIn);
	fclose(fpOut);
	fclose(fpBin);

	// Open the Header file to define the sizes and name
	fpOut = fopen(outHFileStr, "w");

	fprintf(fpOut, "#ifndef XO2_JEDEC_%s_H\n", pName);
	fprintf(fpOut, "#define XO2_JEDEC_%s_H\n", pName);
	fprintf(fpOut, "\n#include \"XO2_dev.h\"    // for XO2_JEDEC_t def\n\n");

	fprintf(fpOut, "extern XO2_JEDEC_t XO2_JEDEC_%s;\n", pName);

	fprintf(fpOut, "#endif\n");

	fclose(fpOut);

	return 0;
}



/**
 * Convert a line of fuse data into a list of C byte values in an array.
 * This function is specific to the MachXO2 format of 128 bits per page (fuse row).
 * So it just processes knowing that there are 16 bytes per row.
 *
 * JEDEC file format is lsb first so shift into byte from top down.
 */
void    convertFuseToHexArray(unsigned char *p, FILE *fpOut, int cnt)
{
	unsigned char val;
	int i, j;

	for (i = 0; i < 16; i++)
	{
		val = 0;
		for (j = 0; j < 8; j++)
		{
			val = (val<<1) | (*p - '0');
			++p;
		}
		if((cfg_data_start == 1) && (cfg_data_flag == 1)){
		//if(cfg_data_flag == 1){
			fwrite(&val, 1, 1, fpBin);	
		}
		fprintf(fpOut, " 0x%02x,", val);
	}
	fprintf(fpOut, "  // %d\n", cnt);
}


/**
 * Convert the E Field (Feature Row) into a list of C byte values in an array.
 * This function is specific to the MachXO2 Feature Row format.
 * The number of bytes in a row is passed in.
 * the bits are actually in reverse order so we need to work backwards through
 * the string.
 *
 * JEDEC file format is lsb first so shift into byte from top down.
 */
void    convertFeatureRowToHexArray(unsigned char *p, FILE *fpOut, int cnt)
{
	unsigned char val;
	int i, j;


	// start at last char in string and work backwards
	p = p + ((8 * cnt) - 1);


	for (i = 0; i < cnt; i++)
	{
		val = 0;
		for (j = 0; j < 8; j++)
		{
			val = (val<<1) | (*p - '0');

			--p;
		}

		fprintf(fpOut, " 0x%02x,", val);
	}

}


// TODO
// Still need to handle the U devices and their increased sizes.

//	MachXO2_256,
//	MachXO2_640,
//	MachXO2_640U,
//	MachXO2_1200,
//	MachXO2_1200U,
//	MachXO2_2000,
//	MachXO2_2000U,
//	Mach_XO2_4000,
//	MachXO2_7000
void    findDeviceType(char *pS, XO2Devices_t *pDevID)
{
	printf("XO2 Dev: %s\n", pS);

	if (strstr(pS, "LCMXO2-256") != NULL)
		*pDevID = MachXO2_256;
	else if (strstr(pS, "LCMXO2-640") != NULL)
		*pDevID = MachXO2_640;
	else if (strstr(pS, "LCMXO2-1200") != NULL)
		*pDevID = MachXO2_1200;
	else if (strstr(pS, "LCMXO2-2000") != NULL)
		*pDevID = MachXO2_2000;
	else if (strstr(pS, "LCMXO2-4000") != NULL)
		*pDevID = MachXO2_4000;
	else if (strstr(pS, "LCMXO2-7000") != NULL)
		*pDevID = MachXO2_7000;
	else
		*pDevID = MachXO2_1200;   // our default
}
