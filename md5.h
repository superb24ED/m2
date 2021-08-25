#ifndef MD5_H
#define MD5_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define F(X, Y, Z) (((X) & (Y)) | ((~X) & (Z)))
#define G(X, Y, Z) (((X) & (Z)) | ((Y) & (~Z)))
#define H(X, Y, Z) ((X) ^ (Y) ^ (Z))
#define I(X, Y, Z) ((Y) ^ ((X) | (~Z)))
#define LEFTROTATE(NUM,S) (((NUM) << S) | ((NUM >> (0x20 - S))))
//加法常数表,T[i] = (int)(abs(sin(i + 1)) * 0x100000000);
const uint32_t T[0x40] = {
0xD76AA478,	0xE8C7B756,	0x242070DB,	0xC1BDCEEE,	0xF57C0FAF,	0x4787C62A,	0xA8304613,	0xFD469501,
0x698098D8,	0x8B44F7AF,	0xFFFF5BB1,	0x895CD7BE,	0x6B901122,	0xFD987193,	0xA679438E,	0x49B40821,
0xF61E2562,	0xC040B340,	0x265E5A51,	0xE9B6C7AA,	0xD62F105D,	0x2441453,	0xD8A1E681,	0xE7D3FBC8,
0x21E1CDE6,	0xC33707D6,	0xF4D50D87,	0x455A14ED,	0xA9E3E905,	0xFCEFA3F8,	0x676F02D9,	0x8D2A4C8A,
0xFFFA3942,	0x8771F681,	0x6D9D6122,	0xFDE5380C,	0xA4BEEA44,	0x4BDECFA9,	0xF6BB4B60,	0xBEBFBC70,
0x289B7EC6,	0xEAA127FA,	0xD4EF3085,	0x4881D05,	0xD9D4D039,	0xE6DB99E5,	0x1FA27CF8,	0xC4AC5665,
0xF4292244,	0x432AFF97,	0xAB9423A7,	0xFC93A039,	0x655B59C3,	0x8F0CCC92,	0xFFEFF47D,	0x85845DD1,
0x6FA87E4F,	0xFE2CE6E0,	0xA3014314,	0x4E0811A1,	0xF7537E82,	0xBD3AF235,	0x2AD7D2BB,	0xEB86D391
};
//循环移位S表
const uint32_t S[0x40] = { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
						 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
						 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
						 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };
//CV缓存，用于保存每一轮运算的结果
uint32_t A, B , C, D;
//轮操作数X[k]
uint32_t X[0x10];
//字节序转换
void Uint2Byte(uint32_t val, uint8_t* bytes){
	bytes[0] = (uint8_t)val;
	bytes[1] = (uint8_t)(val >> 8);
	bytes[2] = (uint8_t)(val >> 16);
	bytes[3] = (uint8_t)(val >> 24);
}
void Uint64ToByte(uint64_t val, uint8_t* bytes) {

	bytes[0] = (uint8_t)val;
	bytes[1] = (uint8_t)(val >> 8);
	bytes[2] = (uint8_t)(val >> 0x10);
	bytes[3] = (uint8_t)(val >> 0x18);
	bytes[4] = (uint8_t)(val >> 0x20);
	bytes[5] = (uint8_t)(val >> 0x28);
	bytes[6] = (uint8_t)(val >> 0x30);
	bytes[7] = (uint8_t)(val >> 0x38);
}
uint32_t Byte2Uint32(const uint8_t* bytes){
	return (uint32_t)bytes[0]
		| ((uint32_t)bytes[1] << 8)
		| ((uint32_t)bytes[2] << 16)
		| ((uint32_t)bytes[3] << 24);
}
//轮函数, 执行一个轮次，对一个0x40字节的分组buf进行运算，结果保存在CV缓冲区
void __fastcall wFunc(uint8_t* buf) {
	int i;
	for (i = 0; i < 0x10; i++) {//轮操作数X初始化,将0x40字节的字节流转换成0x10个uint32
		X[i] = Byte2Uint32(buf + i * 4);
	}
	//MD5分组算法
	uint32_t a = A, b = B, c = C, d = D, temp, g, k;
	for (i = 0; i < 0x40; i++) {
		if (i < 0x10) {
			g = F(b, c, d);
			k = i;
		}
		else if (i < 0x20) {
			g = G(b, c, d);
			k = (1 + 5 * i) % 0x10;
		}
		else if (i < 0x30) {
			g = H(b, c, d);
			k = (5 + 3 * i) % 0x10;
		}
		else {
			g = I(b, c, d);
			k = (7 * i) % 0x10;
		}
		temp = d;
		d = c;
		c = b;
		b = b + LEFTROTATE((a + g + X[k] + T[i]), S[i]);
		a = temp;
	}
	A += a;
	B += b;
	C += c;
	D += d;
}

