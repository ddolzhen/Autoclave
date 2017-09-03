#include "Status.h"
#define SENDBACK cli();Serial.print(command);sei();
int tup;
int tdown;
int pdesired;
int thold; 
int a=0;
bool commandin;
char temp1, temp2;
char press1, press2;
float Temp;
String command;
Status State;
Status StateRun;
Status State2;
char runstp;
char mode; // 0 for waiting for data, 1 for manual, 2 for sterilization, 3 for pasterization, 4 for ending
int tempS;
int presS;
bool reachtempster;
bool reachtemppast1,reachtemppast2;
bool filled;
byte level1,level2,level3;
byte ctr;
byte stage;
byte timer;
bool control;
byte ctr2;
byte timer2;
bool pcontrol;

byte timcool;
byte timcool2;

bool blinker;
bool fillcool;
bool filled3;
bool startcoolcd;
byte tcoolin;
byte timin;

byte paim;
byte taim;

byte paimtemp;


byte tcheck;
void setup() {
    timer=0;
    timer2=0;
    ctr=0;
    Serial.begin(9600); //Serial Setup
    Serial.setTimeout(10);
    cli();
    StateRun.bit2=runstp;
  
//set timer0 interrupt at 60Hz
  TCCR0A = 0;// set entire TCCR0A register to 0
  TCCR0B = 0;// same for TCCR0B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  OCR0A = 255;// = (16*10^6) / (2000*64) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS01 and CS00 bits for 1024 prescaler
  TCCR0B |= (1 << CS02) | (1 << CS00);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);

    
//set timer1 interrupt 
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 1400;// = (16*10^6) / (Freq*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);





  mode=15;
  tup=255;
  tdown=255;
  thold=255;
  pdesired=255;
  timin=255;
  tcoolin=255;

    State.bit1=1;
    State.bit2=0;
    State.bit3=1;
    State.bit4=0;
    State.bit5=0;
    State.bit6=0;
    State.bit8=0;
    State2.bit4=0;

  pinMode(3, OUTPUT); // LED

  pinMode(22, OUTPUT); // valve1
  pinMode(23, OUTPUT);
  pinMode(24, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT); //valve6
  pinMode(28, OUTPUT); // motor
  pinMode(29, OUTPUT); // burner

  pinMode(50, INPUT_PULLUP); // Level1
  pinMode(51, INPUT_PULLUP); // Level2
  pinMode(52, INPUT_PULLUP); // Level3
    
  
  press1=65;
  press2=66;
  temp1=88;
  temp2=89;
  runstp=0;
  sei();
  //State2.bit4=0;
}
 
