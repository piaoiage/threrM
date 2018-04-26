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



/*----------- 函数申明---------------*/

void Go_case0(u16 x1,u16 x2,u16 y);
void Go_case1(u16 x1,u16 y1,u16 psc);
void delay_s(u8 t);




/*-----------全局变量定义方便mian.c里面的函数共同使用-----------*/

double Buffer[65],Output,xn[50],Xk[50];
static int MODE,i,j,k;



/*-----------目前写过最长的主函数，希望后期优化能够完美封装-------------*/

int main(void)
{
    double BuffRe1[5],BuffRe2[4];
    u8 a,b,index1,index2,recover0,recover1;     //判断函数变量
    u8 t;
    delay_init();	    	 //延时函数初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
    uart_init(115200);	 	//串口初始化为115200			     //LED端口初始化
    EXTIX_Init();
    LED_Init();
    OLED_Init();
    OLED_ShowString(0,0,"Welcome",24);
    OLED_ShowString(0,25,"Wating...",16);
    OLED_Refresh_Gram();      
    TPAD_Init(6);
    Adc_Init();
    GUI_Init();
    tp_dev.init();         //触摸控制器初始化
    VS_Init();	  				//初始化VS1053
    my_mem_init(SRAMIN);		//初始化内部内存池
    exfuns_init();				//为fatfs相关变量申请内存
    f_mount(fs[0],"0:",1); 		//挂载SD卡
    f_mount(fs[1],"1:",1); 		//挂载FLASH.
    while(font_init()) 			//检查字库
    {
        LCD_ShowString(30,50,200,16,16,"Font Error!");
        delay_ms(200);
        LCD_Fill(30,50,240,66,WHITE);//清除显示
    }

    while(!TPAD_Scan(0))
    {
        Show_Str(150,380,200,24,"按TPAD启动设备",24,0);             //提示通过TPAD启动设备
        delay_ms(50);
    }
    LCD_Clear(WHITE);
    POINT_COLOR=BLACK;
    LCD_ShowString(160,350,200,24,24,"Entering...");
    OLED_Clear();
    OLED_ShowString(0,0,"LOADING",24);
    OLED_Refresh_Gram();
    Entering(0);                            //加载进入画面
		delay_s(2);
    LCD_Clear(WHITE);
    GUI_LCD1();                           //启动显示为主界面
    while(1)
    {  
        tp_dev.scan(0);
        if(tp_dev.x[0]>lcddev.width-60&&tp_dev.y[0]<50)            //通过使用屏幕上两个触摸点选择模式进行GUI显示   flag防止每次执行被优化出现问题
        {
				 MODE=1; 
				 change_gui(1);                                        //选择模式进行GUI0显示
				 k=0;                                                  //坐标画点回到原点重新开始画
				 LED0=0;                                            
				 delay_ms(500);
				 LED0=1; 	 
				}
        else if(tp_dev.x[0]<60&&tp_dev.y[0]<50)                 //通过使用屏幕上两个触摸点选择模式进行GUI显示
        { 
					MODE=0;                                               
          change_gui(0);                                       //选择模式进行GUI1显示
					i=0;j=0;k=0;                                         // 数据采集框内数据从第一个位置开始重新显示
          LED1=0;                                           
				  delay_ms(500);
				  LED1=1;  
        } 
				else                                                   
				{
        Output=Fir(Get_Adc_Average(ADC_Channel_10,5),Buffer);    //电压采集后进行FIR滤波处理[20Hz-20000	Hz]
        //DFT_caculate(Data_Store(Output,xn),Xk);                //FIR滤波后的数据进行50点离散傅里叶变换DFT运算
        OLED_Clear();
        OLED_ShowString(20,20,"RUN",24);
        OLED_Refresh_Gram();                                     //更新OLED显存
/*----------------------------------------出现高于正常情况的信号------------------------------------*/
					
        if(Output>1000)
        {
            for(index1=0; index1<10; index1++)

                BuffRe1[index1] =Fir(Get_Adc_Average(ADC_Channel_10,5),Buffer);    //判断是否出现生命迹象取10次OUTPUT模拟函数求导运算

            for(index2=0; index2<9; index2++)                                     //判断是否有波峰出现，从而进行滤除可能存在的尖峰信号
            {
                BuffRe2[index2]=BuffRe1[index2+1]-BuffRe1[index2];
                if(BuffRe2[index2]>0)
                    a+=1;
                else
                    b+=1;

            }
						
				/*---------------出现呼救信号主界面动作-------------*/
            if((a>3)&&(b>3))                                           //采用概率情况判断是否出现峰值信号
            {
             if(MODE%2==1)
                change_gui(0);
                LCD_Fill(46,490,414,748,WHITE);
                LCD_ShowString(100,520,28,24,24,"a=");
                LCD_ShowxNum(128,520,a,1,24,0);                        //界面切回到主界面，出口显示计算出的a,b两个值和“检验符合标准”
                LCD_ShowString(100,560,28,24,24,"b=");
                LCD_ShowxNum(128,560,b,1,24,0);
                Show_Str(150,620,200,24,"检验标准合格",24,0);           
               for(t=0; t<3; t++)
               {                                                          //结果输出窗口显示“疑似存在”
                LCD_Fill(295,365,400,400,WHITE);
                delay_ms(800);                                         
                Show_Str(300,370,120,24,"疑似存在",24,0);                 //结果显示闪烁3次，提醒观察者
                delay_ms(800);
                }
               delay_s(2);
               LCD_Clear(WHITE);
               a=0;b=0;
								
				/*---------人机交互部分，询问是否启动语音侦测--------*/
               while(1)
               { 
                Show_Str(120,380,200,24,"是否启动语音侦测",24,0);          //使用触点来咨询选择进行下一步
                LCD_ShowString(132,488,200,24,24,"YES");                 //进入触点 “YES”
                LCD_ShowString(288,488,200,24,24,"NO");                  //退出回到正常检测模式“NO”
                LCD_Draw_Circle(150,500,50);
                LCD_Draw_Circle(300,500,50);
                tp_dev.scan(0);
								  
		   /*---------启动语音侦测，初始化vs1053加载PATCH-------*/
                if(tp_dev.x[0]>130&&tp_dev.x[0]<170&&tp_dev.y[0]>480&&tp_dev.y[0]<520)     //触电检测到按下
                 {
                   LCD_Clear(WHITE);
                   Entering(1);
                   LCD_Clear(WHITE);
                   printf("Ram Test:0X%04X\r\n",VS_Ram_Test());            //串口打印RAM测试结果，可以在XCOM上查看
                   Show_Str(30,210,200,16,"正弦波测试...",16,0);           // 启动VS1053，正弦测试，
                   VS_Sine_Test();                                        //听到“滴 ”的一声表明启动成功
                   Show_Str(30,210,200,16,"启动成功",16,0);                 
                   recoder_play();                                       //执行语音侦测，可以保留录音到SD卡的文件夹下
                   change_gui(0); 
                   for(recover0=0;recover0<30;recover0++)									 //按下TPAD结束语音侦测，关闭VS1053，回到主界面
			             Output=Fir(Get_Adc_Average(ADC_Channel_10,5),Buffer);  //很重要的一点，硬件设备的延迟性，重复刷新直至传感器稳定输出
									 i=0;j=0;
                   LED1=1;
                   delay_ms(500);
                   LED1=0;
									 break;
                  }
								 
				/*----------跳出来，回到主界面1继续探索信号-----------*/
                 else if(tp_dev.x[0]>280&&tp_dev.x[0]<320&&tp_dev.y[0]>480&&tp_dev.y[0]<520)
								  {
									  LCD_Clear(WHITE);
										for(recover1=0;recover1<30;recover1++)
			              Output=Fir(Get_Adc_Average(ADC_Channel_10,5),Buffer);    //硬件设备的延迟性，重复刷新直至传感器稳定输出
									  change_gui(0);
									  delay_ms(800);
										i=0;j=0;
                    break;
                   }
								}
						 }
				  }
			 }
/*------------------------------------出现异常信号处理完毕---------------------------------------*/			



/*----------------------------------正常执行时，主界面和坐标界面的更新---------------------------*/			 
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




/*-------------翻屏1上时域坐标系画点函数 -------------*/
void Go_case0(u16 x1,u16 x2,u16 y)
{
    u8 size1=58,size2=26;
    if(Output>=0)
        LCD_ShowString(x1+i*size1,y+j*size2,8,16,16,"+");      //判断符号
    else
        LCD_ShowString(x1+i*size1,y+j*size2,8,16,16,"-");
    Output=abs(Output);
    if(Output<10000)
        LCD_ShowxNum(x2+i*size1,y+j*size2,Output,4,16,0);      //动态显示数据 缺点暂时没能实现符号和数字贴合
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


/*-------------翻屏2上时域坐标系画点函数 -------------*/



void Go_case1(u16 x1,u16 y1,u16 psc)
{
    POINT_COLOR=BLUE;
//    printf("%f\t",Output);
    Output=Output/psc;                          //psc :坐标被除数因子  18
    if(Output>0)
        LCD_Draw_Circle(x1+k,y1-Output,3);
    else
        LCD_Draw_Circle(53+k,220+abs(Output),3);         //x1  53 画点起始位置  y1 220  X坐标轴位置具体数值参考SHOW.C文件
    k+=7;
    if(k>=350)
    {
        k=0;
        LCD_Fill(50,100,400,380,WHITE);      //清屏坐标值
        LCD_DrawLine(50, 220, 400, 220)	;		                               //lcdshow_x_y(44,360,372,260);
    }
}


/*-------------一个很LOW的sec级别的演示自定义函数-------------*/
void delay_s(u8 t)
{
    u8 i;
    for(i=0; i<t; i++)
        delay_ms(500*2);

}