//MD5主函数，执行成功返回0，否则返回非0
int MD5(const char* fileName, uint8_t* result) {
	FILE* fp = NULL; //用以记录文件对象
	uint8_t buf[0x40];//读取一个文件数据分组,并保存于此，用以进行轮运算
	uint64_t bits = 0i64;//记录读取位数，并作为最后一轮运算的补位数据
	//CV初始化
	A = 0x67452301, B = 0xEFCDAB89, C = 0x98BADCFE, D = 0X10325476;
	if(!fopen_s(&fp, fileName, "rb+") && fp) {
		while (!feof(fp)) {
			memset(buf, 0, sizeof(buf));
			size_t len = fread(buf, 1, 0x40, fp);//从文件流读取数据
			bits += (uint64_t)len << 3;//更新读取位数
			if (feof(fp)) {//读取到EOF，也就是文件已经读完了
				//此时len<=0x40
				if (len > 0x38) {//也就是模512超过了448，剩余部分少于64，这时很明显需要两个轮次
					//先执行第一个轮次，组建最后一轮次的分组的时候，先将前面0x38字节写好
					if (len == 0x40) {
						//刚好满一组数据，直接执行一个伦次
						wFunc(buf);
						//对最后一轮次的数据填充10...，最后8字节用于写入位数bits,因此填充到0x38
						buf[0] = 0x80;//填充10000....
						memset(buf+1,0,0x37);
					}
					else{
						//不满一组，但剩下的空间不够填充位数bits
						//先填充100000..使得这个分组长度达到512位,并执行一个轮次
						buf[len] = 0x80;
						memset(buf + len + 1, 0, 0x3F - len);
						wFunc(buf);
						//同样对最后一轮次的分组进行填充
						//由于1已经在前一分组填充了，只需要简单粗暴地将前面0x38字节变成就行了，剩下8字节填充位数bits
						memset(buf, 0, 0x38);
					}
				}
				else {//也就是模512不足448,那么这就是最后一个分组了
					// 同样对最后一轮次的分组进行填充
					//前面0x38字节填充10000...
					buf[len] = 0x80;
					memset(buf + len + 1, 0, 0x37 - len);
				}
				//通过上面的处理，现在这个轮次的分组只有bits没有填充进去了
				//将bits以字节流形式写入后8字节.由于bits是小端存储的，所以实质上是将其转换成大端形式加入分组
				Uint64ToByte(bits, buf + 0x38);
			}
			wFunc(buf);//一个伦次的计算
		}
		//CV缓存是4个小端存储的uint32,将其转换为128位的字节流
		Uint2Byte(A, result);
		Uint2Byte(B, result + 4);
		Uint2Byte(C, result + 8);
		Uint2Byte(D, result + 12);
		return 0;
	}
	else {
		fprintf(stderr,"[ERROR]Open file \"%s\": ", fileName);
		perror(NULL);
		return 1;
	}
}
#endif // !MD5_H
