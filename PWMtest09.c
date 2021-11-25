/****************************************
//*pwm.c10
//*2017.12.07
//PWM����@�����Z���T�ƘA//�����Z���T�@���m�Ȓl�i�����j�o��
//���x�Ɖ����x�@�o��
//���̂̑��x�ɉ����ă��[�^�̑��x��ς���
//P����ŃY����ڕW�l��ݒ�
//�p�x�ɂ��2���[�^����
//�X���̃Y���̏C��
���[�^�h���C�oTB�U�U�P�QFNG�g�p
*****************************************/
//***********************************************************
#include<p30f4011.h>                             //�w�b�_�[//
#include<timer.h>
#include<adc10.h>
#include<pwm.h>
#include <stdio.h>
#include <uart.h>
#include <float.h>

//�o�O�C���p�@�΂̖{�ɏ���ĂȂ�
#define UART_RX_TX 0xFBE7//ICD2�R�l�N�^
#define UART_TX_PIN_NORMAL 0xF7FF  // UART TX pin operates normally 
//***********************************************************
_FOSC(CSW_FSCM_OFF & XT_PLL16);
//�N���b�c�N���j�^�[Off 8MHz�~16=128MHz (max120)
_FWDT(WDT_OFF);
//WDT Off
_FBORPOR(PBOR_ON & BORV_20& PWRT_64 & MCLR_EN);
//BOR ON,MCLR ON
_FGS(CODE_PROT_OFF);

//�R�[�h�v���e�N�g�@Off
//**************************************************************
#define CLOCK 120

//�O���[�o���錾
char Flag;
char Flag1;
unsigned int valueM1;
unsigned int valueM2;
//int	valueM3;
unsigned int duty;
unsigned int duty1;
unsigned int Duty;
unsigned int Duty1;

//UART�̐ݒ�p�����[�^
/*unsigned int config1 = UART_EN &				// UART���W���[���̗L��
UART_IDLE_CON &			// �A�C�h���E���[�h��������
UART_ALTRX_ALTTX& 		//UART_RX_TX& 	//�g�p�s���̐ݒ�(U1RT/U1TX)
UART_DIS_WAKE &			// �X���[�v�E���[�h����̃E�F�C�N�A�b�v���Ȃ�
UART_DIS_LOOPBACK &		// ���[�v�E�o�b�N��L��
UART_DIS_ABAUD &			// U1RX�s������̐M�����C���v�b�g�E�L���v�`���ɓ��͂���
UART_EVEN_PAR_8BIT &		// 8�r�b�g��/�p���e�B�Ȃ�
UART_1STOPBIT;			// �X�g�b�v�E�r�b�g1�r�b�g
*/

unsigned int config2 = UART_INT_TX_BUF_EMPTY &	// �o�b�t�@����ɂȂ����犄�荞�ݔ���
UART_TX_PIN_NORMAL &		// �ʏ퓮��
UART_TX_ENABLE &			// ���M����H��L��
UART_INT_RX_CHAR &		//��������M���邽�тɊ��荞��
UART_ADR_DETECT_DIS &	// 
UART_RX_OVERRUN_CLEAR;	// �I�[�o�[�����E�r�b�g���N���A   

//26,27pin�@(UART1)�g���Ƃ� config1�̑��� 
unsigned int config3 = UART_EN &				// UART���W���[���̗L��
UART_IDLE_CON &			// �A�C�h���E���[�h��������
/*UART_ALTRX_ALTTX&*/ 		UART_RX_TX& 	//�g�p�s���̐ݒ�(U1RT/U1TX)
UART_DIS_WAKE &			// �X���[�v�E���[�h����̃E�F�C�N�A�b�v���Ȃ�
UART_DIS_LOOPBACK &		// ���[�v�E�o�b�N��L��
UART_DIS_ABAUD &			// U1RX�s������̐M�����C���v�b�g�E�L���v�`���ɓ��͂���
UART_EVEN_PAR_8BIT &		// 8�r�b�g��/�p���e�B�Ȃ�
UART_1STOPBIT;			// �X�g�b�v�E�r�b�g1�r�b�g


