#Include <18F4550.h>
//#include <stdlib.h>
//#include <stdio.h>
#Fuses HS, NOWDT, NOPROTECT, INTRC, NOBROWNOUT, PLL4, CPUDIV1
#Use delay (clock=2M, crystal=8M)
#Include <GLCD.C>

                 //MAPEO DE PINES//
////////////////////////////////////////////////////////
//  PIN:  |       DESCRIPCIÓN:         |  UBICACIÓN:  //
//        |                            |              //
//PIN_C7  | MOVIMIENTO A LA IZQUIERDA. |  0b10000000  //
//PIN_C6  | MOVIMIENTO A LA DERECHA.   |  0b01000000  //
//PIN_A0  | MOVIMIENTO HACIA ABAJO.    |  0b00000001  //
//PIN_A1  | MOVIMIENTO HACIA ARRIBA.   |  0b00000010  //
//        |                            |              //
//PIN_C1  | BOTON DE SELECCION.        |  0b00000010  //
//----------------------------------------------------//
//PIN_A0  | BOTON PARA SELECCIONAR 9 ESPACIOS.        //
//PIN_A1  | BOTON PARA SELECCIONAR 25 ESPACIOS.       //
//////////////////////////////////////////////////////// 

//Direcciones de memoria del puerto C.
#BYTE TRISC=0xF94
#BYTE PORTC=0xF82

//Direcciones de memoria del puerto A.
#BYTE TRISA=0xF92
#BYTE PORTA=0xF80


//Direcciones de memoria de los registros principales para el control de interrupciones.
#BYTE INTCON=0xFF2
#BYTE INTCON2=0xFF1
#BYTE INTCON3=0xFF0

Int1 X_tres [19][19] = 
{
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
{0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0}
{0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0}
{0,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0}
{0,0,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,0,0}
{0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,1,0,0,0}
{0,0,0,0,1,1,1,1,0,0,0,1,1,1,1,0,0,0,0}
{0,0,0,0,0,1,1,1,1,0,1,1,1,1,0,0,0,0,0}
{0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0}
{0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0}
{0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0}
{0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0}
{0,0,0,0,0,1,1,1,1,0,1,1,1,1,0,0,0,0,0}
{0,0,0,0,1,1,1,1,0,0,0,1,1,1,1,0,0,0,0}
{0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,1,0,0,0}
{0,0,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,0,0}
{0,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0}
{0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0}
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

Int1 X_cinco [10][10] =
{
{1,1,0,0,0,0,0,0,1,1}
{1,1,1,0,0,0,0,1,1,1}
{0,1,1,1,0,0,1,1,1,0}
{0,0,1,1,1,1,1,1,0,0}
{0,0,0,1,1,1,1,0,0,0}
{0,0,0,1,1,1,1,0,0,0}
{0,0,1,1,1,1,1,1,0,0}
{0,1,1,1,0,0,1,1,1,0}
{1,1,1,0,0,0,0,1,1,1}
{1,1,0,0,0,0,0,0,1,1}
};



//COORDENADAS A,B,C Y D PARA LAS X
//Coordenada de inicio para los gráficos en X.
Int8 A = 63;
//Coordenada de inicio para los gráficos en Y.
Int8 B = 1;

Int8 C = 63;

Int8 D = 1;

//COORDENADAS A_CIRCULO,B_CIRCULO,C_CIRCULO Y D_CIRCULO PARA LAS 0

Int8 A_Circulo = 0;
Int8 B_Circulo = 0;

Int8 C_Circulo = 0;
Int8 D_Circulo = 0;

//La variable opcion determina si se juega con un plano de 9 o de 25 espacios. 
Int8 Opcion = 0;

//La variable turnoComprobar determina si se ha pasado el turno 1
int8 turnoComprobar=0;

//La variable posiciontotal guarda la posicion que esta ocupada haciendo uso de la funcion Void PosicionesPlayer1(); 
int8 posiciontotal;

//Las variable posn determinan si es la primera vez que se escribe un grafico en una posicion de la matriz
//en caso de que sea mas de una, se escribe el mensaje de error y se repite el turno
int8 pos1=0,pos2=0,pos3=0,pos4=0,pos5=0,pos6=0,pos7=0,pos8=0,pos9=0;
int8 pos21=0,pos22=0,pos23=0,pos24=0,pos25=0,pos26=0,pos27=0,pos28=0,pos29=0;

//Declaracion de matriz 3x3 y 5x5 para guardar un historial de cada grafico 
Int8 MatPosiciones [3][3];
Int8 MatPosiciones_5 [5][5];

//Declaracion de los mensajes para el menu del juego y el mensaje ganador
Char Opcion1[] = "PRESS LEFT BUTTON TO PLAY WITH 9 SPACES";
Char Opcion2[] = "PRESS RIGHT BUTTON TO PLAY WITH 25 SPACES";

Char Gato25[] = "TIC TAC TOE \n WITH 25 SPACES";
Char Gato9[] = "TIC TAC TOE \n WITH 9 SPACES";

Char Star_Jugador1[] = "START GAME: PLAYER A";
Char Star_Jugador2[] = "PLAYER B";
char Star1_Jugador1[] = "PLAYER A";

Char Escoger1[] = "CHOOSE A";
Char Escoger2[] = "SPACE!";

Char Ganador[] = "WINNER";
Char Player1[] = "PLAYER1";
Char Player2[] = "PLAYER2";
char mensajeError[]="UNAVAILABLE";
char mensajeError2[]="REPEAT TURN";


Void PLANO25(); //Funcion para imprimir el plano de 25 espacios
Void PLANO9(); //Funcion para imprimir el plano de 9 espacios

//Funciones para actualizar el plano de 9 y de 25 espacios
Void ActualizarPlano9();
Void ActualizarPlano25();

//Funciones para imprimir X y 0 para un juego de 3x3
Void ImprimirX();
Void Imprimir0();

//Funciones para imprimir X y 0 para un juego de 5x5
Void ImprimirX_5();
Void Imprimir0_5();

//Funciones para ir rellenando la matriz con 1 o con 2, dependiendo que si es el jugador 1 o el 2 quien pone su grafico en el lugar de la matriz, respectivamente (3x3)
Void PosicionesPlayer1(); 
Void PosicionesPlayer2();
//Funciones para ir rellenando la matriz con 1 o con 2, dependiendo que si es el jugador 1 o el 2 quien pone su grafico en el lugar de la matriz, respectivamente (5x5)
Void PosicionesPlayer1_5();
Void PosicionesPlayer2_5();

//esta funcion es para volver a establecer las x y los 0 que fueron borrados al inciar otro turno. (Plano 3x3)
Void Impresion_De_JugadasX();
Void Impresion_De_JugadasO();

//esta funcion es para volver a establecer las x y los 0 que fueron borrados al iniciar otro turno (Plano 5x5)
Void Impresion_De_JugadasX_5();
Void Impresion_De_JugadasO_5();

//Funciones para determinar el ganador del juego evaluando las posiciones de la matriz
Void Ganador_Tres();
Void Ganador_Cinco();

//Funcion para el buzzer/motor que se activa al terminar cada turno
void Buzzer();

//Funcion para comprobar si el espacio esta o no ocupado, evitando asi la sobreescritura de graficos
void ComprobarEspacios();

Void main()
{
   GLCD_Init(ON);
   GLCD_fillScreen(OFF);
   GLCD_Rect(0, 0,127,63,0,ON);
   TRISC=0b11000010;
   TRISA=0b00110000;
   Do
   {
      GLCD_Text57(2,2,Opcion1,1,ON);
      GLCD_Text57(2,-20,Opcion2,1,ON);
      If (PORTC & 0b10000000)
      {
         Opcion = 1;
      }
      If (PORTC & 0b01000000)
      {
         Opcion = 2;
      }
      Switch(Opcion)
      {
         Case 1:
         {
            PLANO9();
            Opcion = 3;
            Break;
         }
         Case 2:
         {
            PLANO25();
            Opcion = 3;
            Break;     
         }
      }
   } While (Opcion != 3);
}
   

Void PLANO9()
{
   Int turno = 0;
   GLCD_fillScreen(OFF);
   GLCD_Text57(0,0,Gato9,1,ON);
   Delay_ms(200);
   GLCD_Text57(0,0,Gato9,1,OFF);
   Delay_ms(300);
   GLCD_Text57(0,0,Star_Jugador1,1,ON);
   Delay_ms(300);
   GLCD_Text57(0,0,Star_Jugador1,1,OFF);
   Delay_ms(300);
   
   for(int i=0;i<3;i++)
      for(int j=0; j<3;j++)
         {  
            MatPosiciones[i][j]= 0;
         }
   
   ActualizarPlano9();
   
   while(TRUE)
   {
      
      GLCD_Text57(0,0,Escoger1,1,OFF);
      GLCD_Text57(0,12,Escoger2,1,OFF);
      GLCD_Text57(0,0,Star1_Jugador1,1,ON);
      delay_ms(300);
      GLCD_Text57(0,0,Star1_Jugador1,1,OFF);
      delay_ms(300);
      
      
      GLCD_Text57(0,0,Escoger1,1,ON);
      GLCD_Text57(0,12,Escoger2,1,ON);
   
      bucle1:
      RepetirTurno:
      
      //Funcion que realiza la aninmacion de imprimir la X
      ImprimirX();
      
      
      if(PORTC & 0b01000000)     //Mover derecha
      {
         
         if(A < 103)
         {
            A += 21;
         }
         else
         {
            A += 0;
         }
      }
         
      if(PORTC & 0b10000000)     //Mover izquierda
      {
         
         if(A > 64)
         {
            A -= 21;
         }
         else
         {
           A += 0; 
         }
      }
   
      if(PORTA & 0b00000001)     //Mover abajo
      {
         if(B < 43)
         {
            B += 22;
         }
         else
         {
            B += 0;
         }
      }
   
      if(PORTA & 0b00000010)     //Mover arriba
      {
         
         if(B > 2)
         {
            B -= 22;
         }
         else
         {
            B += 0;
         }
      }
      
      //Sí se presiona el boton de selector:
      if(PORTC & 0B00000010)
      {    
           PosicionesPlayer1(); 
           if(turnoComprobar>0){
               goto selector1;}
            
           if((posiciontotal==1) && (A==63) && (B==1))
           {
               pos1++;
               if(pos1>1){
                  GLCD_Text57(0,45,mensajeError,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError,1,OFF);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,OFF);
                  goto RepetirTurno;
                  turnoComprobar++;}
           }
               
      if((posiciontotal==2) && (A==84) && (B==1))
           {
               pos2++;
               if(pos2>1){
                  GLCD_Text57(0,45,mensajeError,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError,1,OFF);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,OFF);
                  goto RepetirTurno;
                  turnoComprobar++; }
           } 
           if((posiciontotal==3) && (A==105) && (B==1))
           {
               pos3++;
               if(pos3>1){
                  GLCD_Text57(0,45,mensajeError,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError,1,OFF);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,OFF);
                  goto RepetirTurno;
                  turnoComprobar++;}
           }
       if((posiciontotal==4) && (A==63) && (B==23))
           {
               pos4++;
               if(pos4>1){
                  GLCD_Text57(0,45,mensajeError,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError,1,OFF);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,OFF);
                  goto RepetirTurno;
                  turnoComprobar++; }
           }
                  
         if((posiciontotal==5) && (A==84) && (B==23))
           {
               pos5++;
               if(pos5>1){
                  GLCD_Text57(0,45,mensajeError,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError,1,OFF);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,OFF);
                  goto RepetirTurno;
                  turnoComprobar++;}
           }         
          if((posiciontotal==6) && (A==105) && (B==23))
           {
               pos6++;
               if(pos6>1){
                  GLCD_Text57(0,45,mensajeError,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError,1,OFF);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,OFF);
                  goto RepetirTurno;
                  turnoComprobar++;}
           }        
          if((posiciontotal==7) && (A==63) && (B==45))
           {
               pos7++;
               if(pos7>1){
                 GLCD_Text57(0,45,mensajeError,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError,1,OFF);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,OFF);
                  goto RepetirTurno;
                  turnoComprobar++; }
           }      
          if((posiciontotal==8) && (A==84) && (B==45))
           {
               pos8++;
               if(pos8>1){
                  GLCD_Text57(0,45,mensajeError,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError,1,OFF);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,OFF);
                  goto RepetirTurno;
                  turnoComprobar++; }
           }
          if((posiciontotal==9) && (A==105) && (B==45))
           {
               pos9++;
               if(pos9>1){
                  GLCD_Text57(0,45,mensajeError,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError,1,OFF);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,OFF);
                  goto RepetirTurno;
                  turnoComprobar++; }
           } 
       
       goto selector1;
      }
      
      
      goto bucle1;
      
      selector1:
      PosicionesPlayer1();
      //Imprime el caracter anterior que fue borrado anteriormente al turno
      Impresion_De_JugadasX();
      //Impresion_De_JugadasO();
      //Evalua las posiciones de la matriz de 3x3 para saber si hay un ganador
      if(Turno != 0){
      Impresion_De_JugadasO();
      }
      buzzer();
      Ganador_Tres();

      delay_ms(50);
   

