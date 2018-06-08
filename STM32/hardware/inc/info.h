#ifndef _INFO_H_
#define _INFO_H_







#define SSID_ADDRESS		0		//起始地址，第一个数据表示长度。最大59字节。
#define PSWD_ADDRESS		60		//起始地址，第一个数据表示长度。最大59字节。

#define DEVID_ADDRESS       120		//起始地址，第一个数据表示长度。最大19字节。
#define AKEY_ADDRESS        140		//起始地址，第一个数据表示长度。最大59字节。





unsigned char Info_Check(void);

unsigned char Info_WifiLen(unsigned char sp);

_Bool Info_Read(void);

_Bool Info_Alter(char *info);


#endif
