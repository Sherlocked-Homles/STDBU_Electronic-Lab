#include <reg52.h>
#define uchar unsigned char
#define uint  unsigned int
sbit we = P2^7;
sbit du = P2^6;
uchar KeyValue = 22;//����ֵ
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
                0x00,  //Ϩ��
                0x00  //�Զ���
 
                         };
void delay(uint z)
{
	uint x,y;
	for(x = z; x > 0; x--)
		for(y = 114; y > 0 ; y--);
}

//���20�������ļ���ɨ�躯���ͼ�������̵�ɨ�躯�����в�ͬ���û��������бȶ��ҳ�����
void KeyScan()	//������ֵ���Ӻ���
{
	P3 = 0xf0;//�ж���һ�б�����
	if(P3 != 0xf0)
	{
		delay(5);//�������
		if( P3 != 0xf0)
		{
			switch(P3)
			{
				case 0xe0:	KeyValue = 0;	break;
				case 0xd0:	KeyValue = 1;	break;
				case 0xb0:	KeyValue = 2;	break;
				case 0x70:	KeyValue = 3;	break;
			}
			P3 = 0x0f;//�ж���һ�б�����
			switch(P3)
			{
				case 0x0e:	KeyValue = KeyValue;	break;
				case 0x0d:	KeyValue += 4;	break;
				case 0x0b:	KeyValue += 8;	break;
				case 0x07:	KeyValue += 12;	break;
			}
			while(P3 != 0x0f);//���ּ��
		}			
	}
	P3 = 0xff;//�ж϶�������
	if(P3 != 0xff)
	{
		delay(5);//�������
		if( P3 != 0xff)
		{
			switch(P3)
			{
				case 0xfe:	KeyValue = 16;	break;
				case 0xfd:	KeyValue = 17;	break;
				case 0xfb:	KeyValue = 18;	break;
				case 0xf7:	KeyValue = 19;	break;
			}
			while(P3 != 0xff);//���ּ��
		}	
	}	
}



void main()
{
	we = 1;//��λѡ
	P0 = 0;//��λ�����ȫ��ʾ
	we = 0;//����λѡ

	du = 1;//�򿪶�ѡ��
	P0 = 0;
	while(1)
	{
	 	KeyScan();
		P0 = leddata[KeyValue];
	}
}