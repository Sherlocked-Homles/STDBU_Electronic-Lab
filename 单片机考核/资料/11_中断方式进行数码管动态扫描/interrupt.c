/*
定时器0中断方式进行数码管动态显示扫描，
非软件延时方式去动态扫描，
按下S2数码管数值加1，按下S3数码管数值减1
--------------------------------------
--------------------------------------
与软件延时方式去做数码管动态扫描的区别：
软件延时扫描，当按下按键时数码管只会显示1位数
中断方式扫描，当按下按键时不影响数码管应显示的位数
*/
#include <reg52.h>

#define uchar unsigned char
#define uint  unsigned int

sbit we = P2^7;	//位定义数码管位选锁存器接口
sbit du = P2^6;	 //位定义数码管位选锁存器接口
sbit key_s2 = P3^0;//S2按键位定义
sbit key_s3 = P3^1;//S3按键位定义

uchar i;//数码管显示位选变量
uchar num[] = {0, 0, 0, 0}; //百十个位，总数暂存数组
//数码管0~9段选表
uchar code leddata[]={0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
//数码管1~3位选表
uchar code DPY[]={0xfe, 0xfd, 0xfb};

//毫秒级延时函数
void delay(uint z)
{
	uint x,y;
	for(x = z; x > 0; x--)
		for(y = 114; y > 0 ; y--);
}
//中断服务特殊功能寄存器配置
void init()
{
	
	TMOD = 0x01;  //定时器16为计数工作模式
	TH0 =0xED;
	TL0 =0xFF; //5ms
	ET0 = 1; //开定时器0中断
	TR0 = 1;//启动定时器0
	EA = 1;	//开总中断
}

void main()
{
	init();//初始化定时器0
	while(1)
	{
		if(key_s2 == 0)//判断S2是否按键
		{
			delay(5);//软件延时
			if(key_s2 == 0)
			{
				if(num[3] != 255) //当等于255时不再加1
				num[3]++; //加1
			}
			while(!key_s2);	//松手检测
		}
		if(key_s3 == 0)//判断S3是否按键
		{
			delay(5);//软件延时
			if(key_s3 == 0)
			{
				if(num[3] != 0)	//当等于0时，不再减1
				num[3]--; //减1
			}
			while(!key_s3);	//松手检测
		}
	}
}

//定时器0中断服务程序  做数码管动态扫描，不用软件延时
void timer0() interrupt 1
{
	TH0 =0xED;
	TL0 =0xFF; //5ms  模式1非自动重装需要手动重装

 	if(i < 3)
	{
		num[0] = num[3] / 100; //计算百位
		num[1] = num[3] % 100 / 10;	//计算十位
		num[2] = num[3] % 10;//计算个位
		P0 = 0xff; //清除断码
		we = 1;//打开位选
		P0 = DPY[i];
		we = 0;	//关闭位选
	
		P0 = 0xff; //清除断码
		du = 1;	//打开段选
		P0 = leddata[num[i]];
		du = 0;	//关闭段选
		i++;//显示下一位
	}
	else
	{
		i = 0;
	}

}