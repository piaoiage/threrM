#include "delay.h"
#include "lcd.h"
#include "usart.h"
#include "exti.h"
#include "adc.h"
#include "show.h"
#include "fir.h"
#include "key.h"
#include "dft.h"
#include "malloc.h"
#include "sdio_sdcard.h"
#include "ff.h"
#include "exfuns.h"
#include "text.h"
#include "vs1053.h"
#include "recorder.h"
#include "tpad.h"
#include "lcd.h"
#include "led.h"
#include "oled.h"
#include "touch.h"



/*----------- ��������---------------*/

void Go_case0(u16 x1,u16 x2,u16 y);
void Go_case1(u16 x1,u16 y1,u16 psc);
void delay_s(u8 t);




/*-----------ȫ�ֱ������巽��mian.c����ĺ�����ͬʹ��-----------*/

double Buffer[65],Output,xn[50],Xk[50];
static int MODE,i,j,k;



/*-----------Ŀǰд�������������ϣ�������Ż��ܹ�������װ-------------*/

int main(void)
{
    double BuffRe1[5],BuffRe2[4];
    u8 a,b,index1,index2,recover0,recover1;     //�жϺ�������
    u8 t;
    delay_init();	    	 //��ʱ������ʼ��
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200			     //LED�˿ڳ�ʼ��
    EXTIX_Init();
    LED_Init();
    OLED_Init();
    OLED_ShowString(0,0,"Welcome",24);
    OLED_ShowString(0,25,"Wating...",16);
    OLED_Refresh_Gram();      
    TPAD_Init(6);
    Adc_Init();
    GUI_Init();
    tp_dev.init();         //������������ʼ��
    VS_Init();	  				//��ʼ��VS1053
    my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
    exfuns_init();				//Ϊfatfs��ر��������ڴ�
    f_mount(fs[0],"0:",1); 		//����SD��
    f_mount(fs[1],"1:",1); 		//����FLASH.
    while(font_init()) 			//����ֿ�
    {
        LCD_ShowString(30,50,200,16,16,"Font Error!");
        delay_ms(200);
        LCD_Fill(30,50,240,66,WHITE);//�����ʾ
    }

    while(!TPAD_Scan(0))
    {
        Show_Str(150,380,200,24,"��TPAD�����豸",24,0);             //��ʾͨ��TPAD�����豸
        delay_ms(50);
    }
    LCD_Clear(WHITE);
    POINT_COLOR=BLACK;
    LCD_ShowString(160,350,200,24,24,"Entering...");
    OLED_Clear();
    OLED_ShowString(0,0,"LOADING",24);
    OLED_Refresh_Gram();
    Entering(0);                            //���ؽ��뻭��
		delay_s(2);
    LCD_Clear(WHITE);
    GUI_LCD1();                           //������ʾΪ������
    while(1)
    {  
        tp_dev.scan(0);
        if(tp_dev.x[0]>lcddev.width-60&&tp_dev.y[0]<50)            //ͨ��ʹ����Ļ������������ѡ��ģʽ����GUI��ʾ   flag��ֹÿ��ִ�б��Ż���������
        {
				 MODE=1; 
				 change_gui(1);                                        //ѡ��ģʽ����GUI0��ʾ
				 k=0;                                                  //���껭��ص�ԭ�����¿�ʼ��
				 LED0=0;                                            
				 delay_ms(500);
				 LED0=1; 	 
				}
        else if(tp_dev.x[0]<60&&tp_dev.y[0]<50)                 //ͨ��ʹ����Ļ������������ѡ��ģʽ����GUI��ʾ
        { 
					MODE=0;                                               
          change_gui(0);                                       //ѡ��ģʽ����GUI1��ʾ
					i=0;j=0;k=0;                                         // ���ݲɼ��������ݴӵ�һ��λ�ÿ�ʼ������ʾ
          LED1=0;                                           
				  delay_ms(500);
				  LED1=1;  
        } 
				else                                                   
				{
        Output=Fir(Get_Adc_Average(ADC_Channel_10,5),Buffer);    //��ѹ�ɼ������FIR�˲�����[20Hz-20000	Hz]
        //DFT_caculate(Data_Store(Output,xn),Xk);                //FIR�˲�������ݽ���50����ɢ����Ҷ�任DFT����
        OLED_Clear();
        OLED_ShowString(20,20,"RUN",24);
        OLED_Refresh_Gram();                                     //����OLED�Դ�
/*----------------------------------------���ָ�������������ź�------------------------------------*/
					
        if(Output>1000)
        {
            for(index1=0; index1<10; index1++)

                BuffRe1[index1] =Fir(Get_Adc_Average(ADC_Channel_10,5),Buffer);    //�ж��Ƿ������������ȡ10��OUTPUTģ�⺯��������

            for(index2=0; index2<9; index2++)                                     //�ж��Ƿ��в�����֣��Ӷ������˳����ܴ��ڵļ���ź�
            {
                BuffRe2[index2]=BuffRe1[index2+1]-BuffRe1[index2];
                if(BuffRe2[index2]>0)
                    a+=1;
                else
                    b+=1;

            }
						
				/*---------------���ֺ����ź������涯��-------------*/
            if((a>3)&&(b>3))                                           //���ø�������ж��Ƿ���ַ�ֵ�ź�
            {
             if(MODE%2==1)
                change_gui(0);
                LCD_Fill(46,490,414,748,WHITE);
                LCD_ShowString(100,520,28,24,24,"a=");
                LCD_ShowxNum(128,520,a,1,24,0);                        //�����лص������棬������ʾ�������a,b����ֵ�͡�������ϱ�׼��
                LCD_ShowString(100,560,28,24,24,"b=");
                LCD_ShowxNum(128,560,b,1,24,0);
                Show_Str(150,620,200,24,"�����׼�ϸ�",24,0);           
               for(t=0; t<3; t++)
               {                                                          //������������ʾ�����ƴ��ڡ�
                LCD_Fill(295,365,400,400,WHITE);
                delay_ms(800);                                         
                Show_Str(300,370,120,24,"���ƴ���",24,0);                 //�����ʾ��˸3�Σ����ѹ۲���
                delay_ms(800);
                }
               delay_s(2);
               LCD_Clear(WHITE);
               a=0;b=0;
								
				/*---------�˻��������֣�ѯ���Ƿ������������--------*/
               while(1)
               { 
                Show_Str(120,380,200,24,"�Ƿ������������",24,0);          //ʹ�ô�������ѯѡ�������һ��
                LCD_ShowString(132,488,200,24,24,"YES");                 //���봥�� ��YES��
                LCD_ShowString(288,488,200,24,24,"NO");                  //�˳��ص��������ģʽ��NO��
                LCD_Draw_Circle(150,500,50);
                LCD_Draw_Circle(300,500,50);
                tp_dev.scan(0);
								  
		   /*---------����������⣬��ʼ��vs1053����PATCH-------*/
                if(tp_dev.x[0]>130&&tp_dev.x[0]<170&&tp_dev.y[0]>480&&tp_dev.y[0]<520)     //�����⵽����
                 {
                   LCD_Clear(WHITE);
                   Entering(1);
                   LCD_Clear(WHITE);
                   printf("Ram Test:0X%04X\r\n",VS_Ram_Test());            //���ڴ�ӡRAM���Խ����������XCOM�ϲ鿴
                   Show_Str(30,210,200,16,"���Ҳ�����...",16,0);           // ����VS1053�����Ҳ��ԣ�
                   VS_Sine_Test();                                        //�������� ����һ�����������ɹ�
                   Show_Str(30,210,200,16,"�����ɹ�",16,0);                 
                   recoder_play();                                       //ִ��������⣬���Ա���¼����SD�����ļ�����
                   change_gui(0); 
                   for(recover0=0;recover0<30;recover0++)									 //����TPAD����������⣬�ر�VS1053���ص�������
			             Output=Fir(Get_Adc_Average(ADC_Channel_10,5),Buffer);  //����Ҫ��һ�㣬Ӳ���豸���ӳ��ԣ��ظ�ˢ��ֱ���������ȶ����
									 i=0;j=0;
                   LED1=1;
                   delay_ms(500);
                   LED1=0;
									 break;
                  }
								 
				/*----------���������ص�������1����̽���ź�-----------*/
                 else if(tp_dev.x[0]>280&&tp_dev.x[0]<320&&tp_dev.y[0]>480&&tp_dev.y[0]<520)
								  {
									  LCD_Clear(WHITE);
										for(recover1=0;recover1<30;recover1++)
			              Output=Fir(Get_Adc_Average(ADC_Channel_10,5),Buffer);    //Ӳ���豸���ӳ��ԣ��ظ�ˢ��ֱ���������ȶ����
									  change_gui(0);
									  delay_ms(800);
										i=0;j=0;
                    break;
                   }
								}
						 }
				  }
			 }
/*------------------------------------�����쳣�źŴ������---------------------------------------*/			



/*----------------------------------����ִ��ʱ����������������ĸ���---------------------------*/			 
        switch(MODE%2)
        {
        case 0:
            Go_case0(47,55,500);
            break;
        case 1:
            Go_case1(53,220,18);
            break;
        }
			   
	 }
}