//AD�R���o�[�^�[�̃p�����[�^�ݒ�
unsigned int ValueADCON1 =
ADC_MODULE_ON&
ADC_IDLE_CONTINUE&  //Idle mode �̓���
ADC_FORMAT_INTG&   //�ϊ����ʂ̃f�[�^�`��
ADC_CLK_TMR&       //�ϊ��J�n�g���K�\�[�X�w��@����̓^�C�}3�̂͂�
//ADC_CLK_MPWM&      //���[�^�[����PWM�ɂ��ϊ��J�n
ADC_AUTO_SAMPLING_ON&  //�T���v������
ADC_SAMPLE_SIMULTANEOUS&
//	ADC_SAMPLE_INDIVIDUAL&
ADC_SAMP_OFF;

//ADCON2 ���W�X�^
//ADC_VREF_EXT_EXT&
//ADC_SCAN_OFF&
unsigned int ValueADCON2 =
ADC_VREF_AVDD_AVSS&
ADC_SCAN_OFF&
ADC_CONVERT_CH_0ABC&
ADC_SAMPLES_PER_INT_1&
ADC_ALT_BUF_ON&
ADC_ALT_INPUT_OFF;

//ADCON3 ���W�X�^
//ADC_SAMPLE_TIME_4&
//ADC_CONV_CLK_SYSTEM&
//ADC_CONV_CLK_4Tcy,
unsigned int ValueADCON3 =
ADC_SAMPLE_TIME_1&
ADC_CONV_CLK_SYSTEM&
ADC_CONV_CLK_8Tcy;

//
//ENABLE_AN0_ANA&
//ENABLE_AN1_ANA&
//configport�ɂ�ADPCFG�p�̃p�����[�^���w��
unsigned int ValueADPCFG =
ENABLE_AN0_ANA&
ENABLE_AN1_ANA&
ENABLE_AN2_ANA&
ENABLE_AN3_ANA;

//SCAN_NONE;
// );
unsigned int ValueADCSSL = SCAN_NONE;

//	SetChanADC10(
unsigned int Channel0 =
ADC_CHX_POS_SAMPLEA_AN0AN1AN2& //CH1,2,3��MUXA�́{�w��  AN���ȏ�ڑ�����ꍇ�K�v
ADC_CHX_NEG_SAMPLEA_NVREF&     //CH1,2,3��muXA�́[�w��
ADC_CH0_POS_SAMPLEA_AN0&       //CH0�́{�w�� CH0��AN0������悤�ɂȂ����@�����ɐڑ�
ADC_CH0_NEG_SAMPLEA_NVREF;      //CH0�́[�w��



////PWM�̃p�����[�^�ݒ�
//PWM�����@1khz �Ɨ�PWM
#define FCY  3000000 //�N���b�N���g��
#define FPWM 1000    //PWM����

//void initOpenMCPWM(void){
//PWM��Config
//OpenMCPWM(
//0x02EE,

unsigned int Period = FCY / FPWM ; //3000
unsigned int config4 =
PWM_EN &  //PWM���W���[��ON
PWM_IDLE_CON & //�A�C�h������ON
PWM_OP_SCALE1 &  //�@���荞�݂̂��߂̏o�͐M���𕪎�����
PWM_IPCLK_SCALE1 & //���͂̃N���b�N��������
PWM_MOD_FREE; //�t���[�������[�h�@PWM�J�E���^����

unsigned int config5 =
PWM_MOD1_IND &  PWM_MOD2_IND &PWM_MOD3_IND &//�Ɨ������M���@����PWM���g�p���Ȃ�
PWM_PDIS1H & PWM_PEN1L & //BIN2 PWMB PEN PWM�o�́@PDIS�@PWM�@�\�Ȃ�
PWM_PDIS2H & PWM_PDIS2L & //AIN1 BIN1
PWM_PEN3H & PWM_PDIS3L; //PWMA AIN2
//PWM_SEVOPS1 &
unsigned int config6 =
PWM_SEVOPS1 &
PWM_OSYNC_PWM & //PWM�o�͂��ׂ����m�C�Y���܂ނ��Ƃ�h�����Ƃ��ł��܂��B 
PWM_UEN; //��������уf���[�e�B�T�C�N���̕ύX���󂯕t���� 




