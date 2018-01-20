/*
 * File:   serialtemiz.c
 * Author: Monster
 *
 * Created on 13 Ocak 2018 Cumartesi, 16:01
 */


#include <xc.h>
#include "serialtemiz.h"
#include<stdint.h>


#define BUFFSIZE    25

void OscTune(void);
void SerialInit(void);
void led_init(void);
void timer_init(void);
char return_n_before(char[],int,unsigned int);

char buff;
char command[BUFFSIZE];
int i=-1;
int last_i;
int k;
int print_value=0;

uint32_t timeTick=0;
uint32_t timeTick_exact=0;
struct Serial_Ops{
    char command[BUFFSIZE];
    int wr_index;
    int rd_index;
};
struct Serial_Ops com7;

void main(void) {
    timer_init();
    OscTune();
    SerialInit();
    led_init();
    
    
    com7.wr_index=com7.rd_index;
    
    while(!TXSTA2bits.TRMT);
    TXREG2='u';
    
    while(1){
 
        if(com7.wr_index != com7.rd_index){
            
            if(com7.command[com7.rd_index]=='1'){
                while(!TXSTA2bits.TRMT);
                TXREG2='y';
                if(return_n_before(com7.command,com7.rd_index,1)=='c' && return_n_before(com7.command,com7.rd_index,2)=='a'){
                    LATCbits.LATC2=1;
                    while(!TXSTA2bits.TRMT);
                    TXREG2='1';
                }
                
            
                if(return_n_before(com7.command,com7.rd_index,1)=='p' && return_n_before(com7.command,com7.rd_index,2)=='k' ){
                    LATCbits.LATC2=0;
                    while(!TXSTA2bits.TRMT);
                    TXREG2='2';
                }
                    
            }  
    
            
            if(com7.command[com7.rd_index]=='2'){
                if(return_n_before(com7.command,com7.rd_index,1)=='c' && return_n_before(com7.command,com7.rd_index,2)=='a'){
                    LATCbits.LATC3=1;
                    while(!TXSTA2bits.TRMT);
                    TXREG2='3';
                }
                
                if(return_n_before(com7.command,com7.rd_index,1)=='p' && return_n_before(com7.command,com7.rd_index,2)=='k'){
                    LATCbits.LATC3=0;
                    while(!TXSTA2bits.TRMT);
                    TXREG2='4';
                }
                    
            }
            
            if(com7.command[com7.rd_index]=='*'){
                if(return_n_before(com7.command,com7.rd_index,1)=='c' && return_n_before(com7.command,com7.rd_index,2)=='a'){
                    PORTC=0xFF;
                    while(!TXSTA2bits.TRMT);
                    TXREG2='5';
                }
                
                if(return_n_before(com7.command,com7.rd_index,1)=='p' && return_n_before(com7.command,com7.rd_index,2)=='k'){
                    PORTC=0x00;
                    while(!TXSTA2bits.TRMT);
                    TXREG2='6';
                }
            }
            
            if(com7.command[com7.rd_index]=='3'){
                if(return_n_before(com7.command,com7.rd_index,1)=='c' && return_n_before(com7.command,com7.rd_index,2)=='a'){
                    LATCbits.LATC4=1;
                    while(!TXSTA2bits.TRMT);
                    TXREG2='3';
                }
                
                if(return_n_before(com7.command,com7.rd_index,1)=='p' && return_n_before(com7.command,com7.rd_index,2)=='k'){
                    LATCbits.LATC4=0;
                    while(!TXSTA2bits.TRMT);
                    TXREG2='4';
                }
                    
            }

            com7.rd_index++;
            
            if(com7.rd_index>=BUFFSIZE)
                com7.rd_index=0;
            
                
        }
    
        /*while(!TXSTA2bits.TRMT);
        TXREG2=command[i];*/
        
        if(timeTick>=40000){
            while(!TXSTA2bits.TRMT);
            TXREG2='r';
            timeTick=0;
        }
    }
    
    return;
}

void OscTune(){
    OSCCONbits.IRCF     = 0b111; //freq
    OSCCONbits.SCS      = 0b00; //
    OSCCON2bits.PLLRDY  = 0b1;
    OSCCON2bits.SOSCRUN = 0b0;
    OSCTUNEbits.PLLEN   = 0b1;

}

void SerialInit(){
    ANSELB = 0x00;
    TRISBbits.RB6=1;
    TRISBbits.RB7=1;
     
    RCSTA2bits.CREN=1;
    TXSTA2bits.SYNC=0;
    RCSTA2bits.SPEN=1;
    
    BAUDCON2 = 0x08;//0x38;
    SPBRG2 = 0x82;//BaudRate 9600 64Mhz Kristal icin 0x0682
    SPBRGH2 = 0x06;//0x03;//0;
    TXSTA2 = 0x24;
    RCSTA2 = 0x90;
    
    INTCONbits.GIE=1;
    INTCONbits.TMR0IE=1;
    INTCONbits.TMR0IF=0;
    
    PIE3bits.RC2IE=1;
    INTCONbits.PEIE=1;
    INTCONbits.GIE=1;
    
}

void led_init(){
    TRISC=0x00;   
    //PORTC=0x00;
  
}

void timer_init(){
    T0CONbits.TMR0ON = 1;
    T0CONbits.T08BIT=1;
    T0CONbits.T0CS=0;
    T0CONbits.T0SE = 0;
    T0CONbits.PSA=0; //1 for not assigned
    T0CONbits.T0PS=0b011;
    TMR0L = 156; 
}

void interrupt receive(void){
    if(PIR3bits.RC2IF){
        com7.command[com7.wr_index]=RCREG2;  
        com7.wr_index++;
        if(com7.wr_index>=BUFFSIZE)
            com7.wr_index=0;
    }
    
    if (INTCONbits.TMR0IE && INTCONbits.TMR0IF) {
        INTCONbits.TMR0IF=0; 
        timeTick++;
        TMR0L=156;
        /*if(timeTick>=4000){
            timeTick_exact++;
            timeTick=0;
            //LATCbits.LATC2^=1;    
        }
        if(timeTick_exact>=155){
            print_value=1;
            timeTick_exact=0;
            
        }*/
    }
    
    
}

char return_n_before(char array[],int position,unsigned int n){
    while(n>0){
        if(position==0){
            position=sizeof(array)/sizeof(array[0]);
        }else{
            position--;
        }
        n--;
    }
    return array[position];
}