/*-------------����1��ʱ������ϵ���㺯�� -------------*/
void Go_case0(u16 x1,u16 x2,u16 y)
{
    u8 size1=58,size2=26;
    if(Output>=0)
        LCD_ShowString(x1+i*size1,y+j*size2,8,16,16,"+");      //�жϷ���
    else
        LCD_ShowString(x1+i*size1,y+j*size2,8,16,16,"-");
    Output=abs(Output);
    if(Output<10000)
        LCD_ShowxNum(x2+i*size1,y+j*size2,Output,4,16,0);      //��̬��ʾ���� ȱ����ʱû��ʵ�ַ��ź���������
    else if(Output<1000)
        LCD_ShowxNum(x2+i*size1,y+j*size2,Output,3,16,0);
    else if(Output<100)
        LCD_ShowxNum(x2+i*size1,y+j*size2,Output,2,16,0);
    else if(Output<10)
        LCD_ShowxNum(x2+i*size1,y+j*size2,Output,1,16,0);
    i++;
    if(i==6)
    {
        j++;
        i=0;
    }
    if(j==8)  j=0;

}


/*-------------����2��ʱ������ϵ���㺯�� -------------*/



void Go_case1(u16 x1,u16 y1,u16 psc)
{
    POINT_COLOR=BLUE;
//    printf("%f\t",Output);
    Output=Output/psc;                          //psc :���걻��������  18
    if(Output>0)
        LCD_Draw_Circle(x1+k,y1-Output,3);
    else
        LCD_Draw_Circle(53+k,220+abs(Output),3);         //x1  53 ������ʼλ��  y1 220  X������λ�þ�����ֵ�ο�SHOW.C�ļ�
    k+=7;
    if(k>=350)
    {
        k=0;
        LCD_Fill(50,100,400,380,WHITE);      //��������ֵ
        LCD_DrawLine(50, 220, 400, 220)	;		                               //lcdshow_x_y(44,360,372,260);
    }
}


/*-------------һ����LOW��sec�������ʾ�Զ��庯��-------------*/
void delay_s(u8 t)
{
    u8 i;
    for(i=0; i<t; i++)
        delay_ms(500*2);

}