void loop() {


  if(runstp)
  {
    if (mode==2||mode==3||mode==4)
    {
      if(!filled)
      {
        //Fill up the system
        State.bit2=1;
        State.bit1=0;
        State.bit3=0;
        State.bit4=0;
        State.bit5=0;
        State.bit6=0;
        State2.bit4=0;
        State.bit8=0;
      }else
      {
        
        if(control)
        {
          
          if ((taim-tempS)>2)
          {
            State.bit3=1;//Heat up
            State.bit1=0;
            State.bit6=0;
            State.bit8=1;
            State2.bit4=1;
          }else
          {
            State.bit3=1;
            State.bit1=0;// Circulate
            State.bit6=0;
            State.bit8=1;
            State2.bit4=0;
          }
        }
        if(pcontrol)
        {
          if (((mode==4?20:paim)-presS)>1)
          {
            State.bit4=1;
            State.bit5=0;
          }else if ((presS-(mode==4?20:paim))>1)
          {
            State.bit4=0;
            State.bit5=1;
          }else
          {
            State.bit4=0;
            State.bit5=0;
          }
        }
        //Do the rest
      }
    }
  }


  if ((mode==2||mode==3)&&runstp&&(reachtempster||stage==20)&&(!timer)&&(!timer2)&&control)//Time up, start cooling
  {
    control=0;
    fillcool=1;
    State.bit1=1;
    State.bit3=0;
    filled3=0;
    stage=2;
  }

  if(!filled&&State.bit7&&State2.bit2&&runstp&&(mode==2||mode==3))//start heat
  {
    filled=1;
    control=1;
    pcontrol=1;
    State.bit2=0;
    startcoolcd=0;
    stage=88;
  }

  if (stage==2&&runstp)// cooling
  {
    State.bit6=0;
    if (!State2.bit3)
    {
      State.bit2=1;
    }else
    {
      State.bit2=0;
    }
    State.bit8=1;
    
    State.bit1=1;
    State.bit3=0;
  }

  if (fillcool&&State2.bit3&&stage==2&&runstp)
  {
    fillcool=0;
    startcoolcd=1;
  }

  if (!fillcool&&runstp&&stage==2&&startcoolcd&&runstp)
  {
    startcoolcd=0;
    timcool=timin;
    timcool2=0;
  }


  if(!fillcool&&runstp&&stage==2&&!timcool&&!timcool2&&runstp)
  {
    tcheck=tempS;
    stage=10;
  }

  if(stage==10&&State2.bit2&&runstp)
  {
    State.bit6=1;
  }
  
  if(stage==10&&State2.bit2==0&&runstp)
  {
    if(tcheck>tcoolin+3)
    {
      stage=2;
      fillcool=1;
    }else
    {
      stage=99;
      
    }
    
  }


  if (stage==99&&runstp)
  {
    State.bit1=1;
    State.bit2=0;
    State.bit3=0;
    State.bit8=0;
    State.bit6=1;
    State.bit5=0; 
    if(!State.bit7)
    {
      stage=101;
      pcontrol=0;
      
    }   
  }
  if (stage==101&&runstp)
  {
    State.bit6=0;
    State.bit5=1;

    if (presS<5)
    {
      stage=100;
    }
  }
  if (stage==100)
  {
    runstp=0;
    StateRun.bit2=0;
    State.bit1=1;
    State.bit2=0;
    State.bit3=1;
    State.bit4=0;
    State.bit5=0;
    State.bit6=0;
    State.bit8=0;
    State2.bit4=0;
    reachtempster=0;
    reachtemppast1=0;
    reachtemppast2=0;
    filled=0;
    stage=0;
    control=0;
    pcontrol=0;
    timer=thold;
    timer2=0;
  }


  
  if (runstp&&filled && (taim-tempS)<=2 && (tempS-taim)<=2 && (paim-presS)<=1 && (presS-paim)<=1&&(mode==2||mode==3))
  {
    if(mode==2)
    {
    reachtempster=1;
    }else if(mode==3)
    {
      if(stage==88)
      {
        stage=15;
        taim=tdown;
      }else if (stage==15)
      {
        stage=20;
      }
    }
  }
  


  if (Serial.available()>=5)// Read Command from the PC
  {
    command=Serial.readString();
    if (command.length()>5)
    {
      command.remove(0,command.length()-5);
    }
    commandin=1;
        
  }






  

  if (commandin)
  {
    commandin=0;
     if(command.charAt(1)=='A'&&command.charAt(0)=='A'&&command.charAt(2)=='A'&&command.charAt(4)=='\n'&&command.charAt(3)=='A')//LED ON
    {
       digitalWrite(3,HIGH);
    }else if(command.charAt(1)=='B'&&command.charAt(0)=='B'&&command.charAt(2)=='B'&&command.charAt(4)=='\n'&&command.charAt(3)=='B')//LED OFF
    {
      digitalWrite(3,LOW);
    }else if(command.charAt(0)=='p')//Pasterization Message 1
    {
      tup=command.charAt(1);
      tdown=command.charAt(2);
      pdesired=command.charAt(3);
      mode=3;
      SENDBACK
    }else if(command.charAt(0)=='q')// Pasterization Message 2
    {
      thold=command.charAt(1);
      StateRun.bit3=1;
      StateRun.bit4=1;
      StateRun.bit5=0;
      SENDBACK
      reachtempster=0;
      reachtemppast1=0;
      reachtemppast2=0;
      filled=0;
      stage=0;
      control=0;
      pcontrol=0;
      timer=thold;
      timer2=0;
      fillcool=0;
      if(pdesired==51)
      {
        pdesired=0;
      }else if (pdesired==52)
      {
        pdesired=10;
      }
      if (thold==121)
      {
        thold=0;
      }else if (thold==122)
      {
        thold=10;
      }
      
      taim=tup;
      paim=pdesired;
    }else if(command.charAt(0)=='s')//Sterilization
    {
      mode=2;
      thold=command.charAt(1);
      pdesired=command.charAt(2);
      tup=command.charAt(3);
      StateRun.bit3=0;
      StateRun.bit4=1;
      StateRun.bit5=0;
      SENDBACK
      reachtempster=0;
      reachtemppast1=0;
      reachtemppast2=0;
      pcontrol=0;
      filled=0;
      stage=0;
      control=0;
      timer=thold;
      timer2=0;
      fillcool=0;
      if(pdesired==51)
      {
        pdesired=0;
      }else if (pdesired==52)
      {
        pdesired=10;
      }
      if (thold==121)
      {
        thold=0;
      }else if (thold==122)
      {
        thold=10;
      }
      
      paim=pdesired;
      taim=tup;
    }else if(command.charAt(0)=='R') // Run/Pause/Stop command
    {
      if (command.charAt(1)=='R')
      {
        if ((mode!=15&&tup!=255&&thold!=255&&pdesired!=255&&timin!=255&&tcoolin!=255)||(mode==4&&timin!=255&&tcoolin!=255))
        {
          runstp=1;
        }else if (timin==255||tcoolin==255)
        {
          Serial.write("+a++\n\0");
        }else
        {
          Serial.write("+b++\n\0");
          
        }
        
      }else if (command.charAt(1)=='P')
      {
        runstp=0;
        State.bit1=1;
        State.bit2=0;
        State.bit3=1;
        State.bit4=0;
        State.bit5=0;
        State.bit6=0;
        State.bit8=0;
        State2.bit4=0;
      }else if (command.charAt(1)=='S')
      {
        mode=4;
        runstp=0;
        StateRun.bit3=0;
        StateRun.bit4=0;
        StateRun.bit5=1;
        reachtempster=1;
        reachtemppast1=1;
        reachtemppast2=1;
        filled=1;
        stage=2;
        control=0;
        pcontrol=1;
        
        
        timer=thold;
        timer2=0;
        fillcool=1;
      }
      StateRun.bit2=runstp;
      
      SENDBACK
    }else if(command.charAt(0)=='V')
    {
      if (command.charAt(2)=='8')
      {
        State2.setBit(3,command.charAt(1)-48);
      }else
      {
      State.setBit(command.charAt(2)-48,command.charAt(1)-48);
      }
    }else if (command.charAt(0)=='c')
    {
      SENDBACK
      timin=command.charAt(2);
      tcoolin=command.charAt(1);
      if(tcoolin==100)
      {
        tcoolin=10;
      }
    }
    
  }
  
}

