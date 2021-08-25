// md5_hash.cpp: 定义应用程序的入口点。
//
#include "md5.h"
#define UPPER_HEX 1
#define LOWER_HEX 0

//对字节流进行16进制输出 outType = UPPER_HEX or LITTLE_HEX
void PrintByteString(const uint8_t* bytes,size_t bSize, bool outType)
{
	if (outType)
		for (size_t i = 0; i < bSize; i++)
			printf("%2.2X", bytes[i]);
	else
		for (size_t i = 0; i < bSize; i++)
			printf("%2.2x", bytes[i]);
}

void ShowHelpMessage() {
	puts("md5.exe: get md5 of specified file(s)");
	puts("usage: md5 [-xXh] file_name1 file_name2 ... \n");
	puts("-h,--help\tshow this help message");
	puts("-X\t\tprint with upper hex");
	puts("-x,default\tprint with lower hex");
	
}
int main(int argc, char** argv)
{
	bool outType = LOWER_HEX; //决定了输出的MD5是大写hex还是小写hex 
	int error_flag = 0;
	int fileNameCur = 1;	
	int fileNameEnd = argc;
	if (argc == 1) {
	hex_hci_out:
		char fileName[0x100];
		memset(fileName, 0, 0x100);
		puts("-----------md5_checksum------------");
		printf("Get help message by execute:\n%s -h\n",argv[0]);
		puts("Enter filename:");
		char ch;
		int i = 0;
		while (i<0xFF) {
			ch = getchar();
			if (ch != '\n') {
				fileName[i++] = ch;
			}
			else
				break;
		}
		//fileName[i++] = '"';
		fileName[i] = '\0';
		uint8_t result[0x10];
		memset(result, 0, 0x10);
		if (!MD5(fileName, result))
			PrintByteString(result, 0x10, outType);
		else if (error_flag == 0)
			error_flag = 1;
		printf("\n");
		system("pause");
	}
	else if (argc==2) {
		if (strcmp(argv[1], "-X") == 0) {
			outType = UPPER_HEX;
			goto hex_hci_out;
		}
		else if (strcmp(argv[1], "-x") == 0) {
			outType = LOWER_HEX;
			goto hex_hci_out;
		}
		else if ((strcmp(argv[1], "-h") == 0) ||
			(strcmp(argv[1], "-help") == 0) ||
			(strcmp(argv[1], "--help") == 0))
			ShowHelpMessage();
		else {
			fileNameCur = argc-1;
			goto hex_bat_out;
		}

	}
	else {
		if (!strcmp(argv[1], "-x") || !strcmp(argv[1], "-X")) {
			outType = argv[1][1]=='x'?LOWER_HEX:UPPER_HEX;
			fileNameCur = 2;
		}
		else if (!strcmp(argv[argc-1], "-x") || !strcmp(argv[argc - 1], "-X")) {
			outType = argv[argc-1][1] == 'x' ? LOWER_HEX : UPPER_HEX;
			fileNameEnd--;
		}
		hex_bat_out:
		uint8_t result[0x10];
		while(fileNameCur<fileNameEnd){
			memset(result, 0, 0x10);
			if (!MD5(argv[fileNameCur], result)) {
				//printf("%s\t", argv[fileNameCur]);
				PrintByteString(result, 0x10, outType);
			}
			else
				if (error_flag == 0)
					error_flag = 1;
			if (fileNameEnd - fileNameCur > 1)
				printf("\n");
			fileNameCur++;
		}

	}
	return error_flag;
}
