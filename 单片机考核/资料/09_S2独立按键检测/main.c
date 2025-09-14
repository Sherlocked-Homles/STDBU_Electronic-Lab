#include<reg52.h>  //51ͷ�ļ�
#define uchar unsigned char//�궨��
#define uint unsigned int  //�궨��
sbit we = P2^7;	//�����λѡ
sbit du = P2^6;	//����ܶ�ѡ
sbit key_s2 = P3^0;//S2����λ����
//����ܶ�ѡ��
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
//���뼶��ʱ����
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
	P0 = 0xfe; //ѡͨ��һ������� 
	we = 0;
	while(1)
	{
		du = 1;
		P0 = leddata[num]; //��ʾS2���´���
		du = 0;
		if(key_s2 == 0)//�ж�S2�Ƿ񰴼�
		{
			delay(5);//�����ʱ
			if(key_s2 == 0)
			{
				num++; //������1
			}
			while(!key_s2);	//���ּ��
		}
		if(num == 10) //����ֵΪ����9ʱ����
		{
			num = 0;
		}	
	}
}