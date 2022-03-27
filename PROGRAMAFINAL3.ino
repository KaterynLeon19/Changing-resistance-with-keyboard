#include <PID_v1.h>
#include <max6675.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Keypad.h>

#define Password_Lenght 5 // Give enough room for four chars + NULL char
#define LED_ON 1 
#define niquelina 10
//---------------------------------------------------------------------

//DEFINIR VARIABLES GLOBABLES

unsigned long start;

float temperatura = 0; // Variable para almacenar temperatura
double new_temp = 0;

  //Variables Teclado
int cuadro=0;
int fila=0; 
int state=1;

   // Variables Clave
char Data[Password_Lenght]; // 4 is the number of chars it can hold + the null char = 5
char Master[Password_Lenght] = "1245"; 
byte data_count = 0, master_count = 0;
bool Pass_is_good;

    //VariablesCambioTemp
    char char_tecla3;
    char char_tecla4;


//DEFINIR FUNCIONES GLOBALES

void IngreseTemperatura();
int CambioTemperatura(); 
void MostrarTemperatura();
    
//-----------------------------PANTALLA
LiquidCrystal_I2C lcd(0x3F,16,2); // Inicializamos objeto LCD de 16x2 pantalla, 0x3F es la direccion de puerto


//-----------------------------TECLADO
  const byte ROWS=4; 
  const byte COLS=4;
  //Matriz que define los valores tomados del teclado

  char hexaKeys [ROWS][COLS] = {
   { '1','2','3', 'A' },
   { '4','5','6', 'B' },
   { '7','8','9', 'C' },
   { '*','0','#', 'D' }
 
  };

const byte rowPins[ROWS] = {  6,7,8,9 };
const byte colPins[COLS] = {  2,3,4,5  };


  //Inicializar una instancia de la clase NewKeypad
  Keypad customKeypad = Keypad(makeKeymap (hexaKeys), rowPins, colPins, ROWS, COLS);

  char tecla= customKeypad.getKey();
//------------------------------TERMOCUPLA

// Definir los pines para el modulo MAX 6675
    int ktcSO = 11;
    int ktcCS = 12;
    int ktcCLK = 13;

MAX6675 ktc(ktcCLK, ktcCS, ktcSO);

//------------------------------RELE - NIQUELINA 


//------------------------------ILLIS
unsigned long previousMillis=0;
const long interval= 1000;

//VARIABLES CAMBIOTEMPERATURA()

int PulseWidth=255;

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint,2,5,1,P_ON_M, DIRECT); //P_ON_M specifies that Proportional on Measurement be used
                                                            //P_ON_E (Proportional on Error) is the default behavior
                                                            
void setup() {

    //activa comunicacion serial.
       Serial.begin(9600);
       

//----------------PANTALLA 

      lcd.init();
      lcd.backlight();
      
         lcd.print("SENSOR DE TEMPERATURA"); 
             lcd.setCursor(5,1); // Saltamos a la siguiente linea
            lcd.print ("04/05/2018"); 

    
        for (int positionCounter=0; positionCounter <13; positionCounter ++){
          lcd.scrollDisplayLeft(); 
          delay (200);
        }
        
        for (int positionCounter=0; positionCounter <29; positionCounter ++){
          lcd.scrollDisplayRight(); 
          delay (200);
        }
        
         for (int positionCounter=0; positionCounter <16; positionCounter ++){
          lcd.scrollDisplayLeft(); 
          delay (200);
        }
      
      delay(1500);
      
      analogWrite(niquelina, 0); //// VALOR INICIAL NIQUELINA 
        lcd.setCursor(0,0);
        lcd.print("                ");
        lcd.setCursor(0,1);
        lcd.print("                ");
        
 
//-----------------TECLADO

   
//-----------------TERMOCUPLA

//------------------RELE - NIQUELINA 

     pinMode(niquelina, OUTPUT);
     start = millis();

//------------------CAMBIO DE TEMPERATURA ()

//initialize the variables we're linked to
  Input = analogRead(11);
  Setpoint = 35; //Defininimos la temperatura inicial a alcanzar como 35

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  
}

