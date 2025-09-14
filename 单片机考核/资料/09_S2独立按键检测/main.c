#include<reg52.h>  //51头文件
#define uchar unsigned char//宏定义
#define uint unsigned int  //宏定义
sbit we = P2^7;	//数码管位选
sbit du = P2^6;	//数码管段选
sbit key_s2 = P3^0;//S2按键位定义
//数码管段选表
uchar code leddata[]={ 
 
                0x3F,  //"0"
                0x06,  //"1"
                0x5B,  //"2"
                0x4F,  //"3"
                0x66,  //"4"
                0x6D,  //"5"
                0x7D,  //"6"
                0x07,  //"7"
                0x7F,  //"8"
                0x6F,  //"9"
                0x77,  //"A"
                0x7C,  //"B"
                0x39,  //"C"
                0x5E,  //"D"
                0x79,  //"E"
                0x71,  //"F"
                0x76,  //"H"
                0x38,  //"L"
                0x37,  //"n"
                0x3E,  //"u"
                0x73,  //"P"
                0x5C,  //"o"
                0x40,  //"-"
                0x00,  //熄灭
                0x00  //自定义
 
                         };
//毫秒级延时函数
void delay(uint z)
{
	uint x,y;
	for(x = z; x > 0; x--)
		for(y = 114; y > 0 ; y--);
}

void main()
{
	uchar num;
	we = 1;
	P0 = 0xfe; //选通第一个数码管 
	we = 0;
	while(1)
	{
		du = 1;
		P0 = leddata[num]; //显示S2按下次数
		du = 0;
		if(key_s2 == 0)//判断S2是否按键
		{
			delay(5);//软件延时
			if(key_s2 == 0)
			{
				num++; //计数加1
			}
			while(!key_s2);	//松手检测
		}
		if(num == 10) //当数值为超过9时归零
		{
			num = 0;
		}	
	}
}