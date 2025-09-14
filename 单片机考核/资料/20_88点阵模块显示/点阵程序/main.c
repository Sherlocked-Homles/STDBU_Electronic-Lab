//������ʾ �� - ���ģ���״�� 
#include <reg52.h>
#include <intrins.h>

//����ģ��ӿڶ���
sbit LEDARRAY_LAT = P3^6;//����Ĵ���ʱ��
sbit LEDARRAY_CLK = P3^5;//��λ�Ĵ���ʱ������� 
sbit LEDARRAY_DI = P3^4;//�������������

//��ά���� �ں��������룬ȡ����ʽ��������ڱ�Ŀ¼��
unsigned char  code Display_word[2][8] = {
0x00,0x48,0x2A,0x8E,0xFB,0x0E,0x2A,0x48,//��
0x0C,0x1E,0x3E,0x7C,0x7C,0x3E,0x1E,0x0C //���ģ���״�� 
};

//SPIͨ�ź��� ����һ��ֱ�����ݸ�����ģ��
void Send_Byte( unsigned char dat)
{
	unsigned char i;//ѭ����������
	LEDARRAY_CLK = 0;//������λʱ��
	_nop_();	
	LEDARRAY_LAT = 0;//���ʹ���ʱ��
	_nop_();

	for( i = 0 ; i < 8 ; i++ ) //ѭ��8��д��һ���ֽ�����
	{
		if( dat&0x01 )
		{
			LEDARRAY_DI = 1;	
		}
		else
		{
			LEDARRAY_DI = 0;
		}
		
		LEDARRAY_CLK = 1;				//�����ط�������
		//				_nop_();
		LEDARRAY_CLK = 0;
		//				_nop_();
				
		dat >>= 1;			
	}		
}

void main()
{
	unsigned char k, temp;
	unsigned int i;//i����ֵԽ����ʾͣ����ʱ���Խ��
	while(1)
	{
			//��
		for(i = 0 ; i < 1000 ; i++)
		{
			temp = 0x7f;
			for(k = 0 ; k < 8 ; k++)		//һ����ĸ8BYTE
			{
				Send_Byte(temp);			//��ѡ��
				Send_Byte(Display_word[0][k]);			//������
				LEDARRAY_LAT = 1;					//��������
				_nop_();
			
				LEDARRAY_LAT = 0;
				_nop_();
				temp = (temp>>1)|0x80;
			}
		}
			//���ģ���״�� 
		for(i = 0 ; i < 1000 ; i++)
		{
			temp = 0x7f;
			for(k = 0 ; k < 8 ; k++)		//һ����ĸ4BYTE
			{
				Send_Byte(temp);			//��ѡ��
				Send_Byte(Display_word[1][k]);			//������
				LEDARRAY_LAT = 1;					//��������
				_nop_();
			
				LEDARRAY_LAT = 0;
				_nop_();
				temp = (temp>>1)|0x80;
			}
		}	
	}
}