/////////////////////////////////////////////////////
//                                                 //
//          Fin del turno del jugador A            //
//                                                 //
/////////////////////////////////////////////////////

      //Inicio del juego del jugador B
      GLCD_Text57(0,0,Escoger1,1,OFF);
      GLCD_Text57(0,12,Escoger2,1,OFF);
      delay_ms(100);
      GLCD_Text57(0,0,Star_Jugador2,1,ON);
      delay_ms(300); 
      GLCD_Text57(0,0,Star_Jugador2,1,OFF);
      GLCD_Text57(0,0,Escoger1,1,ON);
      GLCD_Text57(0,12,Escoger2,1,ON);
    
      bucle2:
      RepetirTurno2:
      //Funcion que realiza la animacion de imprimir el 0
      Imprimir0();
      
      If(PORTC & 0B01000000)  //Si se presiona el boton de la derecha.
      {
         if(A_Circulo < 42)
         {
            A_Circulo += 22;
         }
         else
         {
            A_Circulo += 0;
         }
      }
      
      If(PORTC & 0B10000000)  //Se se presiona el boton de la izq.
      {
         if(A_Circulo > 2)
         {
            A_Circulo -= 22;
         }
         else
         {
            A_Circulo += 0;
         }
      }
          
      If(PORTA & 0B00000001)  //Si se presiona el boton de abajo.
      {
         if(B_Circulo < 42)
         {
            B_Circulo += 22;
         }
         else
         {
            B_Circulo += 0;
         }
      }

      If(PORTA & 0B00000010) //Si se presiona el boton de arriba.
      {
         if(B_Circulo > 2)
         {
            B_Circulo -= 22;
         }
         else
         {
            B_Circulo += 0;  
         }
      }
      
      //Sí se presiona el boton de selector:
      If(PORTC & 0B00000010)
      {
         Turno++;
         PosicionesPlayer2(); 
           if(turnoComprobar>0){
               goto seleccionar2;}
            
           if((posiciontotal==21) && (A_Circulo==0) && (B_Circulo==0))
           {
               pos21++;
               if(pos21>1){
                  GLCD_Text57(0,45,mensajeError,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError,1,OFF);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,OFF);
                  goto RepetirTurno2;
                  turnoComprobar++;}
           }
               
      if((posiciontotal==22) && (A_Circulo==22) && (B_Circulo==0))
           {
               pos22++;
               if(pos22>1){
                  GLCD_Text57(0,45,mensajeError,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError,1,OFF);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,OFF);
                  goto RepetirTurno2;
                  turnoComprobar++; }
           } 
           if((posiciontotal==23) && (A_Circulo==44) && (B_Circulo==0))
           {
               pos23++;
               if(pos23>1){
                  GLCD_Text57(0,45,mensajeError,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError,1,OFF);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,OFF);
                  goto RepetirTurno2;
                  turnoComprobar++;}
           }
       if((posiciontotal==24) && (A_Circulo==0) && (B_Circulo==22))
           {
               pos24++;
               if(pos24>1){
                  GLCD_Text57(0,45,mensajeError,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError,1,OFF);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,OFF);
                  goto RepetirTurno2;
                  turnoComprobar++; }
           }
                  
         if((posiciontotal==25) && (A_Circulo==22) && (B_Circulo==22))
           {
               pos25++;
               if(pos25>1){
                  GLCD_Text57(0,45,mensajeError,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError,1,OFF);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,OFF);
                  goto RepetirTurno2;
                  turnoComprobar++;}
           }         
          if((posiciontotal==26) && (A_Circulo==44) && (B_Circulo==22))
           {
               pos26++;
               if(pos26>1){
                  GLCD_Text57(0,45,mensajeError,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError,1,OFF);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,OFF);
                  goto RepetirTurno2;
                  turnoComprobar++;}
           }        
          if((posiciontotal==27) && (A_Circulo==0) && (B_Circulo==44))
           {
               pos27++;
               if(pos27>1){
                 GLCD_Text57(0,45,mensajeError,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError,1,OFF);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,OFF);
                  goto RepetirTurno2;
                  turnoComprobar++; }
           }      
          if((posiciontotal==28) && (A_Circulo==22) && (B_Circulo==44))
           {
               pos28++;
               if(pos28>1){
                  GLCD_Text57(0,45,mensajeError,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError,1,OFF);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,OFF);
                  goto RepetirTurno2;
                  turnoComprobar++; }
           }
          if((posiciontotal==29) && (A_Circulo==44) && (B_Circulo==44))
           {
               pos29++;
               if(pos29>1){
                  GLCD_Text57(0,45,mensajeError,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError,1,OFF);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,ON);
                  Delay_ms(500);
                  GLCD_Text57(0,45,mensajeError2,1,OFF);
                  goto RepetirTurno2;
                  turnoComprobar++; }
           } 
       Goto Seleccionar2;
      }
          
      Goto bucle2;
       
      Seleccionar2:
      
      PosicionesPlayer2();
      Impresion_De_JugadasO();
      if(Turno != 1){
      Impresion_De_JugadasX();
      }
      buzzer();
      Ganador_Tres();
      delay_ms(50);
   }
}


