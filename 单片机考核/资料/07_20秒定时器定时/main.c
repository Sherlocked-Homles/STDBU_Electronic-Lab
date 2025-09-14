/*定时器1，定时模式 工作模式1 16位计数器，
定时20秒后使能蜂鸣器*/
#include <reg52.h> //51头文件
#define uchar unsigned char//宏定义
#define uint  unsigned int//宏定义
sbit we = P2^7;//位定义数码管位选锁存器接口
sbit du = P2^6;//位定义数码管段选锁存器接口
sbit beep = P2^3;//蜂鸣器接口

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

//两位数码管动态显示函数
void display(uchar i)
{
	uchar shi, ge;
	shi = i / 10;//求模  i除以10取商的整数部分
	ge  = i % 10;//求余  i除以10取余数部分

	P0 = 0xff; //清除断码
	we = 1;//打开位选
	P0 = 0xfe;//1111 1110 只选通第一位数码管
	we = 0;	//关闭位选

	du = 1;	//打开段选
	P0 = leddata[shi];
	du = 0;	//关闭段选
	delay(5);//延时5毫秒
	
	P0 = 0xff;//清除断码
	we = 1;	//打开位选
	P0 = 0xfd;//1111 1101  只选通第二位数码管
	we = 0;	//关闭位选
	
	du = 1;//打开段选
	P0 = leddata[ge]; 
	du = 0;	//关闭段选
	delay(5);//延时5毫秒	
}

void main()
{
		uchar a; //50次数计数
		uchar b;//秒计数
		TR1 = 1;//启动T1
		TMOD = 0x10;//T1为定时器，工作模式1 16位计数器
		TH1 = 0x4b;
		TL1 = 0xfc;//0x4bfc	定时50ms				
		while(1)
		{
			if(TF1 == 1)//判断T1是否溢出
			{
				TH1 = 0x4b;
				TL1 = 0xfc;//0x4bfc	定时50ms
				TF1 = 0;//清零便于下次判断
				a++;//50毫秒计数加1	
			}
			if(a == 20)//判断是否到1秒
			{
				a = 0;//清零便于下次记录50ms的次数
				b++;//秒加1
			}

			display(b);//显示秒的值

			if(b == 20)//检查是否到20秒
			{
				TR1 = 0;//时间到关闭定时器1
				beep = 0; //使能蜂鸣器
				delay(4000);//使能时间4秒
				beep = 1;//关闭蜂鸣器
				while(1); //停止程序
			}

			
		}
}