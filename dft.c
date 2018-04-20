#include<math.h>
#include<stdio.h>
#define PI 3.14159265
#define N 8

/*
���������ź� �źŶ���Ϊdouble����
exp(-j*2*PI/N)*n*K��������ŷ����ʽת��Ϊ cos(2*PI/N*n*k) - j*sin(2*PI/N*n*k)
����źų���cos(2*PI/N*n*k)���Ƕ��źŽ���DFT��ʵ��������sin(2*PI/N*n*k)����DFT���鲿
��data[k]�����д洢��data[k]����Ϊ complex_num�������ͣ�complex_num.real����double���͵�
complex_num.imagҲ��double�������� 
���ı任֮�������Xk[k]���洢����DFT��ʵ��ƽ���鲿ƽ�����ڿ���֮���ֵ����double���� 
double Xk[N] = {0};�� Xk[N]�����洢�任������ݣ���main()�����ж��壬���ݸ� DFT_caculate()����
���βΣ��ں����оͻ��Xk[N]�е����ֽ��мӹ����� Xk[N]�е���ֵ�Ѿ����޸ģ���˺����˳���
Xk[N]�е���ֵ�Ѿ����ı䣬����Ҫ�������з���ֵ������Ҳ���Է���Xk[N]������Xk[N]������Xk[N]
�������Ƕ�Xk[N]�ļӹ� 
*/ 

void DFT_caculate(double *signal,double *Xk);

typedef struct complex_num
{
double real;                                                 
double imag;
}num;                             //����ṹ������
 
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
//���źŽ���DFT 
void DFT_caculate(double *signal,double *Xk)
{
num data[N];  //����ṹ������complex_num�Ľṹ������data[N]����DFT���ʵ�����鲿 
int n,k;   
for(k = 0;k < N;k ++)
for(n = 0;n < N;n ++)
{
data[k].real = data[k].real + signal[n]*cos(2*PI/N*n*k); //DFTʵ�� 
data[k].imag = data[k].imag + signal[n]*sin(2*PI/N*n*k); //DFT�鲿  
}
for(k = 0;k < N;k ++)
{
Xk[k] = sqrt(data[k].real * data[k].real + data[k].imag * data[k].imag);
}  // Xk[k]��DFT��ʵ��ƽ���鲿ƽ�����ڿ���֮���ֵ����double����
}