Void PLANO25()
{
   Int8 turno1 = 0;
   GLCD_fillScreen(OFF);
   GLCD_Text57(0,0,Gato25,1,ON);
   Delay_ms(200);
   GLCD_Text57(0,0,Gato25,1,OFF);
   Delay_ms(300);
   GLCD_Text57(0,0,Star_Jugador1,1,ON);
   Delay_ms(300);
   GLCD_Text57(0,0,Star_Jugador1,1,OFF);
   Delay_ms(300);
   
   for(int i=0;i<5;i++)
      for(int j=0; j<5;j++)
         {  
            MatPosiciones_5[i][j]= 0;
         }
         
   ActualizarPlano25();
   while(TRUE)
   {
      GLCD_Text57(0,0,Escoger1,1,OFF);
      GLCD_Text57(0,12,Escoger2,1,OFF);
      GLCD_Text57(0,0,Star1_Jugador1,1,ON);
      delay_ms(300);
      GLCD_Text57(0,0,Star1_Jugador1,1,OFF);
      delay_ms(300);
      GLCD_Text57(0,0,Escoger1,1,ON);
      GLCD_Text57(0,12,Escoger2,1,ON);
   
      bucle1:
      ImprimirX_5();
      
      if(PORTC & 0b01000000)     //Mover derecha
      {
         
         if(C < 103)
         {
            C += 13;
         }
         else
         {
            C += 0;
         }
      }
         
      if(PORTC & 0b10000000)     //Mover izquierda
      {
         if(C > 64)
         {
            C -= 13;
         }
         else
         {
           C += 0; 
         }
      }
   
      if(PORTA & 0b00000001)     //Mover abajo
      {
         
         if(D < 43)
         {
            D += 13;
         }
         else
         {
            D += 0;
         }
      }
   
      if(PORTA & 0b00000010)     //Mover arriba
      {
         
         if(D > 2)
         {
            D -= 13;
         }
         else
         {
            D += 0;
         }
      }
      
      //Sí se presiona el boton de selector:
      if(PORTC & 0B00000010)
      {   
        goto selector1;
      }
      
      goto bucle1;
      
      selector1:
      PosicionesPlayer1_5();
      Impresion_De_JugadasX_5();
      if(Turno1 != 0){
      Impresion_De_JugadasO_5();}
      Buzzer();
      Ganador_Cinco();

      delay_ms(50);
      
/////////////////////////////////////////////////////
//                                                 //
//          Fin del turno del jugador A            //
//                                                 //
/////////////////////////////////////////////////////

      //Inicio del juego del jugador B
      GLCD_Text57(0,0,Escoger1,1,OFF);
      GLCD_Text57(0,12,Escoger2,1,OFF);
      delay_ms(100);
      GLCD_Text57(0,0,Star_Jugador2,1,ON);
      delay_ms(300); 
      GLCD_Text57(0,0,Star_Jugador2,1,OFF);
      GLCD_Text57(0,0,Escoger1,1,ON);
      GLCD_Text57(0,12,Escoger2,1,ON);
    
      bucle2:
      Imprimir0_5();
      
      If(PORTC & 0B01000000)  //Si se presiona el boton de la derecha.
      {
         if(C_Circulo < 42)
         {
            C_Circulo += 13;
         }
         else
         {
            C_Circulo += 0;
         }
      }
      
      If(PORTC & 0B10000000)  //Se se presiona el boton de la izq.
      {
         if(C_Circulo > 2)
         {
            C_Circulo -= 13;
         }
         else
         {
            C_Circulo += 0;
         }
      }
          
      If(PORTA & 0B00000001)  //Si se presiona el boton de abajo.
      {
         if(D_Circulo < 42)
         {
            D_Circulo += 13;
         }
         else
         {
            D_Circulo += 0;
         }
      }

      If(PORTA & 0B00000010) //Si se presiona el boton de arriba.
      {
         if(D_Circulo > 2)
         {
            D_Circulo -= 13;
         }
         else
         {
            D_Circulo += 0;  
         }
      }
      
      //Sí se presiona el boton de selector:
      If(PORTC & 0B00000010)
      {
         Turno1++;
         Goto Seleccionar2;
      }
          
      Goto bucle2;
       
      Seleccionar2:            
      PosicionesPlayer2();
      Impresion_De_JugadasO_5();
      If (Turno1 != 0){
         Impresion_De_JugadasX_5();
      }
      Buzzer();
      Ganador_Cinco();
      delay_ms(50);
   }
       
}

//Funcion para imprimir la X (plano 3x3)
void ImprimirX()
{
   //Imprimimos el grafico de la "X" para el juego de 3x3.
   for(int Y_Grafico_Color = 2; Y_Grafico_Color < 17; Y_Grafico_Color++)
   {
      for(int X_Grafico_Color = 2; X_Grafico_Color < 17; X_Grafico_Color++)
      {
         GLCD_Pixel(A + X_Grafico_Color,B + Y_Grafico_Color, X_tres[X_Grafico_Color][Y_Grafico_Color]);
      }
   }
   delay_ms(50);
   
   //Borramos el grafico de la "X".
   for(int Y_Grafico_Borrado = 2; Y_Grafico_Borrado < 17; Y_Grafico_Borrado++)
   {
      for(int X_Grafico_Borrado = 2; X_Grafico_Borrado < 17; X_Grafico_Borrado++)
      {
         GLCD_Pixel(A + X_Grafico_Borrado,B + Y_Grafico_Borrado, 0);
      }
   }
   delay_ms(50);
}

//Funcion para imprimir las X en el plano de 5x5
void ImprimirX_5()
{
   //Imprimimos el grafico de la "X" para el juego de 5x5.
   for(int Y_Grafico_Color = 0; Y_Grafico_Color < 10; Y_Grafico_Color++)
   {
      for(int X_Grafico_Color = 0; X_Grafico_Color < 10; X_Grafico_Color++)
      {
         GLCD_Pixel(C + X_Grafico_Color,D + Y_Grafico_Color, X_Cinco[X_Grafico_Color][Y_Grafico_Color]);
      }
   }
   delay_ms(50);
   
   //Borramos el grafico de la "X".
   for(int Y_Grafico_Borrado = 0; Y_Grafico_Borrado < 10; Y_Grafico_Borrado++)
   {
      for(int X_Grafico_Borrado = 0; X_Grafico_Borrado < 10; X_Grafico_Borrado++)
      {
         GLCD_Pixel(C + X_Grafico_Borrado,D + Y_Grafico_Borrado, 0);
      }
   }
   delay_ms(50);
}

//Imprimimos el 0 para un juego de 3x3
void Imprimir0()
{
   GLCD_Circle (A_Circulo + 73, B_Circulo + 11, 7, 0, ON);
   delay_ms(50);
         
   GLCD_Circle (A_Circulo +73, B_Circulo + 11, 7, 0, OFF);  
   delay_ms(50);
}

//Imprimimos el 0 para un juego de 5x5
void Imprimir0_5()
{
   GLCD_Circle (C_Circulo + 69, D_Circulo + 6, 4, 0, ON);
   delay_ms(50);
         
   GLCD_Circle (C_Circulo +69, D_Circulo + 6, 4, 0, OFF);  
   delay_ms(50);
}

//Imprimimos plano del juego 3x3
void ActualizarPlano9()
{
   GLCD_Rect(63, 0, 127, 63,0,ON);
   
   GLCD_Line(84,0,84,63,ON);
   GLCD_Line(105,0,105,63,ON);
   
   GLCD_Line(63,22,127,22,ON);
   GLCD_Line(63,43,127,43,ON);
}


//Imprimimos plano del juego 5x5
Void ActualizarPlano25()
{
   GLCD_Rect(62, 0, 127, 63,0,ON);
   
   GLCD_Line(75,0,75,63,ON);
   GLCD_Line(88,0,88,63,ON);
   GLCD_Line(101,0,101,63,ON);
   GLCD_Line(114,0,114,63,ON);
   
   
   GLCD_Line(63,13,127,13,ON);
   GLCD_Line(63,25,127,25,ON);
   GLCD_Line(63,38,127,38,ON);
   GLCD_Line(63,51,127,51,ON);
}

//LAS SIGUIENTES FUNCIONES DE POSICIONES CUMPLEN LA FUNCION DE ESTABLECER SI EN CADA ESPACIO DE LA MATRIZ HAY UNA X O UN 0, ASIGNANDOLE A ESTOS UN VALOR DE 
//1 O DE 2, RESPECTIVAMENTE
Void PosicionesPlayer1()
{
   
   If ((A == 63) && (B == 1))    //Posición [0][0] del tablero.
   {
      
      //Si la posicion [0][0] no esta ocupada por un 0 (tiene un valor 2), entonces [0][0] esta ocupado por una X (que tiene un valor de 1)
      If (MatPosiciones[0][0] != 2)
      MatPosiciones[0][0]=1;
      posiciontotal=1;
   }
 
   If ((A == 84) && (B == 1))    //Posición [0][1] del tablero.
   {
      
      //Si la posicion [0][1] no esta ocupada por un 0 (tiene un valor 2), entonces [0][1] esta ocupado por una X (que tiene un valor de 1)
      If (MatPosiciones[0][1] != 2)
      MatPosiciones[0][1]=1;
      posiciontotal=2;
   }
   If ((A == 105) && (B == 1))   //Posición [0][2] del tablero.
   {
     
      //Si la posicion [0][2] no esta ocupada por un 0 (tiene un valor 2), entonces [0][2] esta ocupado por una X (que tiene un valor de 1)
      If (MatPosiciones[0][2] != 2)
      MatPosiciones[0][2]=1;
      posiciontotal=3;
   }
   
   If ((A == 63) && (B == 23))   //Posición [1][0] del tablero.
   {
      
   //Si la posicion [1][0] no esta ocupada por un 0 (tiene un valor 2), entonces [1][0] esta ocupado por una X (que tiene un valor de 1)
      If (MatPosiciones[1][0] != 2)
      MatPosiciones[1][0]=1;
      posiciontotal=4;
   }
   If ((A == 84) && (B == 23))   //Posición [1][1] del tablero.
   {
     
   //Si la posicion [1][1] no esta ocupada por un 0 (tiene un valor 2), entonces [1][1] esta ocupado por una X (que tiene un valor de 1)
      If (MatPosiciones[1][1] != 2)
      MatPosiciones[1][1]=1;
      posiciontotal=5;
   }
   If ((A == 105) && (B == 23))  //Posición [1][2] del tablero.
   {  
      
      //Si la posicion [1][2] no esta ocupada por un 0 (tiene un valor 2), entonces [1][2] esta ocupado por una X (que tiene un valor de 1)
      If (MatPosiciones[1][2] != 2)
      MatPosiciones[1][2]=1;
      posiciontotal=6;
   }
      
   If ((A == 63) && (B == 45))   //Posición [2][0] del tablero.
   {
      
   //Si la posicion [2][0] no esta ocupada por un 0 (tiene un valor 2), entonces [2][0] esta ocupado por una X (que tiene un valor de 1)
      If (MatPosiciones[2][0] != 2)
      MatPosiciones[2][0]=1;
      posiciontotal=7;
   }
   If ((A == 84) && (B == 45))   //Posición [2][1] del tablero.
   {
      
      //Si la posicion [2][1] no esta ocupada por un 0 (tiene un valor 2), entonces [2][1] esta ocupado por una X (que tiene un valor de 1)
      If (MatPosiciones[2][1] != 2)
      MatPosiciones[2][1]=1;
      posiciontotal=8;
   }
   If ((A == 105) && (B == 45))  //Posición [2][2] del tablero.
   {
      
   //Si la posicion [2][2] no esta ocupada por un 0 (tiene un valor 2), entonces [2][2] esta ocupado por una X (que tiene un valor de 1)
      If (MatPosiciones[2][2] != 2)
      MatPosiciones[2][2]=1;
      posiciontotal=9;
   }
}