void __attribute__((__interrupt__, __shadow__)) _ADCInterrupt(void){
   	IFS0bits.ADIF = 0;
	LATDbits.LATD3 = !LATDbits.LATD3;
	  // valueM1 = 500; //ADtest�p
	  valueM1 = ReadADC10(0);
	  valueM2 = ReadADC10(3);
	//	valueM3 = ReadADC10(2);
	Flag = 1;
      } 



void __attribute__((__interrupt__)) _T2Interrupt(void){
       
    // duty = 1800;
   //  Duty = 1000;
   if(duty > 3000){
      duty = 3000;
    }else if(Duty > 3000){
      Duty = 3000;
    }
    TRISE = 0x0000;// LED�|�[�g(RE8)�����ݒ�(0)�B
    IFS0bits.T2IF = 0;
    IFS2bits.PWMIF = 0;
    SetDCMCPWM (1,duty,0);   // �p���X����ύX  (PIN,�p���X��,�ύX��
    SetDCMCPWM (3,Duty,0);  //�p���X����ύX  (PIN,�p���X��,�ύX��)
      duty1 = duty;
      Duty1 = Duty;
	Flag1 = 1;

} 




int main(void){

	//�|�[�g�̏����ݒ�
   // PORTE = 0; //RE���S�ĂO
	LATB = 0xFF;
	TRISD = 0x0000;// LED�|�[�g(RD3)�����ݒ�(0)�B
	TRISB = 0x01F;  //AN0 to AN4 input,other all put
	TRISF = 0;     //all output
	LATC = 0x1000; //
	LATDbits.LATD3 = 1;// RD3 LED�_���B
	Flag =  0;
    Flag1 = 0;
    duty =  0;
    duty1 = 0;
    Duty =  0;
    Duty1 = 0;
	//CRLF();

	//UART�����ݒ�
	OpenUART1(config3, config2, (unsigned int)(32000000 / (115200 * 16) - 1));
	ConfigIntUART1(UART_RX_INT_DIS & UART_TX_INT_DIS);
//	OpenUART2(config3, config2, (unsigned int)(32000000 / (115200 * 16) - 1));
//	ConfigIntUART2(UART_RX_INT_DIS & UART_TX_INT_DIS);
	//CRLF();

	//AD�R���o�[�^�[�����ݒ�
	OpenADC10(ValueADCON1, ValueADCON2, ValueADCON3, ValueADPCFG, ValueADCSSL);
	ConfigIntADC10(ADC_INT_ENABLE&ADC_INT_PRI_5);   //���荞�݋���
	SetChanADC10(Channel0);
	//SetChanADC10(ADC_CH0_POS_SAMPLEA_AN0&ADC_CH0_NEG_SAMPLEA_NVREF);

	//PWM�o�͓Ɨ����� ������]
	OpenMCPWM(Period,Period, config4, config5, config6);
//    ConfigIntMCPWM(PWM_INT_OFF&PWM_INT_PR_7);   //���荞�݋���
	SetDCMCPWM(1, 0, 0); 
    SetDCMCPWM(3,0,0);
	
    //M = 10��6�� (4/120MHz) �~256�~T = 0.0000085333T�b
	//�^�C�}�Q�̐ݒ�  (4/120MHz) �~256�~100 =0.0008533333�b 0.001
	OpenTimer2(T2_ON&T2_GATE_OFF&T2_PS_1_256&T2_SOURCE_INT, 100/*�ύX*/);
	ConfigIntTimer2(T2_INT_PRIOR_7&T2_INT_ON);
    
   	//�^�C�}�R�̐ݒ�  (4/120MHz) �~256�~10000 = 0.33333�b  
	OpenTimer3(T3_ON&T3_GATE_OFF&T3_PS_1_256&T3_SOURCE_INT, 25000/*�ύX*/);
	ConfigIntTimer3(T3_INT_PRIOR_6&T3_INT_OFF);

	//�����񑗐M
    unsigned  char asc[5];
             float T3 = 0.42666;
             float D1D2 = 1;
             float ro = (100/3.175)*60/(3.14*5);
    unsigned  int  valueD1 =0; 
    unsigned  int  valueD2 =0; 
    unsigned  int  valueDD =0; 
    unsigned  int  D1 = 0; 
    unsigned  int  D2 = 0;
    unsigned  int  D3 = 0; 
    unsigned  int  D4 = 0;
    unsigned  int  D =0;
    unsigned  int  Dx =0; 
    unsigned  int x1 = 6;
            float y1 =0;
            float y2 =0;
    unsigned  int deg =0;
           //   int Deg = 0;
            float vv =0;
            float VV =0;
            float vx =0;
            float Vx =0;
            float v[16];
            float V[16];
            float vw[16];
            float rad[4];
            float radx;
            float Rad =0;
            float Rad1 =0;
            float w = 0;
              int out = 0;
    unsigned  int r = 20;
    unsigned  int kp;
            float t;
    unsigned  int i = 0;
    unsigned  int i2 = 0;
    unsigned  int i3 = 0;
/*    unsigned  int a;  
    unsigned  int b;
    unsigned  int c; 
    unsigned  int d;
    unsigned  int e; 
    unsigned  int f;
    unsigned  int g;  
    unsigned  int h;
    unsigned  int i; 
    unsigned  int j;
    unsigned  int k; 
    unsigned  int l;
    unsigned  int m; 
    unsigned  int n;
    unsigned  int o; 
    unsigned  int p; 
//  temp = ReadUART1(); //��M������temp�֕ۑ�    ��M�o�b�t�@�̃f�[�^�ǂݏo��
    //   while(BusyUART1());
    */
     LATEbits.LATE2 = 1; //BIN1 = 1 
     LATEbits.LATE1 = 0; //BIN2 0V�𗬂��K�v�����邽��LATE
     LATEbits.LATE0 = 0; //PWMB 
     LATEbits.LATE3 = 0; //AIN1 = 1
     LATEbits.LATE4 = 1; //AIN2 = 0 
     LATEbits.LATE5 = 0; //PWMA  
     LATFbits.LATF0 = 1; //STBY 

      while(1){
        if(Flag){
         Flag = 0;
         valueD1 = 4800/(valueM1 -  20);
		 D2 = D1;
         D1 = valueD1;
         D = D1 - D2;
         v[0] = D/T3;  //���bv����

         valueD2 = 4800/(valueM2 - 20);
		 D4 = D3;
         D3 = valueD2;
         Dx = D3 - D4;
         if(D > 6){
         valueD1 = valueD2;
         D = Dx;
         }else if(Dx > 6){
         valueD2 = valueD1;
         Dx = D;
         }
         v[0] = D/T3;  //���bv����
         V[0] = Dx/T3;  //���bv����
         y1 = valueD1 - valueD2;
         valueDD = (valueD1 + valueD2)/2;

         for(i = 16; i < 1; i--){
               v[i] = v[i-1];
         }
         vv = v[1] - v[2];
         vx =v[1]  + vv; 
         for(i = 16; i < 1; i--){
               V[i] = v[i-1];
         }
         VV = V[1] - V[2];
         Vx = V[1] + VV;  
         
         

   if(y1 > D1D2){
   i2++;
   rad[0] = atan2f(y1,x1);
   rad[2] = rad[1];
   rad[1] = rad[0];
   Rad = rad[1] - rad[2];
   Rad1 = Rad - radx; 
   deg = rad[0]*180/3.14;
   if(i2 > 1 && i2 < i){
   w = Rad1/T3;  //  w = 2*3.14;
   }else if(i2 > i){
    w =Rad /T3;
   }
   vw[0] = r*w;  //r�͑O�҂̋Ȃ钷��
   for (i = 16; i < 1; i--){
         vw[i] = vw[i-1];
   }
   if(i2 == 1){
   t = valueD2/Vx;   //1.6 
   i = (t/T3 + 1)/2 ;  //5 
   radx = rad[1]; 
    }
       //kp =20;
  // out = kp*(valueDD - 10);
   duty = duty1 + ro*v[i]  + ro*vw[i];
   Duty = Duty1 + ro*V[i] ;
     Flag = 0;

}else if(y1 >= -D1D2 && y1 <= D1D2){
   i = 0;
   i2 = 0;
   i3 = 0; 
   w = 0;
   rad[0] = 0;
   rad[1] = 0;
   rad[2] = 0;
   Rad = 0;
   vw[0] = 0;
   v[1] = 0;
   V[1] = 0;
   for(i = 16; i < 1; i--){
         vw[i] = vw[i-1];
   }
   duty = duty1 + ro*v[0] ;
   Duty = Duty1 + ro*V[0] ;
      Flag = 0;
}else if(y1 < D1D2 ){
   i3++;
   y2 = -y1;
   rad[0] = atan2f(y2,x1);
   rad[2] = rad[1];
   rad[1] = rad[0];
   Rad = rad[1] - rad[2];
   Rad1 = Rad - radx; 
   deg = rad[0]*180/3.14;
   if( i3 > 1 && i3 < i){
   w = rad[1]/T3;  //  w = 2*3.14;
   }else if(i3 > i){
    w = rad[1] /T3;
   }
   vw[0] = r*w;  //r�͑O�҂̋Ȃ钷��
   for(i = 16; i < 1; i--){
         vw[i] = vw[i-1];
   }
   if(i3 == 1){
   t = valueD2/Vx;   //1.6 
   i =  (t/T3 + 1)/2 ;  //5 
   radx = rad[1]; 
  }
 //  kp = 20;
 //  out = kp*(valueDD - 10);
  duty = duty1 + ro*v[i]  ;
  Duty = Duty1 + ro*V[i] + ro*vw[i];
   }else{
   i = 0;
   i2 = 0;
   i3 = 0; 
   w = 0;
   v[1] = 0;
   V[1] = 0;
   vw[0] = 0;
   duty = 0;
   Duty = 0;
   Flag = 0;
}
 if( valueDD <12 ){
   i = 0;
   i2 = 0;
   i3 = 0; 
   w = 0;
   rad[0] = 0;
   rad[1] = 0;
   rad[2] = 0;
   Rad = 0;
   vw[0] = 0;
   v[1] = 0;
   V[1] = 0;
   for(i = 16; i < 1; i--){
         vw[i] = vw[i-1];
   }
  duty = 0;
  Duty = 0;
 }




/*         a = valueD1 / 10;
         b = valueD1 % 10;   
         c = valueD2 / 10;
         d = valueD2 % 10; 
         e = v1 / 10;
         f = v1 % 10;
         g = v2 / 10;
         h = v2 % 10;   
         i = deg / 10;
         j = deg % 10; 
         k = v3 / 10;
         l = v3 % 10;         
         m = Vx / 10;
         n = Vx % 10;
         o = t / 10;
         p = t % 10; 
        asc[0] = (char)(a+48);
        asc[1] = (char)(b+48);
        asc[2] = (char)(c+48);
        asc[3] = (char)(d+48);
	    asc[4] = (char)('\n');
        asc[5] = (char)(e+48);
        asc[6] = (char)(f+48);
        asc[7] = (char)(g+48);
        asc[8] = (char)(h+48);
        asc[9] = (char)(i+48);
        asc[10] = (char)(j+48);
        asc[11] = (char)(k+48);
        asc[12] = (char)(l+48);
        asc[13] = (char)(m+48);
        asc[14] = (char)(n+48);
        asc[15] = (char)(o+48);
        asc[16] = (char)(p+48); 
		asc[4] = '\0';  //�I�������@�Ō�ɂ�NULL��������͂��Ȃ��Ƒ��M���ʂ��ł�
   //     	newline();
           //�����񑗐M
            putsUART1(asc);
		 	while (BusyUART1());
			newline();
*/
     }
 if(Flag1){
      Flag1 = 0;
}
}
}
/*
void newline(void){

	putsUART1("\n\r");

	while (BusyUART1());
}*/
