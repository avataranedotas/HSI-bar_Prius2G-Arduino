

/* Projecto Barra HSI para o Prius 2G
 Requere o CAN-bus shield para o Arduino.
 Os dados são mostrados no LCD Serial
  
 Baseado no Sketch original de SK Pang Electronics www.skpang.co.uk v3.0 21-02-11  
 */

#include <SoftwareSerial.h>
#include <mcp2515.h>


//definições do lcd série (lcd's serial da SparkFun v2.5)
SoftwareSerial sLCD(3, 6); // RX, TX
#define COMMAND 0xFE //caracter especial para enviar comandos para o LCD serial
#define CLEAR   0x01 //caracter para limpar o écran do LCD
#define LINE0   0x80 //caracter para escolher a posição do cursor no início da primeira linha
#define LINE1   0xC0 //caracter para escolher a posição do cursor no início da segunda linha
#define DIM     0x7C //caracter especial para escolher a luminosidade do écran e outras funções


//velocidades CAN Bus
#define CAN_125 	7		// Velocidade CAN a 125 kbps
#define CAN_250  	3		// Velocidade CAN a 250 kbps
#define CAN_500		1		// Velocidade CAN a 500 kbps


/* Pinos onde está ligado o joystick (do CAN-Bus shield) */
#define UP     A1
#define RIGHT  A2
#define DOWN   A3
#define CLICK  A4
#define LEFT   A5



#define LED2 8    //pinos para os leds 2 e 3
#define LED3 7
#define FALTA 10

char buffer[32];  //Buffer de dados temporários para escrever no LCD
char esc[11];     //Para escrever a barra E
int dados[10];     //para receber dados das funções

int acel;  //posição do acelerador 0-100%

int acel2;  //posição do acelerador 0-200
int travao2; //posição do travão 0-100%
int rpmrequisitadas2; //rpm 0-4500 rpm
int cbat2; //corrente da bateria   -125 a 125A
int temp_agua2; //temperatura da água de refrigeração 0 - 200 C
int velo2; // velocidade em kmh 0 a 176
int mud2; //mudança 
int soc2; //carga da bateria 0-100%
int modev2; //modo EV
int lum2; //luminosidade painel
int lum_old = 10;

//variáveis para a barra
int R=0;
int E=0;
int P=0;


//pontos variáveis iniciais da barra
int pontoA = 20;
int pontoB = 35;
int pontoC = 65;


boolean flagE4 = false;
boolean flagE5 = false;
boolean flagE6 = false;


boolean message_ok0 = false;
boolean message_ok1 = false;
boolean message_ok2 = false;
boolean message_ok3 = false;
boolean message_ok4 = false;
boolean message_ok5 = false;
boolean message_ok6 = false;
boolean message_ok7 = false;
boolean message_ok8 = false;
boolean message_ok9 = false;


//variáveis para a barra anteriores à actual 
int RA=0;
int EA=0;
int PA=0;



//variáveis para fazer contas ao ler da ECU
int valor;
long valor2;
float flut;
int timeout = 0;
tCAN message;

// programação dos caracteres especiais para construir a barra

byte E0[8] = {   
  B00011111, B00010000, B00010000, B00010000, B00010000, B00010000, B00010000, B00011111           };

byte E1[8] = {     
  B00011111, B00011000, B00011000, B00011000, B00011000, B00011000, B00011000, B00011111           };

byte E2[8] = { 
  B00011111, B00011100, B00011100, B00011100, B00011100, B00011100, B00011100, B00011111           };

byte E3[8] = { 
  B00011111, B00011110, B00011110, B00011110, B00011110, B00011110, B00011110, B00011111           };

byte E4[8] = { 
  B00011111, B00011111, B00011111, B00011111, B00011111, B00011111, B00011111, B00011111           };

byte E5[8] = { 
  B00011111, B00000001, B00000001, B00000001, B00000001, B00000001, B00000001, B00011111           };

byte E6[8] = {     
  B00011111, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00011111           };

byte E7[8] = { 
  B00011111, B00010001, B00010001, B00010001, B00010001, B00010001, B00010001, B00011111           };

byte E8[8] = { 
  B00011111, B00011001, B00011001, B00011001, B00011001, B00011001, B00011001, B00011111           };

byte E9[8] = { 
  B00011111, B00011101, B00011101, B00011101, B00011101, B00011101, B00011101, B00011111           };



byte R0[8] = { 
  B00000000, B00000000, B00000000, B00011111, B00000000, B00000000, B00000000, B00011111           };

byte R1[8] = { 
  B00000000, B00000000, B00000000, B00011111, B00000001, B00000001, B00000001, B00011111           };

byte R2[8] = { 
  B00000000, B00000000, B00000000, B00011111, B00000011, B00000011, B00000011, B00011111           };

byte R3[8] = { 
  B00000000, B00000000, B00000000, B00011111, B00000111, B00000111, B00000111, B00011111           };

byte R4[8] = { 
  B00000000, B00000000, B00000000, B00011111, B00001111, B00001111, B00001111, B00011111           };

byte R5[8] = { 
  B00000000, B00000000, B00000000, B00011111, B00011111, B00011111, B00011111, B00011111           };

byte R6[8] = { 
  B00000000, B00000000, B00000000, B00011111, B00010001, B00010001, B00010001, B00011111           };

byte R7[8] = { 
  B00000000, B00000000, B00000000, B00011111, B00010011, B00010011, B00010011, B00011111           };

byte R8[8] = { 
  B00000000, B00000000, B00000000, B00011111, B00010111, B00010111, B00010111, B00011111           };

byte R9[8] = { 
  B00000000, B00000000, B00000000, B00011111, B00010000, B00010000, B00010000, B00011111           };

byte P0[8] = { 
  B00011111, B00000000, B00000000, B00000000, B00011111, B00000000, B00000000, B00000000           };

byte P1[8] = { 
  B00011111, B00010000, B00010000, B00010000, B00011111, B00000000, B00000000, B00000000           };

byte P2[8] = { 
  B00011111, B00011000, B00011000, B00011000, B00011111, B00000000, B00000000, B00000000           };

byte P3[8] = { 
  B00011111, B00011100, B00011100, B00011100, B00011111, B00000000, B00000000, B00000000           };

byte P4[8] = { 
  B00011111, B00011110, B00011110, B00011110, B00011111, B00000000, B00000000, B00000000           };

byte P5[8] = { 
  B00011111, B00011111, B00011111, B00011111, B00011111, B00000000, B00000000, B00000000           };

byte P6[8] = { 
  B00011111, B00000001, B00000001, B00000001, B00011111, B00000000, B00000000, B00000000           };