Void PosicionesPlayer1_5()
{
   If ((C == 63) && (D == 1))    //Posición [0][0] del tablero.
   {
      If (MatPosiciones_5[0][0] != 2)
      MatPosiciones_5[0][0]=1;
   }
   If ((C == 76) && (D == 1))    //Posición [0][1] del tablero.
   {
      If (MatPosiciones_5[0][1] != 2)
      MatPosiciones_5[0][1]=1;
   }
   If ((C == 89) && (D == 1))   //Posición [0][2] del tablero.
   {
      If (MatPosiciones_5[0][2] != 2)
      MatPosiciones_5[0][2]=1;
   }
   If ((C == 102) && (D == 1))   //Posición [0][3] del tablero.
   {
      If (MatPosiciones_5[0][3] != 2)
      MatPosiciones_5[0][3]=1;
   }
   If ((C == 115) && (D == 1))   //Posición [0][4] del tablero.
   {
      If (MatPosiciones_5[0][4] != 2)
      MatPosiciones_5[0][4]=1;
   }
   
   If ((C == 63) && (D == 14))    //Posición [1][0] del tablero.
   {
      If (MatPosiciones_5[1][0] != 2)
      MatPosiciones_5[1][0]=1;
   }
   If ((C == 76) && (D == 14))    //Posición [1][1] del tablero.
   {
      If (MatPosiciones_5[1][1] != 2)
      MatPosiciones_5[1][1]=1;
   }
   If ((C == 89) && (D == 14))   //Posición [1][2] del tablero.
   {
      If (MatPosiciones_5[1][2] != 2)
      MatPosiciones_5[1][2]=1;
   }
   If ((C == 102) && (D == 14))   //Posición [1][3] del tablero.
   {
      If (MatPosiciones_5[1][3] != 2)
      MatPosiciones_5[1][3]=1;
   }
   If ((C == 115) && (D == 14))   //Posición [1][4] del tablero.
   {
      If (MatPosiciones_5[1][4] != 2)
      MatPosiciones_5[1][4]=1;
   }
   
   If ((C == 63) && (D == 27))    //Posición [2][0] del tablero.
   {
      If (MatPosiciones_5[2][0] != 2)
      MatPosiciones_5[2][0]=1;
   }
   If ((C == 76) && (D == 27))    //Posición [2][1] del tablero.
   {
      If (MatPosiciones_5[2][1] != 2)
      MatPosiciones_5[2][1]=1;
   }
   If ((C == 89) && (D == 27))   //Posición [2][2] del tablero.
   {
      If (MatPosiciones_5[2][2] != 2)
      MatPosiciones_5[2][2]=1;
   }
   If ((C == 102) && (D == 27))   //Posición [2][3] del tablero.
   {
      If (MatPosiciones_5[2][3] != 2)
      MatPosiciones_5[2][3]=1;
   }
   If ((C == 115) && (D == 27))   //Posición [2][4] del tablero.
   {
      If (MatPosiciones_5[2][4] != 2)
      MatPosiciones_5[2][4]=1;
   }
   
   If ((C == 63) && (D == 40))    //Posición [3][0] del tablero.
   {
      If (MatPosiciones_5[3][0] != 2)
      MatPosiciones_5[3][0]=1;
   }
   If ((C == 76) && (D == 40))    //Posición [3][1] del tablero.
   {
      If (MatPosiciones_5[3][1] != 2)
      MatPosiciones_5[3][1]=1;
   }
   If ((C == 89) && (D== 40))   //Posición [3][2] del tablero.
   {
      If (MatPosiciones_5[3][2] != 2)
      MatPosiciones_5[3][2]=1;
   }
   If ((C == 102) && (D == 40))   //Posición [3][3] del tablero.
   {
      If (MatPosiciones_5[3][3] != 2)
      MatPosiciones_5[3][3]=1;
   }
   If ((C == 115) && (D == 40))   //Posición [3][4] del tablero.
   {
      If (MatPosiciones_5[3][4] != 2)
      MatPosiciones_5[3][4]=1;
   }
   
   If ((C == 63) && (D == 53))    //Posición [4][0] del tablero.
   {
      If (MatPosiciones_5[4][0] != 2)
      MatPosiciones_5[4][0]=1;
   }
   If ((C == 76) && (D == 53))    //Posición [4][1] del tablero.
   {
      If (MatPosiciones_5[4][1] != 2)
      MatPosiciones_5[4][1]=1;
   }
   If ((C == 89) && (D == 53))   //Posición [4][2] del tablero.
   {
      If (MatPosiciones_5[4][2] != 2)
      MatPosiciones_5[4][2]=1;
   }
   If ((C == 102) && (D == 53))   //Posición [4][3] del tablero.
   {
      If (MatPosiciones_5[4][3] != 2)
      MatPosiciones_5[4][3]=1;
   }
   If ((C == 115) && (D == 53))   //Posición [4][4] del tablero.
   {
      If (MatPosiciones_5[4][4] != 2)
      MatPosiciones_5[4][4]=1;
   }
}

Void PosicionesPlayer2()
{
   If ((A_Circulo == 0) && (B_Circulo == 0))    //Posición [0][0] del tablero.
   {
      If (MatPosiciones[0][0] != 1)
      MatPosiciones[0][0]=2;
      posiciontotal=21;
   }
   If ((A_Circulo == 22) && (B_Circulo == 0))    //Posición [0][1] del tablero.
   {
      If (MatPosiciones[0][1] != 1)
      MatPosiciones[0][1]=2;
      posiciontotal=22;
   }
   If ((A_Circulo == 44) && (B_Circulo == 0))   //Posición [0][2] del tablero.
   {
      If (MatPosiciones[0][2] != 1)
      MatPosiciones[0][2]=2;
      posiciontotal=23;
   }
   
   If ((A_Circulo == 0) && (B_Circulo == 22))   //Posición [1][0] del tablero.
   {
      If (MatPosiciones[1][0] != 1)
      MatPosiciones[1][0]=2;
      posiciontotal=24;
   }
   If ((A_Circulo == 22) && (B_Circulo == 22))   //Posición [1][1] del tablero.
   {
      If (MatPosiciones[1][1] != 1)
      MatPosiciones[1][1]=2;
      posiciontotal=25;
   }
   If ((A_Circulo == 44) && (B_Circulo == 22))  //Posición [1][2] del tablero.
   {
      If (MatPosiciones[1][2] != 1)
      MatPosiciones[1][2]=2;
      posiciontotal=26;
   }
      
   If ((A_Circulo == 0) && (B_Circulo == 44))   //Posición [2][0] del tablero.
   {
      If (MatPosiciones[2][0] != 1)
      MatPosiciones[2][0]=2;
      posiciontotal=27;
   }
   If ((A_Circulo == 22) && (B_Circulo == 44))   //Posición [2][1] del tablero.
   {
      If (MatPosiciones[2][1] != 1)
      MatPosiciones[2][1]=2;
      posiciontotal=28;
   }
   If ((A_Circulo == 44) && (B_Circulo == 44))  //Posición [2][2] del tablero.
   {
      If (MatPosiciones[2][2] != 1)
      MatPosiciones[2][2]=2;
      posiciontotal=29;
   }
}


