#include <reg52.h>
#define uchar unsigned char
#define uint  unsigned int
sbit we = P2^7;
sbit du = P2^6;
uchar KeyValue = 22;//按键值
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
void delay(uint z)
{
	uint x,y;
	for(x = z; x > 0; x--)
		for(y = 114; y > 0 ; y--);
}

//检测20个按键的键盘扫描函数和检测矩阵键盘的扫描函数略有不同，用户可以自行比对找出差异
void KeyScan()	//带返回值的子函数
{
	P3 = 0xf0;//判断哪一列被按下
	if(P3 != 0xf0)
	{
		delay(5);//软件消抖
		if( P3 != 0xf0)
		{
			switch(P3)
			{
				case 0xe0:	KeyValue = 0;	break;
				case 0xd0:	KeyValue = 1;	break;
				case 0xb0:	KeyValue = 2;	break;
				case 0x70:	KeyValue = 3;	break;
			}
			P3 = 0x0f;//判断哪一行被按下
			switch(P3)
			{
				case 0x0e:	KeyValue = KeyValue;	break;
				case 0x0d:	KeyValue += 4;	break;
				case 0x0b:	KeyValue += 8;	break;
				case 0x07:	KeyValue += 12;	break;
			}
			while(P3 != 0x0f);//松手检测
		}			
	}
	P3 = 0xff;//判断独立按键
	if(P3 != 0xff)
	{
		delay(5);//软件消抖
		if( P3 != 0xff)
		{
			switch(P3)
			{
				case 0xfe:	KeyValue = 16;	break;
				case 0xfd:	KeyValue = 17;	break;
				case 0xfb:	KeyValue = 18;	break;
				case 0xf7:	KeyValue = 19;	break;
			}
			while(P3 != 0xff);//松手检测
		}	
	}	
}



void main()
{
	we = 1;//打开位选
	P0 = 0;//八位数码管全显示
	we = 0;//锁存位选

	du = 1;//打开段选端
	P0 = 0;
	while(1)
	{
	 	KeyScan();
		P0 = leddata[KeyValue];
	}
}