byte P7[8] = { 
  B00011111, B00010001, B00010001, B00010001, B00011111, B00000000, B00000000, B00000000           };

byte P8[8] = { 
  B00011111, B00011001, B00011001, B00011001, B00011111, B00000000, B00000000, B00000000           };

byte P9[8] = { 
  B00011111, B00011101, B00011101, B00011101, B00011111, B00000000, B00000000, B00000000           };


//temporizadores

unsigned long time1=0;
//unsigned long time2=0;
//unsigned long time3=0;
//unsigned long time4=0;

int count=0;       //Variável geral
int count2=0;       //Variável geral
int flag=0;


void setup() {
  pinMode(LED2, OUTPUT);     //LEDs 2 e 3 do shield CANbus
  pinMode(LED3, OUTPUT); 

  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);

  pinMode(UP,INPUT);         //pinos do joystick
  pinMode(DOWN,INPUT);
  pinMode(LEFT,INPUT);
  pinMode(RIGHT,INPUT);
  pinMode(CLICK,INPUT);

  digitalWrite(UP, HIGH);       /* ligar os pull-ups internos nas entradas do joystick */
  digitalWrite(DOWN, HIGH);
  digitalWrite(LEFT, HIGH);
  digitalWrite(RIGHT, HIGH);
  digitalWrite(CLICK, HIGH);


  //início


  sLCD.begin(9600);              //inicia a comunicação com o LCD

  sLCD.write(0x12);        //faz reset para 9600bps

  delay (2000);

  sLCD.write(DIM);        //escolhe a velocidade 19200bps
  sLCD.write(0x0F);


  sLCD.begin(19200);              //inicia a comunicação a 19200

    delay (100);

  clear_lcd();

  //Ajuste inicial da Luminosidade do LCD para 100%
  sLCD.write(DIM);
  sLCD.write(0x9D);
  delay(100);

  //mensagem de boas vindas
  clear_lcd();

  sLCD.write(COMMAND);                   
  sLCD.write(LINE1);
  sLCD.print("   HSI Ver.03   ");
  //         |                |

  sLCD.write(COMMAND);                   
  sLCD.write(LINE0);
  sLCD.print("   BEM-VIND@    ");
  //         |                |


  delay (1000);


  if(inic(CAN_500))  // Inicializa o controlador CAN MCP2515 à velocidade escolhida, 500 para o Prius
  {
    sLCD.write(COMMAND);                   
    sLCD.write(LINE0);
    sLCD.print(" * * Pronto * * ");
    sLCD.write(COMMAND);                   
    sLCD.write(LINE1);
    sLCD.print("Ligue a viatura!");
    //         |                |
  } 
  else
  {
    sLCD.write(COMMAND);                   
    sLCD.write(LINE0);
    sLCD.print("    ERRO NA     ");
    sLCD.write(COMMAND);                   
    sLCD.write(LINE1);
    sLCD.print(" INICIALIZACAO  ");
    delay (10000);
    //         |                |
  } 

  delay(1000); 

  //programa os caracteres especiais iniciais 

  prog_car(0,R9);
  prog_car(1,R0);
  prog_car(2,R2);
  prog_car(3,E6);
  prog_car(4,E5);
  prog_car(5,E0);
  prog_car(6,P0);
  prog_car(7,P6);

  clear_lcd();

  //desenha a barra inicial vazia


  sLCD.write(COMMAND);                   
  sLCD.write(LINE0);
  
  sLCD.write((byte)0x0);
  sLCD.write(1);
  sLCD.write(1);
  sLCD.write(5);
  sLCD.write(3);
  sLCD.write(3);
  sLCD.write(3);
  sLCD.write(3);
  sLCD.write(4);
  sLCD.write(3);
  sLCD.write(3);
  sLCD.write(3);
  sLCD.write(3);
  sLCD.write(4);
  sLCD.write(6);
  sLCD.write(7);


}