Void PosicionesPlayer2_5()
{  
  If ((C_Circulo == 0) && (D_Circulo == 0))    //Posición [0][0] del tablero.
   {
      If (MatPosiciones_5[0][0] != 1)
      MatPosiciones_5[0][0]=2;
   }
   If ((C_Circulo == 13) && (D_Circulo == 0))    //Posición [0][1] del tablero.
   {
      If (MatPosiciones_5[0][1] != 1)
      MatPosiciones_5[0][1]=2;
   }
   If ((C_Circulo == 26) && (D_Circulo == 0))   //Posición [0][2] del tablero.
   {
      If (MatPosiciones_5[0][2] != 1)
      MatPosiciones_5[0][2]=2;
   }
   If ((C_Circulo == 39) && (D_Circulo == 0))   //Posición [0][3] del tablero.
   {
      If (MatPosiciones_5[0][3] != 1)
      MatPosiciones_5[0][3]=2;
   }
   If ((C_Circulo == 52) && (D_Circulo == 0))   //Posición [0][4] del tablero.
   {
      If (MatPosiciones_5[0][4] != 1)
      MatPosiciones_5[0][4]=2;
   }
   
   If ((C_Circulo == 0) && (D_Circulo == 13))    //Posición [1][0] del tablero.
   {
      If (MatPosiciones_5[1][0] != 1)
      MatPosiciones_5[1][0]=2;
   }
   If ((C_Circulo == 13) && (D_Circulo == 13))    //Posición [1][1] del tablero.
   {
      If (MatPosiciones_5[1][1] != 1)
      MatPosiciones_5[1][1]=2;
   }
   If ((C_Circulo == 26) && (D_Circulo == 13))   //Posición [1][2] del tablero.
   {
      If (MatPosiciones_5[1][2] != 1)
      MatPosiciones_5[1][2]=2;
   }
   If ((C_Circulo == 39) && (D_Circulo == 13))   //Posición [1][3] del tablero.
   {
      If (MatPosiciones_5[1][3] != 1)
      MatPosiciones_5[1][3]=2;
   }
   If ((C_Circulo == 52) && (D_Circulo == 13))   //Posición [1][4] del tablero.
   {
      If (MatPosiciones_5[1][4] != 1)
      MatPosiciones_5[1][4]=2;
   }
   
   If ((C_Circulo == 0) && (D_Circulo == 26))    //Posición [2][0] del tablero.
   {
      If (MatPosiciones_5[2][0] != 1)
      MatPosiciones_5[2][0]=2;
   }
   If ((C_Circulo == 13) && (D_Circulo == 26))    //Posición [2][1] del tablero.
   {
      If (MatPosiciones_5[2][1] != 1)
      MatPosiciones_5[2][1]=2;
   }
   If ((C_Circulo == 26) && (D_Circulo == 26))   //Posición [2][2] del tablero.
   {
      If (MatPosiciones_5[2][2] != 1)
      MatPosiciones_5[2][2]=2;
   }
   If ((C_Circulo == 39) && (D_Circulo == 26))   //Posición [2][3] del tablero.
   {
      If (MatPosiciones_5[2][3] != 1)
      MatPosiciones_5[2][3]=2;
   }
   If ((C_Circulo == 52) && (D_Circulo == 26))   //Posición [2][4] del tablero.
   {
      If (MatPosiciones_5[2][4] != 1)
      MatPosiciones_5[2][4]=2;
   }
   
   If ((C_Circulo == 0) && (D_Circulo == 39))    //Posición [3][0] del tablero.
   {
      If (MatPosiciones_5[3][0] != 1)
      MatPosiciones_5[3][0]=2;
   }
   If ((C_Circulo == 13) && (D_Circulo == 39))    //Posición [3][1] del tablero.
   {
      If (MatPosiciones_5[3][1] != 1)
      MatPosiciones_5[3][1]=2;
   }
   If ((C_Circulo == 26) && (D_Circulo == 39))   //Posición [3][2] del tablero.
   {
      If (MatPosiciones_5[3][2] != 1)
      MatPosiciones_5[3][2]=2;
   }
   If ((C_Circulo == 39) && (D_Circulo == 39))   //Posición [3][3] del tablero.
   {
      If (MatPosiciones_5[3][3] != 1)
      MatPosiciones_5[3][3]=2;
   }
   If ((C_Circulo == 52) && (D_Circulo == 39))   //Posición [3][4] del tablero.
   {
      If (MatPosiciones_5[3][4] != 1)
      MatPosiciones_5[3][4]=2;
   }
   
   If ((C_Circulo == 0) && (D_Circulo == 52))    //Posición [4][0] del tablero.
   {
      If (MatPosiciones_5[4][0] != 1)
      MatPosiciones_5[4][0]=2;
   }
   If ((C_Circulo == 13) && (D_Circulo == 52))    //Posición [4][1] del tablero.
   {
      If (MatPosiciones_5[4][1] != 1)
      MatPosiciones_5[4][1]=2;
   }
   If ((C_Circulo == 26) && (D_Circulo == 52))   //Posición [4][2] del tablero.
   {
      If (MatPosiciones_5[4][2] != 1)
      MatPosiciones_5[4][2]=2;
   }
   If ((C_Circulo == 39) && (D_Circulo == 52))   //Posición [4][3] del tablero.
   {
      If (MatPosiciones_5[4][3] != 1)
      MatPosiciones_5[4][3]=2;
   }
   If ((C_Circulo == 52) && (D_Circulo == 52))   //Posición [4][4] del tablero.
   {
      If (MatPosiciones_5[4][4] != 1)
      MatPosiciones_5[4][4]=2;
   }
}


Void Impresion_De_JugadasX()
{
   PosicionesPlayer1();
   Int Recorrimiento_Y,Recorrimiento_X;
   //Si hay una X en posicion [0][0], entonces imprimir X
   If (MatPosiciones[0][0] == 1)    //Posición [0][0] del tablero.
   {
      for(Recorrimiento_Y = 1; Recorrimiento_Y < 18; Recorrimiento_Y++)
      {
         for(Recorrimiento_X = 1; Recorrimiento_X < 18; Recorrimiento_X++)
         {
            GLCD_Pixel(63 + Recorrimiento_X,1 + Recorrimiento_Y, X_tres[Recorrimiento_X][Recorrimiento_Y]);
         }
      }
   }
   //Si hay una X en posicion [0][1], entonces imprimir X
   If (MatPosiciones[0][1] == 1)    //Posición [0][1] del tablero.
   {
      for(Recorrimiento_Y = 1; Recorrimiento_Y < 18; Recorrimiento_Y++)
      {
         for(Recorrimiento_X = 1; Recorrimiento_X < 18; Recorrimiento_X++)
         {
            GLCD_Pixel(84 + Recorrimiento_X,1 + Recorrimiento_Y, X_tres[Recorrimiento_X][Recorrimiento_Y]);
         }
      }
   }
   //Si hay una X en posicion [0][2], entonces imprimir X
   If (MatPosiciones[0][2] == 1)   //Posición [0][2] del tablero.
   {
      for(Recorrimiento_Y = 1; Recorrimiento_Y < 18; Recorrimiento_Y++)
      {
         for(Recorrimiento_X = 1; Recorrimiento_X < 18; Recorrimiento_X++)
         {
            GLCD_Pixel(105 + Recorrimiento_X,1 + Recorrimiento_Y, X_tres[Recorrimiento_X][Recorrimiento_Y]);
         }
      }
   }
   //Si hay una X en posicion [1][0], entonces imprimir X
   If (MatPosiciones[1][0] == 1)   //Posición [1][0] del tablero.
   {
      for(Recorrimiento_Y = 1; Recorrimiento_Y < 18; Recorrimiento_Y++)
      {
         for(Recorrimiento_X = 1; Recorrimiento_X < 18; Recorrimiento_X++)
         {
            GLCD_Pixel(63 + Recorrimiento_X,23 + Recorrimiento_Y, X_tres[Recorrimiento_X][Recorrimiento_Y]);
         }
      }
   }
   //Si hay una X en posicion [1][1], entonces imprimir X
   If (MatPosiciones[1][1] == 1)   //Posición [1][1] del tablero.
   {
      for(Recorrimiento_Y = 1; Recorrimiento_Y < 18; Recorrimiento_Y++)
      {
         for(Recorrimiento_X = 1; Recorrimiento_X < 18; Recorrimiento_X++)
         {
            GLCD_Pixel(84 + Recorrimiento_X,23 + Recorrimiento_Y, X_tres[Recorrimiento_X][Recorrimiento_Y]);
         }
      }
   }
   //Si hay una X en posicion [1][2], entonces imprimir X
   If (MatPosiciones[1][2] == 1)  //Posición [1][2] del tablero.
   {
      for(Recorrimiento_Y = 1; Recorrimiento_Y < 18; Recorrimiento_Y++)
      {
         for(Recorrimiento_X = 1; Recorrimiento_X < 18; Recorrimiento_X++)
         {
            GLCD_Pixel(105 + Recorrimiento_X,23 + Recorrimiento_Y, X_tres[Recorrimiento_X][Recorrimiento_Y]);
         }
      }
   }
   //Si hay una X en posicion [2][0], entonces imprimir X
   If (MatPosiciones[2][0] == 1)   //Posición [2][0] del tablero.
   {
      for(Recorrimiento_Y = 1; Recorrimiento_Y < 18; Recorrimiento_Y++)
      {
         for(Recorrimiento_X = 1; Recorrimiento_X < 18; Recorrimiento_X++)
         {
            GLCD_Pixel(63 + Recorrimiento_X,45 + Recorrimiento_Y, X_tres[Recorrimiento_X][Recorrimiento_Y]);
         }
      }
   }
   //Si hay una X en posicion [2][1], entonces imprimir X
   If (MatPosiciones[2][1] == 1)   //Posición [2][1] del tablero.
   {
      for(Recorrimiento_Y = 1; Recorrimiento_Y < 18; Recorrimiento_Y++)
      {
         for(Recorrimiento_X = 1; Recorrimiento_X < 18; Recorrimiento_X++)
         {
            GLCD_Pixel(84 + Recorrimiento_X,45 + Recorrimiento_Y, X_tres[Recorrimiento_X][Recorrimiento_Y]);
         }
      }
   }
   //Si hay una X en posicion [2][2], entonces imprimir X
   If (MatPosiciones[2][2] == 1)  //Posición [2][2] del tablero.
   {
      for(Recorrimiento_Y = 1; Recorrimiento_Y < 18; Recorrimiento_Y++)
      {
         for(Recorrimiento_X = 1; Recorrimiento_X < 18; Recorrimiento_X++)
         {
            GLCD_Pixel(105 + Recorrimiento_X,45 + Recorrimiento_Y, X_tres[Recorrimiento_X][Recorrimiento_Y]);
         }
      }
   }
}
Void ComprobarEspacios()
{
   PosicionesPlayer1();
   //Int Recorrimiento_Y,Recorrimiento_X;
   //Si hay una X en posicion [0][0], entonces imprimir X
   If (MatPosiciones[0][0] == 1)    //Posición [0][0] del tablero.
   {
      posiciontotal=1;
   }
   //Si hay una X en posicion [0][1], entonces imprimir X
   If (MatPosiciones[0][1] == 1)    //Posición [0][1] del tablero.
   {
      posiciontotal=2;
   }
   //Si hay una X en posicion [0][2], entonces imprimir X
   If (MatPosiciones[0][2] == 1)   //Posición [0][2] del tablero.
   {
      posiciontotal=3;
   }
   //Si hay una X en posicion [1][0], entonces imprimir X
   If (MatPosiciones[1][0] == 1)   //Posición [1][0] del tablero.
   {
      posiciontotal=4;
   }
   //Si hay una X en posicion [1][1], entonces imprimir X
   If (MatPosiciones[1][1] == 1)   //Posición [1][1] del tablero.
   {
      posiciontotal=5;
   }
   //Si hay una X en posicion [1][2], entonces imprimir X
   If (MatPosiciones[1][2] == 1)  //Posición [1][2] del tablero.
   {
      posiciontotal=6;
   }
   //Si hay una X en posicion [2][0], entonces imprimir X
   If (MatPosiciones[2][0] == 1)   //Posición [2][0] del tablero.
   {
      posiciontotal=7;
   }
   //Si hay una X en posicion [2][1], entonces imprimir X
   If (MatPosiciones[2][1] == 1)   //Posición [2][1] del tablero.
   {
     posiciontotal=8;
   }
   //Si hay una X en posicion [2][2], entonces imprimir X
   If (MatPosiciones[2][2] == 1)  //Posición [2][2] del tablero.
   {
      posiciontotal=9;
   }
}


