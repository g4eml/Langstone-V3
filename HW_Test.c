#include "Mouse.h"
#include "Touch.h"

#include <string.h>
#include <stdlib.h>
#include <lgpio.h>

int p1;
int k1;
int lastp1;
int lastk1;

#define pttPin 17        // Physical pin is 11
#define keyPin 18        //Physical pin is 12

int mousePresent;
int touchPresent;
char mousePath[20];
char touchPath[20];

int gpiohandle;

void detectHw();
void processTouch();
void processMouse(int mbut);
void initGPIO(void); 
void processGPIO(void);

int main()
{
  printf("\n\nStarting Langstone Hardware Test\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
  detectHw();
  
  if(mousePresent) 
  {
  printf("Mouse detected on %s\n",mousePath);
  initMouse(mousePath);
  }
  else
  {
  printf("ERROR:- No Mouse Detected\n");
  }
  
  if(touchPresent)
  {
  printf("Touch Screen detected on %s\n",touchPath);
  initTouch(touchPath);
  } 
    else
  {
  printf("ERROR:- No Touch Screen Detected\n");
  }


  initGPIO();
  
  printf("\n");
  if(mousePresent) printf("Looking for Mouse Scroll Wheel and Buttons\n");
  if(touchPresent) printf("Looking for Touches \n");
  printf("Looking for inputs from GPIO 17 (Pin 11) (PTT) and GPIO 18 (Pin 12) (KEY)\n");
  printf("\nTesting...... Enter <Control C> to exit\n\n");

   while(1)
    {
      if(touchPresent)
        {
          if(getTouch()==1)
            {
              processTouch();
            }
        }
        
        if(mousePresent)
         {
           int but=getMouse();
           if(but>0)
            {
             processMouse(but);
            }           
         }
      processGPIO();
    }
}


 void detectHw()
{
  FILE * fp;
  char * ln=NULL;
  size_t len=0;
  ssize_t rd;
  int p;
  char handler[2][20];
  char * found;
  p=0;
  mousePresent=0;
  touchPresent=0;
  fp=fopen("/proc/bus/input/devices","r");
   while ((rd=getline(&ln,&len,fp)!=-1))
    {
      if(ln[0]=='N')        //name of device
      {
        p=0;
        if((strstr(ln,"FT5406")!=NULL) || (strstr(ln,"pi-ts")!=NULL) || (strstr(ln,"ft5x06")!=NULL))         //Found Raspberry Pi TouchScreen entry
          {
           p=1;
          }                                                                 
      }
      
      if(ln[0]=='H')        //handlers
      {
         if(strstr(ln,"mouse")!=NULL)
         {
           found=strstr(ln,"event");
           strcpy(handler[p],found);
           handler[p][strlen(found)-2] = 0; 
           if(p==0) 
            {
              sprintf(mousePath,"/dev/input/%s",handler[0]);
              mousePresent=1;
            }
           if(p==1) 
           {
             sprintf(touchPath,"/dev/input/%s",handler[1]);
             touchPresent=1;
           }
         }
      }   
    }
  fclose(fp);
  if(ln)  free(ln);
  
}



 
void processTouch()
{ 


printf("Touch detected at X=%d Y=%d\n",touchX,touchY);
} 

void processMouse(int mbut)
{
  if(mbut==128)       //scroll whell turned 
    {
      
      if(mouseScroll>0)       printf("Scroll Wheel Clicked Clockwise\n");
      if(mouseScroll<0)       printf("Scroll Wheel Clicked Anticlockwise\n");
      mouseScroll=0;
    }
    
  if(mbut==1+128)      //Left Mouse Button down
    {
      printf("Left Mouse Button Pressed\n");    
    }
    
  if(mbut==2+128)      //Right Mouse Button down
    {
        printf("Right Mouse Button Pressed\n");         
    }
    
  if(mbut==3+128)      //Middle Mouse Button down
    {
        printf("Middle Mouse Button Pressed\n");         
    }
      
    
}
 
 

void initGPIO(void)
{
  gpiohandle = lgGpiochipOpen(4);
  lgGpioClaimInput(gpiohandle,LG_SET_PULL_UP,pttPin);
  lgGpioClaimInput(gpiohandle,LG_SET_PULL_UP,keyPin);
}

void processGPIO(void)
{

    p1=lgGpioRead(gpiohandle,pttPin);
    k1=lgGpioRead(gpiohandle,keyPin);
    if(p1!=lastp1)
      {
      lastp1=p1;                                                          
      printf("GPIO 17 (Pin 11) (PTT) =%d\n",p1);
      }
    if(k1!=lastk1)
      {
      lastk1=k1;
      printf("GPIO 18 (Pin 12) (KEY) =%d\n",k1);
      }    
  
}
 