void loop() 
{



  //ler os dados do BUS CAN
  ecu_3(dados);

  //modo EV
  if (dados[0] !=9999) modev2=dados[0]; 

  if ( dados[1] !=9999)  //temperatura

  {
    temp_agua2 = dados[1];
    sLCD.write(COMMAND);
    sLCD.write(LINE1 + 6);                     // Move o cursor do LCD 
    sprintf(buffer,"%3dC",(int) temp_agua2);
    sLCD.print(buffer);
  }


  if ( dados[2] !=9999)  //dimmer

  {
    lum2 = dados[2];
    if (lum2 != lum_old && flag==0) //se houver alterações e não estiver em hibernação ajusta a luminosidade
    {
      if (lum2 == 24)
      {
        //Ajusta a Luminosidade do LCD  min = 80
        sLCD.write(DIM);
        sLCD.write(0x84);
        delay (200);
      }

      else

      {
        //Ajusta a Luminosidade do LCD max = 9D
        sLCD.write(DIM);
        sLCD.write(0x9D);
        delay(200);
      }
      lum_old = lum2;  //actualiza o valor actual
      EA=63;           //força desenhar a barra ao acordar da hibernação
      PA=10;
      RA=16;
    }
  }

  

  if ( millis() - time1 >= 10000)   //verificar se não está desligado de 10 em 10 segundos
  { 
    count2 = 0;
    for (count=0;count<=(FALTA-1);count++)
    {
      if (dados[count]==9999) count2=count2+1;
    }

    if (count2==FALTA)    //se estiver desligado desligar a iluminação do LCD
    {
      lum_old = 10;  // **************************************************************
      lum2=10;

      if (flag == 0)  //se ainda estiver ligado avisa e desliga
      {
        clear_lcd();
        sLCD.write(COMMAND);                   
        sLCD.write(LINE0);
        sLCD.print("Falta de sinais ");
        sLCD.write(COMMAND);                   
        sLCD.write(LINE1);
        sLCD.print(" Hibernando...  ");
        //         |                |
        sLCD.write(DIM);
        sLCD.write(0x80);  //desliga a iluminação
        delay (5000);
        clear_lcd();
        flag = 1;    //actualiza a flag de hibernação
      }
    }
    else
    {
      flag=0;
    }

    time1=millis();
  }



  //RPM

  if (dados[3] != 9999)          // Ler as RPM 
  {
    rpmrequisitadas2 = dados[3];   
    sLCD.write(COMMAND);                   // Move o cursor LCD 
    sLCD.write(LINE1+11);
    if (modev2 == 64) sLCD.print("   EV");    //se estiver em modo EV escreve EV
    else {
      sprintf(buffer,"%4d",(int) rpmrequisitadas2);
      sLCD.print(buffer);
      if (rpmrequisitadas2 < 970) sLCD.print("E");  //se o MCI não estiver a trabalhar mostra E em vez de R
      else sLCD.print("R");
    }
  }

  //corrente da bateria


  if(dados[4] != 9999) //ler a bateria
  {
    cbat2 = dados[4];
    sLCD.write(COMMAND);
    sLCD.write(LINE1);                     // Move o cursor do LCD 
    sprintf(buffer,"%+4dA",(int) cbat2);  
    sLCD.print(buffer);
  }


  //acelerador

    if (dados[5] !=9999) acel2 = dados[5];



  //travão

  if (dados[6] !=9999) travao2 = dados[6];



  //ler a velocidade

  if( dados[7] != 9999)
  {
    velo2 = dados[7];
    if (mud2 == 33) velo2 = velo2 * -1;   //se estiver metida a marcha atrás a velocidade passa a negativa
  }



  //mudança

  if (dados[8] !=9999) mud2 = dados[8];


  // SOC

  if (dados[9] !=9999) soc2 = dados[9];




  //construção da barra

  //mudanças D,B e R   em N e P a barra não faz nada
  // P=32 R=33 N=34 D=35 B=36



  if (  ((mud2 >= 35) || (mud2 == 33))   )

  { 
    // P=32 R=33 N=34 D=35 B=36

    //Sem nenhum pé nos pedais, aceleração 0 e travao 0
    if ((acel2 == 0) && (travao2 ==0))
    {
      // mudanças D e B
      if (mud2 != 33)
      {
        if (velo2<9)  //velocidades baixas, o carro acelera ligeiramente
        { 
          E=1; 
          P=0; 
          R=0;
        } 

        if (velo2 >= 9 && velo2<12)  //velocidade entre 9 e 11 km/h: não acelera nem regenera
        { 
          E=0; 
          P=0; 
          R=0;
        } 

        if ((velo2 >= 12) && (mud2 == 35))  //mudança D e velocidade maior que 12km/h: regenera um pouco
        { 
          E=0; 
          P=0; 
          R=1;
        }

        if ((velo2 >= 12) && (mud2 == 36))  //mudança B e velocidade maior que 12km/h: regenera mais um pouco
        { 
          E=0; 
          P=0; 
          R=2;
        }   
      }

      // mudança R em qualquer velocidade acelera 2 traços
      if (mud2 == 33) 
      { 
        E=2; 
        P=0; 
        R=0;
      }  
    }

    //Pé apenas no travão - independente da mudança 
    if ((acel2 == 0) && (travao2 >0))
    {

      if (velo2 < 5)  //marcha atrás ou até 5 km/h nao regenera, seja D ou B
      {
        P=0;

        if (travao2 >= 14)  //com o pé forte no travão não acelera nem regenera
        { 
          E=0; 
          R=0;
        } 

        if (travao2 <  14) //com o pé ligeiramente no travão acelera 1 barra
        { 
          E=1; 
          R=0;
        }
      }

      if (velo2 >= 5 && velo2 <=16)  //velocidade entre 5 e 16 km/h, pouca diferença entre D e B
      {
        E=0;
        P=0;
        if (travao2 >16)    R=(velo2 -5) *16 /11 ;                 //se travao superior a 16 enche conforme a velocidade a partir de 5
        if (travao2 <=16)   R=(velo2 -5) *travao2 /11 ;            //se travao menor enche conforme força no travao e velocidade
      }

      if (velo2 >= 16)   //velocidade superior a 16km/h
      {
        E=0;
        P=0;
        if (travao2 >16)  R=16;                //se travao superior a 16 enche a barra toda
        if (travao2 <=16) R=travao2;           //se travao menor enche conforme a força no travão a partir do zero
      }

    }

    //Pé apenas no acelerador

    //mudança R: começa em 2 e acaba em 63 com 50% do acelerador

    //ponto A, E=33 é onde termina o modo eléctrico
    //ponto B, E=63 para as 2200rpm 
    //ponto C, modo PWR cheio

    if ((acel2 > 0) && (travao2 == 0))
    {
      R=0;

      //cálculo do ponto A
      if (velo2<50)
      {
        if (soc2<45) pontoA=5;
        if (soc2>=45 && soc2 <=56) pontoA=5+ ((soc2-45) *15 /11);
        if (soc2>=57 && soc2 <67)  pontoA = 20;
        if (soc2>=67 && soc2 <77)  pontoA = 20 + (soc2-67);
        if (soc2>=77) pontoA = 30;
      }

      if (velo2>=50 && velo2<63)
      {
        if (soc2<45) pontoA=5;
        if (soc2>=45 && soc2 <=56) pontoA=  5+  (   (soc2-45)*(    136-(velo2-50)    *75  /10) /100);
        if (soc2>=57 && soc2 <67 ) pontoA = 5+  ((11)*(136-(velo2-50)*75 /10) /100);
        if (soc2>=67 && soc2 <77 ) pontoA = (19+ (soc2-66))*(26-(velo2-50))/26;
        if (soc2>=77) pontoA = 15+ ((11)*(136-(velo2-50)*105/10) /100);
      }

      if (velo2>=63)
      {
        if (soc2<45) pontoA=5;
        if (soc2>=45 && soc2 <=56) pontoA=5+ ((soc2-45) *5 /11);
        if (soc2>=57 && soc2 <67 ) pontoA = 10;
        if (soc2>=67 && soc2 <77 ) pontoA = 10+((soc2-67)/2);
        if (soc2>=77) pontoA = 15;
      }

      //cálculo do ponto B
      pontoB= 50 - ((211 * velo2)/1000);


      //ponto C de origem vem 65 (PWR cheio) 

      //enchimento em modo R ou em modo EV
      if (mud2 == 33 || modev2 == 64) 
      { 
        pontoA=25;
        pontoB=40;
      }  



      //enchimento da barra
      // P=32 R=33 N=34 D=35 B=36

      acel = acel2/2;

      if (acel<=pontoA)
      {
        P=0;
        E= (acel2 *33 /pontoA /2); 
      }

      if (acel>pontoA && acel<pontoB)
      {
        P=0;
        E= ((acel2-(pontoA*2))*15)/(pontoB-pontoA) +33;
      }

      if (acel>=pontoB && acel<pontoC)
      {
        E=63;
        P= ((acel2-(pontoB*2))*5)   /    (pontoC-pontoB);
      }

      if (acel>=pontoC)
      {
        E=63;
        P=10;
      }


      //soma 1 para velocidades menores que 9
      if (velo2<9 && E<5) E=E+1;



    }



    //Pés no acelerador e no travão ao mesmo tempo
    if ((acel2 > 0) && (travao2 > 0))
    {
      E=0;
      R=0;
      P=10;
    }

  }

  else  //outros casos mostra a barra vazia

  {

    E=0;
    P=0;
    R=0;
  }
  //se houver mudanças em relação à situação anterior desenha as várias partes da barra

  if (R!=RA) barra_R(R);
  if (E!=EA) barra_E(E);
  if (P!=PA) barra_P(P);
}