Void Impresion_De_JugadasX_5()
{
   //Funcion para determinar si hay una X o un 0
   PosicionesPlayer1_5();
   Int Recorrimiento_Y_5,Recorrimiento_X_5;
   If (MatPosiciones_5[0][0] == 1)    //Posición [0][0] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(63 + Recorrimiento_X_5,1 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   If (MatPosiciones_5[0][1] == 1)    //Posición [0][1] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(76 + Recorrimiento_X_5,1 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   If (MatPosiciones_5[0][2] == 1)    //Posición [0][2] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(89 + Recorrimiento_X_5,1 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   If (MatPosiciones_5[0][3] == 1)    //Posición [0][3] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(102 + Recorrimiento_X_5,1 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   If (MatPosiciones_5[0][4] == 1)    //Posición [0][4] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(115 + Recorrimiento_X_5,1 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   
   
   If (MatPosiciones_5[1][0] == 1)    //Posición [1][0] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(63 + Recorrimiento_X_5,14 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   If (MatPosiciones_5[1][1] == 1)    //Posición [1][1] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(76 + Recorrimiento_X_5,14 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   If (MatPosiciones_5[1][2] == 1)    //Posición [1][2] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(89 + Recorrimiento_X_5,14 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   If (MatPosiciones_5[1][3] == 1)    //Posición [1][3] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(102 + Recorrimiento_X_5,14 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   If (MatPosiciones_5[1][4] == 1)    //Posición [1][4] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(115 + Recorrimiento_X_5,14 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   
   
   If (MatPosiciones_5[2][0] == 1)    //Posición [2][0] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(63 + Recorrimiento_X_5,1 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   If (MatPosiciones_5[2][1] == 1)    //Posición [2][1] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(76 + Recorrimiento_X_5,27 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   If (MatPosiciones_5[2][2] == 1)    //Posición [2][2] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(89 + Recorrimiento_X_5,27 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   If (MatPosiciones_5[2][3] == 1)    //Posición [2][3] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(102 + Recorrimiento_X_5,27 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   If (MatPosiciones_5[2][4] == 1)    //Posición [2][4] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(115 + Recorrimiento_X_5,27 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   
   
   If (MatPosiciones_5[3][0] == 1)    //Posición [3][0] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(63 + Recorrimiento_X_5,40 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   If (MatPosiciones_5[3][1] == 1)    //Posición [3][1] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(76 + Recorrimiento_X_5,40 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   If (MatPosiciones_5[3][2] == 1)    //Posición [3][2] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(89 + Recorrimiento_X_5,40 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   If (MatPosiciones_5[3][3] == 1)    //Posición [3][3] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(6102 + Recorrimiento_X_5,40 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   If (MatPosiciones_5[3][4] == 1)    //Posición [3][4] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(115 + Recorrimiento_X_5,40 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   
   
   If (MatPosiciones_5[4][0] == 1)    //Posición [4][0] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(63 + Recorrimiento_X_5,53 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   If (MatPosiciones_5[4][1] == 1)    //Posición [4][1] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(76 + Recorrimiento_X_5,53 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   If (MatPosiciones_5[4][2] == 1)    //Posición [4][2] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(89 + Recorrimiento_X_5,53 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   If (MatPosiciones_5[4][3] == 1)    //Posición [4][3] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(102 + Recorrimiento_X_5,53 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
   If (MatPosiciones_5[4][4] == 1)    //Posición [4][4] del tablero.
   {
      for(Recorrimiento_Y_5 = 0; Recorrimiento_Y_5 < 10; Recorrimiento_Y_5++)
      {
         for(Recorrimiento_X_5 = 0; Recorrimiento_X_5 < 10; Recorrimiento_X_5++)
         {
            GLCD_Pixel(115 + Recorrimiento_X_5,53 + Recorrimiento_Y_5, X_Cinco[Recorrimiento_X_5][Recorrimiento_Y_5]);
         }
      }
   }
}

Void Impresion_De_JugadasO()
{
   PosicionesPlayer2();
   //Si hay un 0 en la posicion [0][0], imprimir 0
   If (MatPosiciones[0][0] == 2)    //Posición [0][0] del tablero.
   {
      GLCD_Circle (73, 11, 7, NO, ON);
   }
   If (MatPosiciones[0][1] == 2)    //Posición [0][1] del tablero.
   {
      GLCD_Circle (95, 11, 7, NO, ON);
   }
   If (MatPosiciones[0][2] == 2)   //Posición [0][2] del tablero.
   {
      GLCD_Circle (117, 11, 7, NO, ON);
   }
   
   If (MatPosiciones[1][0] == 2)   //Posición [1][0] del tablero.
   {
      GLCD_Circle (73, 33, 7, NO, ON);
   }
   If (MatPosiciones[1][1] == 2)   //Posición [1][1] del tablero.
   {
      GLCD_Circle (95, 33, 7, NO, ON);
   }
   If (MatPosiciones[1][2] == 2)  //Posición [1][2] del tablero.
   {
      GLCD_Circle (117, 33, 7, NO, ON);
   }
      
   If (MatPosiciones[2][0] == 2)   //Posición [2][0] del tablero.
   {
      GLCD_Circle (73, 55, 7, NO, ON);
   }
   If (MatPosiciones[2][1] == 2)   //Posición [2][1] del tablero.
   {
      GLCD_Circle (95, 55, 7, NO, ON);
   }
   If (MatPosiciones[2][2] == 2)  //Posición [2][2] del tablero.
   {
      GLCD_Circle (117, 55, 7, 0, ON);
   }
}

Void Impresion_De_JugadasO_5()
{
   PosicionesPlayer2_5();
   If (MatPosiciones_5[0][0] == 2)    //Posición [0][0] del tablero.
   {
      GLCD_Circle (69, 6, 4, NO, ON);
   }
   If (MatPosiciones_5[0][1] == 2)    //Posición [0][1] del tablero.
   {
      GLCD_Circle (82, 6, 4, NO, ON);
   }
   If (MatPosiciones_5[0][2] == 2)    //Posición [0][2] del tablero.
   {
      GLCD_Circle (95, 6, 4, NO, ON);
   }
   If (MatPosiciones_5[0][3] == 2)    //Posición [0][3] del tablero.
   {
      GLCD_Circle (108, 6, 4, NO, ON);
   }
   If (MatPosiciones_5[0][4] == 2)    //Posición [0][4] del tablero.
   {
      GLCD_Circle (121, 6, 4, NO, ON);
   }
   
   
   If (MatPosiciones_5[1][0] == 2)    //Posición [1][0] del tablero.
   {
      GLCD_Circle (69, 19, 4, NO, ON);
   }
   If (MatPosiciones_5[1][1] == 2)    //Posición [1][1] del tablero.
   {
      GLCD_Circle (82, 19, 4, NO, ON);
   }
   If (MatPosiciones_5[1][2] == 2)    //Posición [1][2] del tablero.
   {
      GLCD_Circle (95, 19, 4, NO, ON);
   }
   If (MatPosiciones_5[1][3] == 2)    //Posición [1][3] del tablero.
   {
      GLCD_Circle (108, 19, 4, NO, ON);
   }
   If (MatPosiciones_5[1][4] == 2)    //Posición [1][4] del tablero.
   {
      GLCD_Circle (121, 19, 4, NO, ON);
   }
   
   
   If (MatPosiciones_5[2][0] == 2)    //Posición [2][0] del tablero.
   {
      GLCD_Circle (69, 32, 4, NO, ON);
   }
   If (MatPosiciones_5[2][1] == 2)    //Posición [2][1] del tablero.
   {
      GLCD_Circle (82, 32, 4, NO, ON);
   }
   If (MatPosiciones_5[2][2] == 2)    //Posición [2][2] del tablero.
   {
     GLCD_Circle (95, 32, 4, NO, ON);
   }
   If (MatPosiciones_5[2][3] == 2)    //Posición [2][3] del tablero.
   {
      GLCD_Circle (108, 32, 4, NO, ON);
   }
   If (MatPosiciones_5[2][4] == 2)    //Posición [2][4] del tablero.
   {
      GLCD_Circle (121, 32, 4, NO, ON);
   }
   
   
   If (MatPosiciones_5[3][0] == 2)    //Posición [3][0] del tablero.
   {
      GLCD_Circle (69, 45, 4, NO, ON);
   }
   If (MatPosiciones_5[3][1] == 2)    //Posición [3][1] del tablero.
   {
      GLCD_Circle (82, 45, 4, NO, ON);
   }
   If (MatPosiciones_5[3][2] == 2)    //Posición [3][2] del tablero.
   {
      GLCD_Circle (95, 45, 4, NO, ON);
   }
   If (MatPosiciones_5[3][3] == 2)    //Posición [3][3] del tablero.
   {
      GLCD_Circle (108, 45,4, NO, ON);
   }
   If (MatPosiciones_5[3][4] == 2)    //Posición [3][4] del tablero.
   {
      GLCD_Circle (121, 45, 4, NO, ON);
   }
   
   
   If (MatPosiciones_5[4][0] == 2)    //Posición [4][0] del tablero.
   {
      GLCD_Circle (69, 58, 4, NO, ON);
   }
   If (MatPosiciones_5[4][1] == 2)    //Posición [4][1] del tablero.
   {
      GLCD_Circle (82, 58, 4, NO, ON);
   }
   If (MatPosiciones_5[4][2] == 2)    //Posición [4][2] del tablero.
   {
      GLCD_Circle (95, 58, 4, NO, ON);
   }
   If (MatPosiciones_5[4][3] == 2)    //Posición [4][3] del tablero.
   {
      GLCD_Circle (108, 58, 4, NO, ON);
   }
   If (MatPosiciones_5[4][4] == 2)    //Posición [4][4] del tablero.
   {
      GLCD_Circle (121, 58, 4, NO, ON);
   }
}