ISR(TIMER1_COMPA_vect){//Sends Temperature and Pressure to the PC
a=(a+1)%4;

temp1=tempS/10;
temp2=(tempS%10)*10;
press1=presS/100;
press2=presS%100;

if (a==0)
{
  if (press1==10)
  {
    press1=101;
  }else if (press1==0)
  {
    press1=102;
  }
  if (press2==10)
  {
    press2=101;
  }else if (press2==0)
  {
    press2=102;
  }
  Serial.write('P');
  Serial.write(press1);
  Serial.write(press2);
  Serial.write(press2);
  Serial.write('\n');
}else if (a==1)
{
  if (temp1==10)
  {
    temp1=101;
  }else if (temp1==0)
  {
    temp1=102;
  }
  if (temp2==10)
  {
    temp2=101;
  }else if (temp2==0)
  {
    temp2=102;
  }
  Serial.write('T');
  Serial.write(temp1);
  Serial.write(temp2);
  Serial.write(temp2);
  Serial.write('\n');
}else if (a==2)
{
  Serial.write('S');
  Serial.write((byte)State.getByte());
  Serial.write((byte)StateRun.getByte()); 
  Serial.write((byte)State2.getByte()); 
  Serial.write('\n');
}else if(a==3)
{
  Serial.write('t');
  if (runstp)
  {
    if(mode==2)
    {
    if(!filled)
    {
      Serial.write('1');
    }else if (!reachtempster)
    {
      Serial.write('2');
    }else if (reachtempster&&(timer||timer2))
    {
      Serial.write('3');
    }else if (stage==2&&fillcool)
    {
      Serial.write('4');
    }else if (!fillcool&&(timcool||timcool2))
    {
      Serial.write('5');
    }else if (!fillcool&&(!timcool||!timcool2)&&(stage!=99))
    {
      Serial.write('6');
    }else if (stage!=100)
    {
      Serial.write('7');
    }else 
    {
      Serial.write('8');
    }
    }else if (mode==3)
    {
      if (!filled)
      {
        Serial.write('1');
      }else if (stage==88)
      {
        Serial.write('a');
      }else if (stage==15)
      {
        Serial.write('b');
      }else if (stage==20)
      {
        Serial.write('c');
      }else if (stage==2&&fillcool)
      {
        Serial.write('4');
      }else if (!fillcool&&(timcool||timcool2))
      {
        Serial.write('5');
      }else if (!fillcool&&(!timcool||!timcool2)&&(stage!=99))
      {
        Serial.write('6');
      }else if (stage!=100)
      {
        Serial.write('7');
      }else 
      {
        Serial.write('8');
      }
    }else if (mode==4)
    {
       if (stage==2&&fillcool)
      {
        Serial.write('4');
      }else if (!fillcool&&(timcool||timcool2))
      {
        Serial.write('5');
      }else if (!fillcool&&(!timcool||!timcool2)&&(stage!=99))
      {
        Serial.write('6');
      }else if (stage!=100)
      {
        Serial.write('7');
      }else 
      {
        Serial.write('8');
      }
    }
  }else
  {
    if(stage==100)
    {
      Serial.write('8');
    }else
    {   
      Serial.write('0');
    }
  }
  if (stage!=2)//!filled||!(reachtempster&&mode==2)||stage==0||stage==88||stage==15||stage==20)
  {
    if (timer==0)
    {
      Serial.write((byte)125);
    }else if (timer==10)
    {
      Serial.write((byte)126);
    }else
    {
      Serial.write(timer);
    }
    
    if (timer2==0)
    {
      Serial.write((byte)125);
    }else if (timer2==10)
    {
      Serial.write((byte)126);
    }else
    {
      Serial.write(timer2);
    }
  }else
  {
    if (timcool==0)
    {
      Serial.write((byte)125);
    }else if (timcool==10)
    {
      Serial.write((byte)126);
    }else
    {
      Serial.write(timcool);
    }
    digitalWrite(3,HIGH);
    if (timcool2==0)
    {
      Serial.write((byte)125);
    }else if (timcool2==10)
    {
      Serial.write((byte)126);
    }else
    {
      Serial.write(timcool2);
    }    
  }
    
    Serial.write('\n');    
  }
  digitalWrite(22,!State.bit1);
  digitalWrite(23,!State.bit2);
  digitalWrite(24,!State.bit3);
  digitalWrite(25,!State.bit4);
  digitalWrite(26,!State.bit5);
  digitalWrite(27,!State.bit6);
  digitalWrite(28,!State.bit8);
  digitalWrite(29,!State2.bit4);
}



ISR(TIMER0_COMPA_vect){
ctr=(ctr+1)%7;
  if(!ctr)
  {
     // Check sensors
     State.bit7=!digitalRead(50); //Levels
     State2.bit2=!digitalRead(51);
     State2.bit3=!digitalRead(52);

    tempS=analogRead(A0);
    tempS/=7;
    presS=analogRead(A2);
    presS/=20;
     
  }
ctr2=(ctr2+1)%60;

  if(!ctr2)
  {

    if (runstp && (stage==20||stage==2||reachtempster))
    {
        if(timer2!=0)
        {
          timer2--;
        }else
        {
          if (timer!=0)
          {
            timer--;
            timer2=59;
          }
        }

        if(timcool2!=0)
        {
          timcool2--;
        }else
        {
          if (timcool!=0)
          {
            timcool--;
            timcool2=59;
          }
        }


        
     }
    }

}