// ------------------------------------------funções----------------------------------------------

//função para limpar o ecran
void clear_lcd(void)
{
  sLCD.write(COMMAND);
  sLCD.write(CLEAR);
}  


//função para programar caracteres especiais
//recebe um inteiro de 0 a 7 que representa o número ascii e uma string de bytes com o desenho

void prog_car(int x, byte *desenho)
{
  int i=0;
  for(i = 0; i < 8; i++)

  {
    sLCD.write(COMMAND); // Send command 
    sLCD.write(0x40 | (x << 3) | i); // Set the CGRAM address 
    sLCD.write(desenho[i]); // Set the character 
  } 


}

//---------------------------------------------------------------------------------------------------

//função que recebe um valor de R e Ra, programa os caracteres especiais e imprime a string da zona CHG


void barra_R (int r)

{
  char devolve[3];

  // se R=0 então a barra está vazia e tem de usar só 2 caracteres (0 e 1) para libertar os restantes para as outras zonas

  // se R != 0 então usa os 3 caracteres, 0, 1 e 2


  if (r==0 && RA!=0)  //se for a primeira vez que chega a 0 escreve no LCD, se for repetido não faz nada

  {
    //R9 R0 R0 E0 E6 E6 E6 E6 E5 E6 E6 E6 E6 E5 P0 P6
    prog_car(0,R9);
    prog_car(1,R0);

    devolve[0] = 0;
    devolve[1] = 1;
    devolve[2] = 1;

    //imprime os 3 caracteres

    sLCD.write(COMMAND);                   
    sLCD.write(LINE0);
    for (count=0; count<=2; count++)  
    {
      sLCD.write(devolve[count]);
    }

  }


  if (r>=1 && r<=6)

  {
    if (RA==0)
    {
      switch (r)
      {
      case 1:
        prog_car(2,R1); 
        break;
      case 2:
        prog_car(2,R2); 
        break;
      case 3:
        prog_car(2,R3); 
        break;
      case 4:
        prog_car(2,R4); 
        break;
      case 5:
        prog_car(2,R5); 
        break;
      case 6:
        prog_car(2,R5); 
        break;
      }
      devolve[2] = 2;
      sLCD.write(COMMAND);                   
      sLCD.write(LINE0 + 2);
      sLCD.write(devolve[2]);
    }

    if (RA>=1 && RA<=6)
    {
      switch (r)
      {
      case 1:
        prog_car(2,R1); 
        break;
      case 2:
        prog_car(2,R2); 
        break;
      case 3:
        prog_car(2,R3); 
        break;
      case 4:
        prog_car(2,R4); 
        break;
      case 5:
        prog_car(2,R5); 
        break;
      case 6:
        prog_car(2,R5); 
        break;
      }

    }


    if (RA>=7 && RA<=12)
    {
      switch (r)
      {
      case 1:
        prog_car(2,R1);
        prog_car(1,R0);
        break;
      case 2:
        prog_car(2,R2); 
        prog_car(1,R0);
        break;
      case 3:
        prog_car(2,R3); 
        prog_car(1,R0);
        break;
      case 4:
        prog_car(2,R4);
        prog_car(1,R0); 
        break;
      case 5:
        prog_car(2,R5); 
        prog_car(1,R0);
        break;
      case 6:
        prog_car(2,R5); 
        prog_car(1,R0);
        break;
      }

    }



    if (RA>=13 && RA<=16)
    {
      switch (r)
      {
      case 1:
        prog_car(2,R1);
        prog_car(1,R0);
        prog_car(0,R9);
        break;
      case 2:
        prog_car(2,R2); 
        prog_car(1,R0);
        prog_car(0,R9);
        break;
      case 3:
        prog_car(2,R3); 
        prog_car(1,R0);
        prog_car(0,R9);
        break;
      case 4:
        prog_car(2,R4);
        prog_car(1,R0); 
        prog_car(0,R9);
        break;
      case 5:
        prog_car(2,R5); 
        prog_car(1,R0);
        prog_car(0,R9);
        break;
      case 6:
        prog_car(2,R5); 
        prog_car(1,R0);
        prog_car(0,R9);
        break;
      }

    }



  }



  if (r>=7 && r<=12)

  {
    if (RA==0)
    {
      switch (r)
      {
      case 7:
        prog_car(2,R5); 
        prog_car(1,R1);
        break;
      case 8:
        prog_car(2,R5); 
        prog_car(1,R2); 
        break;
      case 9:
        prog_car(2,R5); 
        prog_car(1,R3); 
        break;
      case 10:
        prog_car(2,R5); 
        prog_car(1,R4); 
        break;
      case 11:
        prog_car(2,R5); 
        prog_car(1,R5); 
        break;
      case 12:
        prog_car(2,R5); 
        prog_car(1,R5); 
        break;
      }
      devolve[2] = 2;
      sLCD.write(COMMAND);                   
      sLCD.write(LINE0 + 2);
      sLCD.write(devolve[2]);
    }

    if (RA>=1 && RA<=6)
    {
      switch (r)
      {
      case 7:
        prog_car(2,R5); 
        prog_car(1,R1); 
        break;
      case 8:
        prog_car(2,R5); 
        prog_car(1,R2); 
        break;
      case 9:
        prog_car(2,R5); 
        prog_car(1,R3); 
        break;
      case 10:
        prog_car(2,R5); 
        prog_car(1,R4); 
        break;
      case 11:
        prog_car(2,R5); 
        prog_car(1,R5); 
        break;
      case 12:
        prog_car(2,R5); 
        prog_car(1,R5); 
        break;
      }

    }


    if (RA>=7 && RA<=12)
    {
      switch (r)
      {
      case 7:
        prog_car(1,R1);
        break;
      case 8:
        prog_car(1,R2);
        break;
      case 9:
        prog_car(1,R3);
        break;
      case 10:
        prog_car(1,R4); 
        break;
      case 11:
        prog_car(1,R5);
        break;
      case 12:
        prog_car(1,R5);
        break;
      }

    }



    if (RA>=13 && RA<=16)
    {
      switch (r)
      {
      case 7:
        prog_car(1,R1);
        prog_car(0,R9);
        break;
      case 8:
        prog_car(1,R2);
        prog_car(0,R9);
        break;
      case 9:
        prog_car(1,R3);
        prog_car(0,R9);
        break;
      case 10:
        prog_car(1,R4); 
        prog_car(0,R9);
        break;
      case 11:
        prog_car(1,R5);
        prog_car(0,R9);
        break;
      case 12:
        prog_car(1,R5);
        prog_car(0,R9);
        break;
      }

    }



  }



  if (r>=13 && r<=16)

  {
    if (RA==0)
    {
      switch (r)
      {
      case 13:
        prog_car(2,R5); 
        prog_car(1,R5);
        prog_car(0,R6);
        break;
      case 14:
        prog_car(2,R5); 
        prog_car(1,R5); 
        prog_car(0,R7);
        break;
      case 15:
        prog_car(2,R5); 
        prog_car(1,R5); 
        prog_car(0,R8);
        break;
      case 16:
        prog_car(2,R5); 
        prog_car(1,R5); 
        prog_car(0,R5);
        break;
      }
      devolve[2] = 2;
      sLCD.write(COMMAND);                   
      sLCD.write(LINE0 + 2);
      sLCD.write(devolve[2]);
    }

    if (RA>=1 && RA<=6)
    {
      switch (r)
      {
      case 13:
        prog_car(2,R5); 
        prog_car(1,R5); 
        prog_car(0,R6); 
        break;
      case 14:
        prog_car(2,R5); 
        prog_car(1,R5); 
        prog_car(0,R7); 
        break;
      case 15:
        prog_car(2,R5); 
        prog_car(1,R5); 
        prog_car(0,R8); 
        break;
      case 16:
        prog_car(2,R5); 
        prog_car(1,R5); 
        prog_car(0,R5); 
        break;
      }

    }


    if (RA>=7 && RA<=12)
    {
      switch (r)
      {
      case 13:
        prog_car(1,R5);
        prog_car(0,R6);
        break;
      case 14:
        prog_car(1,R5);
        prog_car(0,R7);
        break;
      case 15:
        prog_car(1,R5);
        prog_car(0,R8);
        break;
      case 16:
        prog_car(1,R5);
        prog_car(0,R5); 
        break;
      }

    }



    if (RA>=13 && RA<=16)
    {
      switch (r)
      {
      case 13:
        prog_car(0,R6);
        break;
      case 14:
        prog_car(0,R7);
        break;
      case 15:
        prog_car(0,R8);
        break;
      case 16:
        prog_car(0,R5);
        break;
      }

    }



  }


  RA=r;

}


