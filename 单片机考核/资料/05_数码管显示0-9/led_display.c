#include <reg52.h>//51头文件

#define uchar unsigned char//宏定义
#define uint  unsigned int //宏定义
sbit we = P2^7;	//位定义数码管位选锁存器接口
sbit du = P2^6;	//位定义数码管段选锁存器接口

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

		uchar i;
		we = 1;//打开位选
		P0 = 0xfe;//1111 1110 第一位数码管显示，注意数码管的排序，P0.0对应连接的是左边第一位数码管
		we = 0;	//关闭位选
				
		while(1) //大循环
		{
			du = 1;	//打开段选
			P0 = leddata[i];
			du = 0;	//关闭段选
			delay(500);//延时500毫秒
			i++;   //i自加1
			if(i == 10)	//当数值为10时，清零
			{
				i = 0;
			}	
		}
}