//Funcion para determinar el ganador en un plano 3x3
Void Ganador_Tres()
{
   //Caso 1
   // x x x
   // - - -
   // - - -
   If ((MatPosiciones[0][0] == 1) && (MatPosiciones[0][1] == 1) && (MatPosiciones[0][2] == 1)) //(0,0), (0,1), (0,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   //Caso 2
   // - - -
   // x x x 
   // - - -
   If ((MatPosiciones[1][0] == 1) && (MatPosiciones[1][1] == 1) && (MatPosiciones[1][2] == 1)) //(1,0), (1,1), (1,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   //Caso 3
   // - - -
   // - - -
   // x x x
   If ((MatPosiciones[2][0] == 1) && (MatPosiciones[2][1] == 1) && (MatPosiciones[2][2] == 1)) //(2,0), (2,1), (2,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   //Caso 4
   // x - -
   // x - -
   // x - - 
   If ((MatPosiciones[0][0] == 1) && (MatPosiciones[1][0] == 1) && (MatPosiciones[2][0] == 1))
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   //Caso 5
   // - x -
   // - x -
   // - x -
   If ((MatPosiciones[0][1] == 1) && (MatPosiciones[1][1] == 1) && (MatPosiciones[2][1] == 1))
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   //Caso 6
   // - - x
   // - - x
   // - - x
   If ((MatPosiciones[0][2] == 1) && (MatPosiciones[1][2] == 1) && (MatPosiciones[2][2] == 1))
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   //Caso 7
   // x - -
   // - x -
   // - - x
   If ((MatPosiciones[0][0] == 1) && (MatPosiciones[1][1] == 1) && (MatPosiciones[2][2] == 1))
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   } 
   //Caso 8
   // - - x
   // - x -
   // x - - 
   If ((MatPosiciones[0][2] == 1) && (MatPosiciones[1][1] == 1) && (MatPosiciones[2][0] == 1))
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   
   //Caso 9
   // 0 0 0
   // - - - 
   // - - -
   If ((MatPosiciones[0][0] == 2) && (MatPosiciones[0][1] == 2) && (MatPosiciones[0][2] == 2)) //(0,0), (0,1), (0,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   //Caso 10
   // - - -
   // 0 0 0 
   // - - -
   If ((MatPosiciones[1][0] == 2) && (MatPosiciones[1][1] == 2) && (MatPosiciones[1][2] == 2)) //(1,0), (1,1), (1,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   //Caso 11
   // - - -
   // - - -
   // 0 0 0
   If ((MatPosiciones[2][0] == 2) && (MatPosiciones[2][1] == 2) && (MatPosiciones[2][2] == 2)) //(2,0), (2,1), (2,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   //Caso 12
   // 0 - -
   // 0 - -
   // 0 - -
   If ((MatPosiciones[0][0] == 2) && (MatPosiciones[1][0] == 2) && (MatPosiciones[2][0] == 2))
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   //Caso 13
   // - 0 -
   // - 0 - 
   // - 0 -
   If ((MatPosiciones[0][1] == 2) && (MatPosiciones[1][1] == 2) && (MatPosiciones[2][1] == 2))
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   //Caso 14
   // - - 0
   // - - 0
   // - - 0
   If ((MatPosiciones[0][2] == 2) && (MatPosiciones[1][2] == 2) && (MatPosiciones[2][2] == 2))
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   //Caso 15
   // 0 - -
   // - 0 -
   // - - 0
   If ((MatPosiciones[0][0] == 2) && (MatPosiciones[1][1] == 2) && (MatPosiciones[2][2] == 2))
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   //Caso 16
   // - - 0
   // - 0 -
   // 0 - -
   If ((MatPosiciones[0][2] == 2) && (MatPosiciones[1][1] == 2) && (MatPosiciones[2][0] == 2))
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
}
Void Ganador_Cinco()
{
   If ((MatPosiciones_5[0][0] == 1) && (MatPosiciones_5[0][1] == 1) && (MatPosiciones_5[0][2] == 1)) //(0,0), (0,1), (0,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[0][0] == 2) && (MatPosiciones_5[0][1] == 2) && (MatPosiciones_5[0][2] == 2)) //(0,0), (0,1), (0,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[0][1] == 1) && (MatPosiciones_5[0][2] == 1) && (MatPosiciones_5[0][3] == 1)) //(0,1), (0,2), (0,3)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[0][1] == 2) && (MatPosiciones_5[0][2] == 2) && (MatPosiciones_5[0][3] == 2)) //(0,1), (0,2), (0,3)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[0][2] == 1) && (MatPosiciones_5[0][3] == 1) && (MatPosiciones_5[0][4] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[0][2] == 2) && (MatPosiciones_5[0][3] == 2) && (MatPosiciones_5[0][4] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   
   If ((MatPosiciones_5[1][0] == 1) && (MatPosiciones_5[1][1] == 1) && (MatPosiciones_5[1][2] == 1)) //(1,0), (1,1), (1,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[1][0] == 2) && (MatPosiciones_5[1][1] == 2) && (MatPosiciones_5[1][2] == 2)) //(1,0), (1,1), (1,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[1][1] == 1) && (MatPosiciones_5[1][2] == 1) && (MatPosiciones_5[1][3] == 1)) //(1,1), (1,2), (1,3)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[1][1] == 2) && (MatPosiciones_5[1][2] == 2) && (MatPosiciones_5[1][3] == 2)) //(1,1), (1,2), (1,3)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[1][2] == 1) && (MatPosiciones_5[1][3] == 1) && (MatPosiciones_5[1][4] == 1)) //(1,2), (1,3), (1,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[1][2] == 2) && (MatPosiciones_5[1][3] == 2) && (MatPosiciones_5[1][4] == 2)) //(1,2), (1,3), (1,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   
   If ((MatPosiciones_5[2][0] == 1) && (MatPosiciones_5[2][1] == 1) && (MatPosiciones_5[2][2] == 1)) //(2,0), (2,1), (2,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[2][0] == 2) && (MatPosiciones_5[2][1] == 2) && (MatPosiciones_5[2][2] == 2)) //(2,0), (2,1), (2,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[2][1] == 1) && (MatPosiciones_5[2][2] == 1) && (MatPosiciones_5[2][3] == 1)) //(2,1), (2,2), (2,3)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[2][1] == 2) && (MatPosiciones_5[2][2] == 2) && (MatPosiciones_5[2][3] == 2)) //(2,1), (2,2), (2,3)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[2][2] == 1) && (MatPosiciones_5[2][3] == 1) && (MatPosiciones_5[2][4] == 1)) //(2,2), (2,3), (2,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[2][2] == 2) && (MatPosiciones_5[2][3] == 2) && (MatPosiciones_5[2][4] == 2)) //(2,2), (2,3), (2,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   
   If ((MatPosiciones_5[3][0] == 1) && (MatPosiciones_5[3][1] == 1) && (MatPosiciones_5[3][2] == 1)) //(3,0), (3,1), (3,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[3][0] == 2) && (MatPosiciones_5[3][1] == 2) && (MatPosiciones_5[3][2] == 2)) //(3,0), (3,1), (3,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[3][1] == 1) && (MatPosiciones_5[3][2] == 1) && (MatPosiciones_5[3][3] == 1)) //(3,1), (3,2), (3,3)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[3][1] == 2) && (MatPosiciones_5[3][2] == 2) && (MatPosiciones_5[3][3] == 2)) //(3,1), (3,2), (3,3)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[3][2] == 1) && (MatPosiciones_5[3][3] == 1) && (MatPosiciones_5[3][4] == 1)) //(3,2), (3,3), (3,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[3][2] == 2) && (MatPosiciones_5[3][3] == 2) && (MatPosiciones_5[3][4] == 2)) //(3,2), (3,3), (3,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
  
  
   If ((MatPosiciones_5[4][0] == 1) && (MatPosiciones_5[4][1] == 1) && (MatPosiciones_5[4][2] == 1)) //(4,0), (4,1), (4,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[4][0] == 2) && (MatPosiciones_5[4][1] == 2) && (MatPosiciones_5[4][2] == 2)) //(4,0), (4,1), (4,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[4][1] == 1) && (MatPosiciones_5[4][2] == 1) && (MatPosiciones_5[4][3] == 1)) //(4,0), (4,1), (4,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[4][1] == 2) && (MatPosiciones_5[4][2] == 2) && (MatPosiciones_5[4][3] == 2)) //(4,0), (4,1), (4,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[4][2] == 1) && (MatPosiciones_5[4][3] == 1) && (MatPosiciones_5[4][4] == 1)) //(4,0), (4,1), (4,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[4][2] == 2) && (MatPosiciones_5[4][3] == 2) && (MatPosiciones_5[4][4] == 2)) //(4,0), (4,1), (4,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
  
  
  
   If ((MatPosiciones_5[0][0] == 1) && (MatPosiciones_5[1][0] == 1) && (MatPosiciones_5[2][0] == 1)) //(0,0), (1,0), (2,0)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[0][0] == 2) && (MatPosiciones_5[1][0] == 2) && (MatPosiciones_5[2][0] == 2)) //(0,0), (1,0), (2,0)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[1][0] == 1) && (MatPosiciones_5[2][0] == 1) && (MatPosiciones_5[3][0] == 1)) //(0,1), (0,2), (0,3)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[1][0] == 2) && (MatPosiciones_5[2][0] == 2) && (MatPosiciones_5[3][0] == 2)) //(0,1), (0,2), (0,3)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
  
   If ((MatPosiciones_5[2][0] == 1) && (MatPosiciones_5[3][0] == 1) && (MatPosiciones_5[4][0] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[2][0] == 2) && (MatPosiciones_5[3][0] == 2) && (MatPosiciones_5[4][0] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }


   If ((MatPosiciones_5[0][1] == 1) && (MatPosiciones_5[1][1] == 1) && (MatPosiciones_5[2][1] == 1)) //(0,0), (0,1), (0,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[0][1] == 2) && (MatPosiciones_5[1][1] == 2) && (MatPosiciones_5[2][1] == 2)) //(0,0), (0,1), (0,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[1][1] == 1) && (MatPosiciones_5[2][1] == 1) && (MatPosiciones_5[3][1] == 1)) //(0,1), (0,2), (0,3)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[1][1] == 2) && (MatPosiciones_5[2][1] == 2) && (MatPosiciones_5[3][1] == 2)) //(0,1), (0,2), (0,3)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
  
   If ((MatPosiciones_5[2][1] == 1) && (MatPosiciones_5[3][1] == 1) && (MatPosiciones_5[4][1] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[2][1] == 2) && (MatPosiciones_5[3][1] == 2) && (MatPosiciones_5[4][1] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   
   If ((MatPosiciones_5[0][2] == 1) && (MatPosiciones_5[1][2] == 1) && (MatPosiciones_5[2][2] == 1)) //(0,0), (0,1), (0,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[0][2] == 2) && (MatPosiciones_5[1][2] == 2) && (MatPosiciones_5[2][2] == 2)) //(0,0), (0,1), (0,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[1][2] == 1) && (MatPosiciones_5[2][2] == 1) && (MatPosiciones_5[3][2] == 1)) //(0,1), (0,2), (0,3)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[1][2] == 2) && (MatPosiciones_5[2][2] == 2) && (MatPosiciones_5[3][2] == 2)) //(0,1), (0,2), (0,3)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
  
   If ((MatPosiciones_5[2][2] == 1) && (MatPosiciones_5[3][2] == 1) && (MatPosiciones_5[4][2] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[2][2] == 2) && (MatPosiciones_5[3][2] == 2) && (MatPosiciones_5[4][2] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   
   If ((MatPosiciones_5[0][3] == 1) && (MatPosiciones_5[1][3] == 1) && (MatPosiciones_5[2][3] == 1)) //(0,0), (0,1), (0,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[0][3] == 2) && (MatPosiciones_5[1][3] == 2) && (MatPosiciones_5[2][3] == 2)) //(0,0), (0,1), (0,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[1][3] == 1) && (MatPosiciones_5[2][3] == 1) && (MatPosiciones_5[3][3] == 1)) //(0,1), (0,2), (0,3)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[1][3] == 2) && (MatPosiciones_5[2][3] == 2) && (MatPosiciones_5[3][3] == 2)) //(0,1), (0,2), (0,3)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
  
   If ((MatPosiciones_5[2][3] == 1) && (MatPosiciones_5[3][3] == 1) && (MatPosiciones_5[4][3] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[2][3] == 2) && (MatPosiciones_5[3][3] == 2) && (MatPosiciones_5[4][3] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   
   If ((MatPosiciones_5[0][4] == 1) && (MatPosiciones_5[1][4] == 1) && (MatPosiciones_5[2][4] == 1)) //(0,0), (0,1), (0,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[0][4] == 2) && (MatPosiciones_5[1][4] == 2) && (MatPosiciones_5[2][4] == 2)) //(0,0), (0,1), (0,2)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[1][4] == 1) && (MatPosiciones_5[2][4] == 1) && (MatPosiciones_5[3][4] == 1)) //(0,1), (0,2), (0,3)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[1][4] == 2) && (MatPosiciones_5[2][4] == 2) && (MatPosiciones_5[3][4] == 2)) //(0,1), (0,2), (0,3)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
  
   If ((MatPosiciones_5[2][4] == 1) && (MatPosiciones_5[3][4] == 1) && (MatPosiciones_5[4][4] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[2][4] == 2) && (MatPosiciones_5[3][4] == 2) && (MatPosiciones_5[4][4] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   
   //--------------------------------------------------------------------------------------------------------------------//
   
   
   If ((MatPosiciones_5[0][0] == 1) && (MatPosiciones_5[1][1] == 1) && (MatPosiciones_5[2][2] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[0][0] == 2) && (MatPosiciones_5[1][1] == 2) && (MatPosiciones_5[2][2] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[1][1] == 1) && (MatPosiciones_5[2][2] == 1) && (MatPosiciones_5[3][3] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[1][1] == 2) && (MatPosiciones_5[2][2] == 2) && (MatPosiciones_5[3][3] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[2][2] == 1) && (MatPosiciones_5[3][3] == 1) && (MatPosiciones_5[4][4] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[2][2] == 2) && (MatPosiciones_5[3][3] == 2) && (MatPosiciones_5[4][4] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   
   If ((MatPosiciones_5[0][4] == 1) && (MatPosiciones_5[1][3] == 1) && (MatPosiciones_5[2][2] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[0][4] == 2) && (MatPosiciones_5[1][3] == 2) && (MatPosiciones_5[2][2] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[1][3] == 1) && (MatPosiciones_5[2][2] == 1) && (MatPosiciones_5[3][1] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[1][3] == 2) && (MatPosiciones_5[2][2] == 2) && (MatPosiciones_5[3][1] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[2][2] == 1) && (MatPosiciones_5[3][1] == 1) && (MatPosiciones_5[4][0] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[2][2] == 2) && (MatPosiciones_5[3][1] == 2) && (MatPosiciones_5[4][0] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   
   If ((MatPosiciones_5[0][1] == 1) && (MatPosiciones_5[1][2] == 1) && (MatPosiciones_5[2][3] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[0][1] == 2) && (MatPosiciones_5[1][2] == 2) && (MatPosiciones_5[2][3] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[1][2] == 1) && (MatPosiciones_5[2][3] == 1) && (MatPosiciones_5[3][4] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[1][2] == 2) && (MatPosiciones_5[2][3] == 2) && (MatPosiciones_5[3][4] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[1][0] == 1) && (MatPosiciones_5[2][1] == 1) && (MatPosiciones_5[3][2] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[1][0] == 2) && (MatPosiciones_5[2][1] == 2) && (MatPosiciones_5[3][2] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   
   If ((MatPosiciones_5[2][1] == 1) && (MatPosiciones_5[3][2] == 1) && (MatPosiciones_5[4][3] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[2][1] == 2) && (MatPosiciones_5[3][2] == 2) && (MatPosiciones_5[4][3] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   
   If ((MatPosiciones_5[2][0] == 1) && (MatPosiciones_5[3][1] == 1) && (MatPosiciones_5[4][2] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[2][0] == 2) && (MatPosiciones_5[3][1] == 2) && (MatPosiciones_5[4][2] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   
   If ((MatPosiciones_5[0][2] == 1) && (MatPosiciones_5[1][3] == 1) && (MatPosiciones_5[2][4] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[0][2] == 2) && (MatPosiciones_5[1][3] == 2) && (MatPosiciones_5[2][4] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   
   If ((MatPosiciones_5[0][2] == 1) && (MatPosiciones_5[1][1] == 1) && (MatPosiciones_5[2][0] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[0][2] == 2) && (MatPosiciones_5[1][1] == 2) && (MatPosiciones_5[2][0] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[0][3] == 1) && (MatPosiciones_5[1][2] == 1) && (MatPosiciones_5[2][1] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[0][3] == 2) && (MatPosiciones_5[1][2] == 2) && (MatPosiciones_5[2][1] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[1][2] == 1) && (MatPosiciones_5[2][1] == 1) && (MatPosiciones_5[3][0] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[1][2] == 2) && (MatPosiciones_5[2][1] == 2) && (MatPosiciones_5[2][0] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[1][4] == 1) && (MatPosiciones_5[2][3] == 1) && (MatPosiciones_5[3][2] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[1][4] == 2) && (MatPosiciones_5[2][3] == 2) && (MatPosiciones_5[3][2] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[2][3] == 1) && (MatPosiciones_5[3][2] == 1) && (MatPosiciones_5[4][1] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[2][3] == 2) && (MatPosiciones_5[3][2] == 2) && (MatPosiciones_5[4][1] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   
   If ((MatPosiciones_5[2][4] == 1) && (MatPosiciones_5[3][3] == 1) && (MatPosiciones_5[4][2] == 1)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player1,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
   If ((MatPosiciones_5[2][4] == 2) && (MatPosiciones_5[3][3] == 2) && (MatPosiciones_5[4][2] == 2)) //(0,2), (0,3), (0,4)
   {
      GLCD_fillScreen(OFF);
      GLCD_Text57(5,0,Ganador,2,ON);
      GLCD_Text57(5,30,Player2,2,ON);
      Delay_ms(1000);
      GLCD_Init(OFF);
   }
}

void Buzzer(){
   output_high(PIN_B7);
   delay_ms(700);
   output_low(PIN_B7);

}