// -----------------------------------------------------------------------------------------------------------------

//barra principal


void barra_E (int e)

{



  if (e==0)  //se for 0 escreve tudo

  {


    if (EA !=0)

    {

      prog_car(3,E6);
      prog_car(4,E5);
      prog_car(5,E0);

      flagE4 = false;
      flagE5 = false;
      flagE6 = false;

      esc[0]=5;
      esc[1]=3;
      esc[2]=3;
      esc[3]=3;
      esc[4]=3;
      esc[5]=4;
      esc[6]=3;
      esc[7]=3;
      esc[8]=3;
      esc[9]=3;
      esc[10]=4;


      sLCD.write(COMMAND);                   
      sLCD.write(LINE0+3);


      sLCD.write(esc[0]);
      if (EA>=5)  sLCD.write(esc[1]);
      if (EA>=11) sLCD.write(esc[2]);
      if (EA>=17) sLCD.write(esc[3]);
      if (EA>=23) sLCD.write(esc[4]);
      if (EA>=29) sLCD.write(esc[5]);
      if (EA>=35) sLCD.write(esc[6]);
      if (EA>=41) sLCD.write(esc[7]);
      if (EA>=47) sLCD.write(esc[8]);
      if (EA>=53) sLCD.write(esc[9]);
      if (EA>=59) sLCD.write(esc[10]);



    }


  }

  else  //  e!=0

  {

    if (!flagE4)
    {
      prog_car(2,E4);
      flagE4 = true;
    };

    if (e <=53 && !flagE6)
    {
      prog_car(3,E6);
      flagE6 =true;
    }

    if (e <=59 && !flagE5)
    {
      prog_car(4,E5);
      flagE5 = true; 
    }


  }




  //----------------------------------------------

  if (e>=1 && e<=5)
  {


    esc[0]=5;
    esc[1]=3;
    esc[2]=3;
    esc[3]=3;
    esc[4]=3;
    esc[5]=4;
    esc[6]=3;
    esc[7]=3;
    esc[8]=3;
    esc[9]=3;
    esc[10]=4;



    sLCD.write(COMMAND);                   
    sLCD.write(LINE0+3);

    sLCD.write(esc[0]);
    if (EA>=5)  sLCD.write(esc[1]);
    if (EA>=11) sLCD.write(esc[2]);
    if (EA>=17) sLCD.write(esc[3]);
    if (EA>=23) sLCD.write(esc[4]);
    if (EA>=29) sLCD.write(esc[5]);
    if (EA>=35) sLCD.write(esc[6]);
    if (EA>=41) sLCD.write(esc[7]);
    if (EA>=47) sLCD.write(esc[8]);
    if (EA>=53) sLCD.write(esc[9]);
    if (EA>=59) sLCD.write(esc[10]);

    switch (e)
    {
    case 1:
      prog_car(5,E1);
      break;
    case 2:
      prog_car(5,E2); 
      break;
    case 3:
      prog_car(5,E3); 
      break;
    case 4:
      prog_car(5,E4);
      break;
    case 5:
      prog_car(5,E4); 
      break;

    }

  }


  //-------------------------------------------------------------------


  if (e>=6 && e<=11)
  {



    esc[0]=2;
    esc[1]=5;
    esc[2]=3;
    esc[3]=3;
    esc[4]=3;
    esc[5]=4;
    esc[6]=3;
    esc[7]=3;
    esc[8]=3;
    esc[9]=3;
    esc[10]=4;


    if (EA>=11)

    {
      sLCD.write(COMMAND);                   
      sLCD.write(LINE0+4);

      sLCD.write(esc[1]);
      sLCD.write(esc[2]);
      if (EA>=17) sLCD.write(esc[3]);
      if (EA>=23) sLCD.write(esc[4]);
      if (EA>=29) sLCD.write(esc[5]);
      if (EA>=35) sLCD.write(esc[6]);
      if (EA>=41) sLCD.write(esc[7]);
      if (EA>=47) sLCD.write(esc[8]);
      if (EA>=53) sLCD.write(esc[9]);
      if (EA>=59) sLCD.write(esc[10]);



    }

    else

    {

      sLCD.write(COMMAND);                   
      sLCD.write(LINE0+3);

      sLCD.write(esc[0]);
      sLCD.write(esc[1]);

    }

    switch (e)
    {
    case 6:
      prog_car(5,E0);
      break;
    case 7:
      prog_car(5,E1); 
      break;
    case 8:
      prog_car(5,E2); 
      break;
    case 9:
      prog_car(5,E3);
      break;
    case 10:
      prog_car(5,E4); 
      break;
    case 11:
      prog_car(5,E4); 
      break;

    }


  }


  //-------------------------------------------------------------------



  if (e>=12 && e<=17)
  {

    esc[0]=2;
    esc[1]=2;
    esc[2]=5;
    esc[3]=3;
    esc[4]=3;
    esc[5]=4;
    esc[6]=3;
    esc[7]=3;
    esc[8]=3;
    esc[9]=3;
    esc[10]=4;



    if (EA>=17)

    {


      sLCD.write(COMMAND);                   
      sLCD.write(LINE0+5);


      sLCD.write(esc[2]);
      sLCD.write(esc[3]);
      if (EA>=23) sLCD.write(esc[4]);
      if (EA>=29) sLCD.write(esc[5]);
      if (EA>=35) sLCD.write(esc[6]);
      if (EA>=41) sLCD.write(esc[7]);
      if (EA>=47) sLCD.write(esc[8]);
      if (EA>=53) sLCD.write(esc[9]);
      if (EA>=59) sLCD.write(esc[10]);
    }

    else

    {

      sLCD.write(COMMAND);                   
      sLCD.write(LINE0+3);

      sLCD.write(esc[0]);
      sLCD.write(esc[1]);
      sLCD.write(esc[2]);

    }



    switch (e)
    {
    case 12:
      prog_car(5,E0);
      break;
    case 13:
      prog_car(5,E1); 
      break;
    case 14:
      prog_car(5,E2); 
      break;
    case 15:
      prog_car(5,E3);
      break;
    case 16:
      prog_car(5,E4); 
      break;
    case 17:
      prog_car(5,E4); 
      break;

    }


  }


  //-------------------------------------------------------------------




  if (e>=18 && e<=23)
  {





    esc[0]=2;
    esc[1]=2;
    esc[2]=2;
    esc[3]=5;
    esc[4]=3;
    esc[5]=4;
    esc[6]=3;
    esc[7]=3;
    esc[8]=3;
    esc[9]=3;
    esc[10]=4;

    if (EA>=23)

    {

      sLCD.write(COMMAND);                   
      sLCD.write(LINE0+6);


      sLCD.write(esc[3]);
      sLCD.write(esc[4]);
      if (EA>=29) sLCD.write(esc[5]);
      if (EA>=35) sLCD.write(esc[6]);
      if (EA>=41) sLCD.write(esc[7]);
      if (EA>=47) sLCD.write(esc[8]);
      if (EA>=53) sLCD.write(esc[9]);
      if (EA>=59) sLCD.write(esc[10]);

    }

    else

    {

      sLCD.write(COMMAND);                   
      sLCD.write(LINE0+3);

      sLCD.write(esc[0]);
      sLCD.write(esc[1]);
      sLCD.write(esc[2]);
      sLCD.write(esc[3]);

    }
    switch (e)
    {
    case 18:
      prog_car(5,E0);
      break;
    case 19:
      prog_car(5,E1); 
      break;
    case 20:
      prog_car(5,E2); 
      break;
    case 21:
      prog_car(5,E3);
      break;
    case 22:
      prog_car(5,E4); 
      break;
    case 23:
      prog_car(5,E4); 
      break;

    }

  }


  //-------------------------------------------------------------------


  if (e>=24 && e<=29)
  {


    esc[0]=2;
    esc[1]=2;
    esc[2]=2;
    esc[3]=2;
    esc[4]=5;
    esc[5]=4;
    esc[6]=3;
    esc[7]=3;
    esc[8]=3;
    esc[9]=3;
    esc[10]=4;

    if (EA>=29)

    {

      sLCD.write(COMMAND);                   
      sLCD.write(LINE0+7);


      sLCD.write(esc[4]);
      sLCD.write(esc[5]);
      if (EA>=35) sLCD.write(esc[6]);
      if (EA>=41) sLCD.write(esc[7]);
      if (EA>=47) sLCD.write(esc[8]);
      if (EA>=53) sLCD.write(esc[9]);
      if (EA>=59) sLCD.write(esc[10]);

    }

    else

    {

      sLCD.write(COMMAND);                   
      sLCD.write(LINE0+3);

      sLCD.write(esc[0]);
      sLCD.write(esc[1]);
      sLCD.write(esc[2]);
      sLCD.write(esc[3]);
      sLCD.write(esc[4]);

    }


    switch (e)
    {
    case 24:
      prog_car(5,E0);
      break;
    case 25:
      prog_car(5,E1); 
      break;
    case 26:
      prog_car(5,E2); 
      break;
    case 27:
      prog_car(5,E3);
      break;
    case 28:
      prog_car(5,E4); 
      break;
    case 29:
      prog_car(5,E4); 
      break;

    }
  }


  //-------------------------------------------------------------------


  if (e>=30 && e<=35)
  {

    esc[0]=2;
    esc[1]=2;
    esc[2]=2;
    esc[3]=2;
    esc[4]=2;
    esc[5]=5;
    esc[6]=3;
    esc[7]=3;
    esc[8]=3;
    esc[9]=3;
    esc[10]=4;



    if (EA>=35)

    {

      sLCD.write(COMMAND);                   
      sLCD.write(LINE0+8);


      sLCD.write(esc[5]);
      sLCD.write(esc[6]);
      if (EA>=41) sLCD.write(esc[7]);
      if (EA>=47) sLCD.write(esc[8]);
      if (EA>=53) sLCD.write(esc[9]);
      if (EA>=59) sLCD.write(esc[10]);

    }

    else

    {

      sLCD.write(COMMAND);                   
      sLCD.write(LINE0+3);

      sLCD.write(esc[0]);
      sLCD.write(esc[1]);
      sLCD.write(esc[2]);
      sLCD.write(esc[3]);
      sLCD.write(esc[4]);
      sLCD.write(esc[5]);

    }


    switch (e)
    {
    case 30:
      prog_car(5,E7);
      break;
    case 31:
      prog_car(5,E8); 
      break;
    case 32:
      prog_car(5,E9); 
      break;
    case 33:
      prog_car(5,E4);
      break;
    case 34:
      prog_car(5,E4); 
      break;
    case 35:
      prog_car(5,E4); 
      break;

    }


  }


  //-------------------------------------------------------------------


  if (e>=36 && e<=41)
  {


    esc[0]=2;
    esc[1]=2;
    esc[2]=2;
    esc[3]=2;
    esc[4]=2;
    esc[5]=2;
    esc[6]=5;
    esc[7]=3;
    esc[8]=3;
    esc[9]=3;
    esc[10]=4;




    if (EA>=41)

    {

      sLCD.write(COMMAND);                   
      sLCD.write(LINE0+9);

      sLCD.write(esc[6]);
      sLCD.write(esc[7]);
      if (EA>=47) sLCD.write(esc[8]);
      if (EA>=53) sLCD.write(esc[9]);
      if (EA>=59) sLCD.write(esc[10]);

    }

    else

    {

      sLCD.write(COMMAND);                   
      sLCD.write(LINE0+3);

      sLCD.write(esc[0]);
      sLCD.write(esc[1]);
      sLCD.write(esc[2]);
      sLCD.write(esc[3]);
      sLCD.write(esc[4]);
      sLCD.write(esc[5]);
      sLCD.write(esc[6]);

    }

    switch (e)
    {
    case 36:
      prog_car(5,E0);
      break;
    case 37:
      prog_car(5,E1); 
      break;
    case 38:
      prog_car(5,E2); 
      break;
    case 39:
      prog_car(5,E3);
      break;
    case 40:
      prog_car(5,E4); 
      break;
    case 41:
      prog_car(5,E4); 
      break;

    }


  }


  //-------------------------------------------------------------------


  if (e>=42 && e<=47)
  {


    esc[0]=2;
    esc[1]=2;
    esc[2]=2;
    esc[3]=2;
    esc[4]=2;
    esc[5]=2;
    esc[6]=2;
    esc[7]=5;
    esc[8]=3;
    esc[9]=3;
    esc[10]=4;




    if (EA>=47)

    {

      sLCD.write(COMMAND);                   
      sLCD.write(LINE0+10);


      sLCD.write(esc[7]);
      sLCD.write(esc[8]);
      if (EA>=53) sLCD.write(esc[9]);
      if (EA>=59) sLCD.write(esc[10]);

    }

    else

    {

      sLCD.write(COMMAND);                   
      sLCD.write(LINE0+3);

      sLCD.write(esc[0]);
      sLCD.write(esc[1]);
      sLCD.write(esc[2]);
      sLCD.write(esc[3]);
      sLCD.write(esc[4]);
      sLCD.write(esc[5]);
      sLCD.write(esc[6]);
      sLCD.write(esc[7]);

    }

    switch (e)
    {
    case 42:
      prog_car(5,E0);
      break;
    case 43:
      prog_car(5,E1); 
      break;
    case 44:
      prog_car(5,E2); 
      break;
    case 45:
      prog_car(5,E3);
      break;
    case 46:
      prog_car(5,E4); 
      break;
    case 47:
      prog_car(5,E4); 
      break;

    }

  }


  //-------------------------------------------------------------------


  if (e>=48 && e<=53)
  {

    esc[0]=2;
    esc[1]=2;
    esc[2]=2;
    esc[3]=2;
    esc[4]=2;
    esc[5]=2;
    esc[6]=2;
    esc[7]=2;
    esc[8]=5;
    esc[9]=3;
    esc[10]=4;



    if (EA>=53)

    {

      sLCD.write(COMMAND);                   
      sLCD.write(LINE0+11);


      sLCD.write(esc[8]);
      sLCD.write(esc[9]);
      if (EA>=59) sLCD.write(esc[10]);

    }

    else

    {

      sLCD.write(COMMAND);                   
      sLCD.write(LINE0+3);

      sLCD.write(esc[0]);
      sLCD.write(esc[1]);
      sLCD.write(esc[2]);
      sLCD.write(esc[3]);
      sLCD.write(esc[4]);
      sLCD.write(esc[5]);
      sLCD.write(esc[6]);
      sLCD.write(esc[7]);
      sLCD.write(esc[8]);

    }


    switch (e)
    {
    case 48:
      prog_car(5,E0);
      break;
    case 49:
      prog_car(5,E1); 
      break;
    case 50:
      prog_car(5,E2); 
      break;
    case 51:
      prog_car(5,E3);
      break;
    case 52:
      prog_car(5,E4); 
      break;
    case 53:
      prog_car(5,E4); 
      break;

    }

  }


  //-------------------------------------------------------------------


  if (e>=54 && e<=59)
  {


    esc[0]=2;
    esc[1]=2;
    esc[2]=2;
    esc[3]=2;
    esc[4]=2;
    esc[5]=2;
    esc[6]=2;
    esc[7]=2;
    esc[8]=2;
    esc[9]=5;
    esc[10]=4;



    sLCD.write(COMMAND);                   
    sLCD.write(LINE0+3);

    sLCD.write(esc[0]);
    sLCD.write(esc[1]);
    sLCD.write(esc[2]);
    sLCD.write(esc[3]);
    sLCD.write(esc[4]);
    sLCD.write(esc[5]);
    sLCD.write(esc[6]);
    sLCD.write(esc[7]);
    sLCD.write(esc[8]);
    sLCD.write(esc[9]);
    if (EA>=59) sLCD.write(esc[10]);

    switch (e)
    {
    case 54:
      prog_car(5,E0);
      break;
    case 55:
      prog_car(5,E1); 
      break;
    case 56:
      prog_car(5,E2); 
      break;
    case 57:
      prog_car(5,E3);
      break;
    case 58:
      prog_car(5,E4); 
      break;
    case 59:
      prog_car(5,E4); 
      break;

    }


  }


  //-------------------------------------------------------------------


  if (e>=60 && e<=63)
  {



    switch (e)
    {
    case 60:
      prog_car(5,E7);
      break;
    case 61:
      prog_car(5,E8); 
      break;
    case 62:
      prog_car(5,E9); 
      break;
    case 63:
      prog_car(5,E4);
      break;

    }


    esc[0]=2;
    esc[1]=2;
    esc[2]=2;
    esc[3]=2;
    esc[4]=2;
    esc[5]=2;
    esc[6]=2;
    esc[7]=2;
    esc[8]=2;
    esc[9]=2;
    esc[10]=5;



    sLCD.write(COMMAND);                   
    sLCD.write(LINE0+3);

    sLCD.write(esc[0]);
    sLCD.write(esc[1]);
    sLCD.write(esc[2]);
    sLCD.write(esc[3]);
    sLCD.write(esc[4]);
    sLCD.write(esc[5]);
    sLCD.write(esc[6]);
    sLCD.write(esc[7]);
    sLCD.write(esc[8]);
    sLCD.write(esc[9]);
    sLCD.write(esc[10]);



  }

  EA=e;

}