void loop() {

  unsigned long currentMillis = millis();
//-----------------TECLADO

   //Declarar tecla como caracter
   char tecla= customKeypad.getKey();
  
  
 //-----------------TERMOCUPLA
      //Leer temperatura.
   //temperatura=ktc.readCelsius();
  
      
//---------------------INGRESAR DATO Y MOSTRAR PANTALLA 

if (tecla == NO_KEY){

      
       if (millis() - start >= 500UL){
           start = millis();
           MostrarTemperatura(); //Llamamos a la funcion que mostrará la temperatura en la pantalla
       }           
    }      
    else {     
            Data[data_count] = tecla; // store char into data array
           // lcd.setCursor(data_count,1); // move cursor to show each new char
            //lcd.print(Data[data_count]); // print char at said cursor
           
       
            data_count++; // increment data array by 1 to store new char, also keep track of the number of chars entered
          
        
          if(data_count == Password_Lenght-1) // if the array index is equal to the number of expected chars, compare data to master
          {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Password is ");
        
            if(!strcmp(Data, Master)) {// equal to (strcmp(Data, Master) == 0)
                lcd.print("Good");
                delay(1000);
                     
                IngreseTemperatura(); //Llamamos a la funcion IngreseTemperatura
                    
                    while(1){
                      CambioTemperatura(); // Llamamos a la funcion que caliente la niquelina
                      if (CambioTemperatura()==0) { // Sale del WHILE si el usuario lo desea
                        Serial.print("TECLA 6:");
                        break;
                      }
                    }
                    
                } else
              lcd.print("Bad");
        
            delay(1000);// added 1 second delay to make sure the password is completely shown on screen before it gets cleared.
            lcd.clear();
            clearData();   
           
          } 
          Serial.print("TEMPERATURA NEW_TEMP:");
          Serial.print(new_temp);
      }




//------------------RELE - NIQUELINA 
  
   

 }

     //DESARROLLO DE FUNCIONES
     
void clearData()
{
  while(data_count !=0)
  {   // This can be used for any array size, 
    Data[data_count--] = 0; //clear array for new data
  }
  return;
}

void MostrarTemperatura()
{         
           lcd.clear();
           double temperatura=ktc.readCelsius();
              Serial.print("Temperatura CON KEY= "); 
              Serial.print(temperatura);
           Serial.println(" C"); 
           lcd.setCursor(0, 0);
           lcd.print("Temperatura: ");
           lcd.setCursor(2, 1);
           lcd.print(temperatura);
           lcd.setCursor(9, 1);
           lcd.print("C");
              
        
  }

void  IngreseTemperatura()
{         
  lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Ingrese T: ");
       
        lcd.setCursor(0,1);
        char tecla3 = customKeypad.waitForKey(); //Bloquea la ejecucuión del sketch hasta que se pulse una tecla
        lcd.print(tecla3);
        lcd.setCursor(1,1);
        char_tecla3 = tecla3; //almaceno el char ingresado local en uno global
        
        char tecla4 = customKeypad.waitForKey(); //Bloquea la ejecucuión del sketch hasta que se pulse una tecla
        char_tecla4 = tecla4; 

        //Se resta 48 para convertir de ASCII a DECIMAL
        int temp1= (int)char_tecla3-48;
        int temp2= (int)char_tecla4-48;
        new_temp = (10*temp1)+temp2;
        
        lcd.print(tecla4);
        delay(200);
        
        lcd.clear();
        //Mover Pantalla 
        lcd.setCursor(2,0);
        lcd.print("Temperatura");
        lcd.setCursor(4,1);
        lcd.print ("Cambiada");

    //Ponemos cmo Setpoint la temperatura cambiada
      Setpoint=new_temp;
      
        delay(500);
              
       
  }

int CambioTemperatura() {
      
   while (temperatura <= new_temp){
    
//Enviar dato de temperatura por el puerto serial.
    if (millis() - start >= 500UL){
       start = millis();
       temperatura=ktc.readCelsius();

       Input = analogRead(11); // Porque el SO es de DATA INPUT 
       myPID.Compute();
       analogWrite(niquelina,Output);
       
       Serial.print(start);
       Serial.print("\t"); 
       Serial.print(temperatura);
       Serial.print("\t"); 
       Serial.println(Output);

       //MostrarTemperatura Pantalla
       MostrarTemperatura();
       lcd.setCursor(13, 1);
       lcd.print("ON");
           
       char teclaexit = customKeypad.getKey();
       
          if((int)teclaexit ==65) 
          { Serial.print("TECLA EXIT ON:");
            return 0; //se asegura que la tecla ingresada sea A en formato ASCCI */
          }
      }
   }

  
   while(temperatura>= new_temp)
    {  ; 
      PulseWidth=0;
       if (millis() - start >= 500UL){
         start = millis();
         temperatura=ktc.readCelsius();
         
         Serial.print("Temperatura = "); 
         Serial.print(start);
         Serial.print("\t"); 
         Serial.print(temperatura);
         Serial.print("\t"); 
         Serial.println(PulseWidth);
  
         analogWrite(niquelina,PulseWidth);
  
         //MostrarTemperatura Pantalla
         MostrarTemperatura();
  
         lcd.setCursor(13, 1);
         lcd.print("OFF");
         
         char teclaexit2=customKeypad.getKey();
         
          if((int)teclaexit2== 65) 
          { Serial.print("TECLA EXIT OFF:");
            return 0; //se asegura que la tecla ingresada sea A en formato ASCCI */
          }
         
        }
      }
 } 


