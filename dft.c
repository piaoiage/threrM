#include<math.h>
#include<stdio.h>
#define PI 3.14159265
#define N 8

/*
首先输入信号 信号定义为double类型
exp(-j*2*PI/N)*n*K可以利用欧拉公式转换为 cos(2*PI/N*n*k) - j*sin(2*PI/N*n*k)
因此信号乘以cos(2*PI/N*n*k)就是对信号进行DFT的实部，乘以sin(2*PI/N*n*k)就是DFT的虚部
用data[k]来进行存储，data[k]定义为 complex_num数据类型，complex_num.real就是double类型的
complex_num.imag也是double数据类型 
最后的变换之后的序列Xk[k]来存储，是DFT后实部平方虚部平方和在开方之后的值，是double类型 
double Xk[N] = {0};中 Xk[N]用来存储变换后的数据，在main()函数中定义，传递给 DFT_caculate()函数
的形参，在函数中就会对Xk[N]中的数字进行加工处理， Xk[N]中的数值已经被修改，因此函数退出后，
Xk[N]中的数值已经被改变，不需要函数中有返回值，但是也可以返回Xk[N]，传入Xk[N]，返回Xk[N]
函数体是对Xk[N]的加工 
*/ 

void DFT_caculate(double *signal,double *Xk);

typedef struct complex_num
{
double real;                                                 
double imag;
}num;                             //定义结构体类型
 
int main()
{
int n,k;
double signal[N] = {0};
double Xk[N] = {0};
for(n = 0;n < N;n ++)
{
signal[n] = sin(2*PI*n/N);
printf("%lf ",signal[n]);
}
printf("\n");
DFT_caculate(signal,Xk);
for(n = 0;n < N;n ++)
{
printf("%lf ",Xk[n]);
}
return 0;
}
//对信号进行DFT 
void DFT_caculate(double *signal,double *Xk)
{
num data[N];  //定义结构体类型complex_num的结构体数组data[N]用来DFT后的实部和虚部 
int n,k;   
for(k = 0;k < N;k ++)
for(n = 0;n < N;n ++)
{
data[k].real = data[k].real + signal[n]*cos(2*PI/N*n*k); //DFT实部 
data[k].imag = data[k].imag + signal[n]*sin(2*PI/N*n*k); //DFT虚部  
}
for(k = 0;k < N;k ++)
{
Xk[k] = sqrt(data[k].real * data[k].real + data[k].imag * data[k].imag);
}  // Xk[k]是DFT后实部平方虚部平方和在开方之后的值，是double类型
}