//------------------------------------------------------------------------------------------------


//bara zona PWR

void barra_P (int p)

{
  char devolve[2];


  if (p==0 && PA!=0)  //se for a primeira vez que chega a 0 escreve no LCD, se for repetido não faz nada

  {
    // P0 P6
    prog_car(6,P0);
    prog_car(7,P6);

    devolve[0] = 6;
    devolve[1] = 7;


    sLCD.write(COMMAND);                   
    sLCD.write(LINE0+14);
    sLCD.write(devolve[0]);
    sLCD.write(devolve[1]);


  }




  if (p>=1 && p<=6)

  {
    if (PA>=0 && PA<=6)
    {
      switch (p)
      {
      case 1:
        prog_car(6,P1); 
        break;
      case 2:
        prog_car(6,P2); 
        break;
      case 3:
        prog_car(6,P3); 
        break;
      case 4:
        prog_car(6,P4); 
        break;
      case 5:
        prog_car(6,P5); 
        break;
      case 6:
        prog_car(6,P5); 
        break;
      }

    }

  }


  if (PA>=7 && PA<=10)
  {
    switch (p)
    {
    case 1:
      prog_car(6,P1);
      prog_car(7,P6);
      break;
    case 2:
      prog_car(6,P2); 
      prog_car(7,P6);
      break;
    case 3:
      prog_car(6,P3); 
      prog_car(7,P6);
      break;
    case 4:
      prog_car(6,P4);
      prog_car(7,P6); 
      break;
    case 5:
      prog_car(6,P5); 
      prog_car(7,P6);
      break;
    case 6:
      prog_car(6,P5); 
      prog_car(7,P6);
      break;
    }

  }









  if (p>=7 && p<=10)

  {
    if (PA>=0 && PA<=6)
    {
      switch (p)
      {
      case 7:
        prog_car(6,P5); 
        prog_car(7,P7);
        break;
      case 8:
        prog_car(6,P5); 
        prog_car(7,P8); 
        break;
      case 9:
        prog_car(6,P5); 
        prog_car(7,P9); 
        break;
      case 10:
        prog_car(6,P5); 
        prog_car(7,P5); 
        break;

      }

    }


    if (PA>=7 && PA<=10)
    {
      switch (p)
      {
      case 7:
        prog_car(7,P7);
        break;
      case 8:
        prog_car(7,P8);
        break;
      case 9:
        prog_car(7,P9);
        break;
      case 10:
        prog_car(7,P5); 
        break;

      }

    }
  }

  PA=p;

}

// -------------------------------------------------------------------------

//função que lê dados do CAN-BUS


void ecu_3(int *dados_f) 
{

  timeout = 0;
  for (count=0;count<=9;count++)
  {
    dados_f[count]=9999;
  }

  message_ok0 = false;
  message_ok1 = false;
  message_ok2 = false;
  message_ok3 = false;
  message_ok4 = false;
  message_ok5 = false;
  message_ok6 = false;
  message_ok7 = false;
  message_ok8 = false;
  message_ok9 = false;


  mcp2515_bit_modify(CANCTRL, (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0), 0);


  while(timeout < 8000)
  {
    timeout++;
    if (mcp2515_check_message()) 
    {
      if (mcp2515_get_message(&message)) 
      {
        //na posição 0 modo EV
        //na posição 1 temperatura
        //na posição 2 dimmer
        //na posição 3 RPM
        //na posição 4 corrente da bateria
        //na posiçãoo 5 acelerador
        //na posição 6 travão
        //na posição 7 velocidade
        //na posição 8 mudança
        //na posição 9 SOC

        if((message.id == 0x52C && !message_ok1) )	// Verificar o ID da temperatura
        {
          dados_f[1] =  (message.data[1]) / 2 ; //converter em graus C
          message_ok1 = true ;
        }

        if((message.id == 0x529 && !message_ok0) )	// Verificar o ID do modo EV
        {
          dados_f[0] = message.data[4] ;
          message_ok0 = true;
        }

        if((message.id == 0x57F && !message_ok2) )	// Verificar o ID da luminosidade
        {
          dados_f[2] = message.data[2] ;
          message_ok2 = true;
        }

        if((message.id == 0x3C8 && !message_ok3) )	// Verificar o ID das RPM
        {
          valor2 =  ((message.data[2]*256) + message.data[3]) / 8 ;  // converter em RPM
          dados_f[3] = valor2;  
          message_ok3 = true;
        }

        if((message.id == 0x3B && !message_ok4) )	// Verificar o ID da corrente da bateria
        {
          valor = ((message.data[0]) * 256) + (message.data[1]);   //juntar os dois bytes
          if ( (valor & 0x800) != 0 )
          { 
            valor = valor - 0x1000; 
          }  //converter 12 bits em 16 bits
          dados_f[4] =  valor / 10 ;                                       //converter em amperes
          message_ok4 = true; 
        }

        if((message.id == 0x244 && !message_ok5) )	// Verificar o ID do acelerador
        {
          dados_f[5] =  message.data[6];  // 0 a 200
          message_ok5 = true;
        }

        if((message.id == 0x30 && !message_ok6) )	// Verificar o ID do travão
        {
          flut =  ((message.data[4] * 100) / 127);  //converter em %
          dados_f[6] = flut;
          message_ok6 = true;
        }

        if((message.id == 0xB4 && !message_ok7) )	// Verificar o ID da velocidade
        {
          valor2 = ( message.data[5] << 8) | (message.data[6]);
          valor2 = ((valor2 * 10) / 1024);
          dados_f[7] = valor2;                       
          message_ok7 = true;
        }

        if((message.id == 0x120 && !message_ok8) )	// Verificar o ID da mudança
        {
          dados[8] = (message.data[5]);
          message_ok8 = true;
        }

        if((message.id == 0x3CB && !message_ok9) )	// Verificar o ID do SOC
        {
          valor2 = ( message.data[2] << 8) | (message.data[3]);
          valor2 = (valor2 / 2);
          dados_f[9] = valor2;                       
          message_ok9 = true;
        }


      }
    }
    if(message_ok1 && message_ok0 && message_ok2 && message_ok3 && message_ok4 && message_ok5 && message_ok6 && message_ok7 && message_ok8 && message_ok9) return;

  }
  return;
}



//função para iniciar o chip 2515
char inic(unsigned char speed) 
{
  return mcp2515_init(speed);
}


