
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <time.h>
#include <iio/iio.h>
#include <unistd.h>
#include <lgpio.h>
#include <stdio.h>
#include <string.h>
#include "Graphics.h"
#include "Touch.h"
#include "Mouse.h"
#include "Morse.c"
#include <netdb.h>
#include <netinet/in.h>                                                                                                      
#include <arpa/inet.h>


//#define PLUTOIP "ip:pluto.local"

char plutoip[30];


void setFreq(double fr);
void displayFreq(double fr);
void setFreqInc();
void setTx(int ptt);
void setPtts(int p);
void setMode(int mode);
void setVolume(int vol);
void setMute(int m);
void setSquelch(int sql);
void setSSBMic(int mic);
void setFMMic(int mic);
void setAMMic(int mic);
void setRxFilter(int low,int high);
void setTxFilter(int low,int high);
void setBandBits(int b);
void processTouch();
void processMouse(int mbut);
void initGUI();
void sendFifo(char * s);
void initFifos();
void initUDP(void);
void initPluto(void);
void setPlutoRxFreq(long long rxfreq);
void setPlutoTxFreq(long long rxfreq);
void setHwRxFreq(double fr);
void setHwTxFreq(double fr);
void PlutoTxEnable(int txon);
void PlutoRxEnable(int rxon);
void detectHw();
int buttonTouched(int bx,int by);
void setKey(int k);
void displayMenu(void);
void displaySetting(int se);
void changeSetting(void);
void processGPIO(void);
void initGPIO(void);
int readConfig(void);
int writeConfig(void);
int satMode(void);
int splitMode(void);
int txvtrMode(void);
int duplexMode(void);
int multMode(void);
void setMoni(int m);
void initSDR(void);
void waterfall(void);
void clearWaterfall(void);
void P_Meter(void);
void S_Meter(void);
void setRit(int rit);
void setInputMode(int n);
void gen_palette(char colours[][3],int num_grads);
void setPlutoTxAtt(int att);
void setPlutoRxGain(int gain);
int readPlutoRxGain(void);
void setBand(int b);
void setPlutoGpo(int p);
void setTxPin(int v);
long long runTimeMs(void);                                                    
void clearPopUp(void);
void displayPopupMode(void);
void displayPopupBand(void);
void send1750(void);
void setCTCSS(int t);
void displayError(char*st);
void flushUDP(void);
void setFFTBW(int bw);

int wr_ch_lli(struct iio_channel *chn, const char* what, long long val);
int wr_ch_bool(struct iio_channel *chn, const char* what, bool val);
int wr_ch_dbl(struct iio_channel *chn, const char* what, double val);
int wr_ch_str(struct iio_channel *chn, const char* what, const char* str);
int wr_db_str(struct iio_device *dev, const char* what, const char* str);

int minGain(double freq);
int maxGain(double freq);
void setDialLock(int d);
void setBeacon(int b);
int firstpass=1;
double freq;
double freqInc=0.001;
#define numband 24
int band=3;
#define nummode 6
double bandFreq[numband] = {70.200,144.200,432.200,1296.200,2320.200,2400.100,3400.100,5760.100,10368.200,24048.200,47088.2,10489.55,433.2,433.2,433.2,433.2,433.2,433.2,1296.2,1296.2,1296.2,1296.2,1296.2,1296.2};
double bandTxOffset[numband]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-9936.0,-23616.0,-46656.0,-10069.5,0,0,0,0,0,0,0,0,0,0,0,0};
double bandRxOffset[numband]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-9936.0,-23616.0,-46656.0,-10345.0,0,0,0,0,0,0,0,0,0,0,0,0};
double bandRepShift[numband]={0,-0.6,1.6,-6.0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int bandTxHarmonic[numband]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
int bandRxHarmonic[numband]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
int bandMode[numband]={0};
int bandBitsRx[numband]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
int bandBitsTx[numband]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
int bandSquelch[numband][nummode]={0};
int bandFFTRef[numband]={-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10};
int bandTxAtt[numband]={0};
int bandRxGain[numband]={100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100};              //100 is automatic gain
int bandDuplex[numband]={0};
int bandCTCSS[numband]={0};
float bandSmeterZero[numband]={-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80};
int bandSSBFiltLow[numband]={300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300};
int bandSSBFiltHigh[numband]={3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000};
int bandFFTBW[numband]={0};

#define minFreq 0.0
#define maxFreq 99999.99999
#define minHwFreq 69.9
#define maxHwFreq 5999.99999



int mode=0;
int lastmode=0;
char * modename[nummode]={"USB","LSB","CW ","CWN","FM ","AM "};
enum {USB,LSB,CW,CWN,FM,AM};

#define numSettings 22

char * settingText[numSettings]={"Rx Gain= ","SSB Mic Gain= ","FM Mic Gain= ","AM Mic Gain= ","Repeater Shift= ","CTCSS= "," Rx Offset= ","Rx Harmonic Mixing= "," Tx Offset= ","Tx Harmonic Mixing= ","Band Bits (Rx)= ","Band Bits (Tx)= ","Copy Band Bits to Pluto=","FFT Ref= ","Tx Att= ","S-Meter Zero= ", "SSB Rx Filter Low= ", "SSB Rx Filter High= ","CW Ident= ", "CWID Carrier= ", "CW Break-In Hang Time= ", "24 Bands= "};
enum {RX_GAIN,SSB_MIC,FM_MIC,AM_MIC,REP_SHIFT,CTCSS,RX_OFFSET,RX_HARMONIC,TX_OFFSET,TX_HARMONIC,BAND_BITS_RX,BAND_BITS_TX,BAND_BITS_TO_PLUTO,FFT_REF,TX_ATT,S_ZERO,SSB_FILT_LOW,SSB_FILT_HIGH,CWID,CW_CARRIER,BREAK_IN_TIME,BANDS24};
int settingNo=RX_GAIN;
int setIndex=0;
int maxSetIndex=10;

enum {FREQ,SETTINGS,VOLUME,SQUELCH,RIT};
int inputMode=FREQ;

#define NUMCTCSS 52
int CTCSSTone[NUMCTCSS] = {0,670,693,719,744,770,797,825,854,885,915,948,974,1000,1035,1072,1109,1148,1188,1230,1273,1318,1365,1413,1462,1500,1514,1567,1598,1622,1655,1679,1713,1738,1773,1799,1835,1862,1899,1928,1966,1995,2035,2065,2107,2181,2257,2291,2336,2418,2503,2541};


//GUI Layout values X and Y coordinates for each group of buttons.

#define volButtonX 660
#define volButtonY 300
#define sqlButtonX 30
#define sqlButtonY 300
#define ritButtonX 670
#define ritButtonY 60
#define funcButtonsY 429
#define funcButtonsX 30
#define buttonHeight 50
#define buttonSpaceY 55
#define buttonWidth 100
#define buttonSpaceX 105
#define freqDisplayX 80
#define freqDisplayY 55
#define freqDisplayCharWidth 48 
#define freqDisplayCharHeight 54
#define txX 600
#define txY 15
#define modeX 200
#define modeY 15
#define txvtrX 400
#define txvtrY 15
#define moniX 500
#define moniY 15
#define settingX 200
#define settingY 390
#define popupX 30
#define popupY 374
#define FFTX 140
#define FFTY 216
#define sMeterX 2
#define sMeterY 5
#define sMeterWidth 170
#define sMeterHeight 40
#define errorX 200
#define errorY 240
#define diallockX 316
#define diallockY 15



int gpiohandle;
int ptt=0;
int ptts=0;
int moni=0;
int fifofd;
int sendBeacon=0;
int dotCount=0;
int transmitting=0;
int dialLock=0;
int squelchGate=0;
int lastSquelchGate=0;

int rxFilterLow;
int rxFilterHigh;

int bands24 = 0;

int keyDownTimer=0;
int CWIDkeyDownTime=1000;                     //time to put key down between CW Idents (100 per second)

#define configDelay 500                              //delay before config is written after tuning (5 Seconds)
int configCounter=configDelay;

int twoButTimer=0;
int lastBut=0;

int breakInTimer=0;
int breakInTime=100;

long long lastLOhz;

long long lastClock;
long progStartTime=0;

int lastKey=1;

int sMeterType = 0;

int volume=20;
#define maxvol 100

int squelch=20;
#define maxsql 100

int rit=0;
#define minrit -9990
#define maxrit 9990

int SSBMic=50;
#define maxSSBMic 100

int FMMic=50;
#define maxFMMic 100

int AMMic=20;
#define maxAMMic 100

int TxAtt=0;

int tuneDigit=8;
#define maxTuneDigit 11

#define TXDELAY 10000      //10ms delay between setting Tx output bit and sending tx command to SDR
#define RXDELAY 10000       //10ms delay between sending rx command to SDR and setting Tx output bit low. 

#define BurstLength 500000     //length of 1750Hz Burst   500ms

char mousePath[30];
char touchPath[30];
int mousePresent;
int touchPresent;
int plutoPresent;
int portsdownPresent;



int bandBitsToPluto=0;      //copy low 3 band bits to pluto IO1-IO3

int popupSel=0;
int popupFirstBand;
enum {NONE,MODE,BAND,BEACON};

#define pttPin 17       // Physical pin is 11
#define keyPin 18       // Physical pin is 12
#define txPin 21        // Physical pin is 40      
#define bandPin1 1      //Physical pin is 28
#define bandPin1alt 12  //Physical pin is 32     bandPin1 is copied to both of these pins to retain compatibility with Portsdown.
#define bandPin2 19     //Physical pin is 35
#define bandPin3 4      //Physical pin is 7
#define bandPin4 25     //Physical pin is 22
#define bandPin5 22     //Physical pin is 16
#define bandPin6 24     //Physical pin is 18
#define bandPin7 10     //Physical pin is 19
#define bandPin8 9      //Physical pin is 21


int plutoGpo=0;

//robs Waterfall

float inbuf[2];
FILE *fftstream;
FILE *txfftstream;
float buf[512][130];
int points=512;
int rows=130;
int FFTRef = -30;
int spectrum_rows=80;
unsigned char * palette;
int HzPerBin=94;                        //calculated from FFT width and number of samples. Width=48000 number of samples =512
int bwBarStart=3;
int bwBarEnd=34;
float sMeter;                             //peak reading S meter.
float sMeterPeak;

struct iio_context *plutoctx;
struct iio_device *plutophy;

//UDP server to receive FFT data from GNU RAdio

#define RXPORT 7373
#define TXPORT 7474




int main(int argc, char* argv[])
{
  strcpy(plutoip,"ip:");
  char * penv = getenv("PLUTO_IP");
  if(penv==NULL)
    {
      strcpy(penv,"pluto.local");
    }
  strcat(plutoip,penv);

  printf("plutoip = %s\n",plutoip);
   
  initUDP();
  
  
  lastClock=0;
  readConfig();
  detectHw();
  initPluto();
  initFifos();
  initScreen();
  initGPIO();
  printf("Initialising Touch at %s\n",touchPath);
  if(touchPresent) initTouch(touchPath);
  printf("Initialising Mouse at %s\n",mousePath);
  if(mousePresent) initMouse(mousePath);
  initGUI(); 
  initSDR(); 
  //              RGB Vals   Black >  Blue  >  Green  >  Yellow   >   Red     4 gradients    //number of gradients is varaible
  gen_palette((char [][3]){ {0,0,0},{0,0,255},{0,255,0},{255,255,0},{255,0,0}},4);

  
  while(1)
  {
  
    processGPIO();
                                                                                                                    
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
       if(twoButTimer>0) 
       {
        twoButTimer--;
        if(twoButTimer==0)
          {
            lastBut=0;
          }
       }
           
      }
      
      
   
    if(sendBeacon==2)
      {
        dotCount=dotCount+1;
        if(dotCount==1)
          {
            setKey(1);
          }
        if(dotCount==12)
          {
            setKey(0);
          }
        if(dotCount==25)
          {
          dotCount=0;
          }
      } 
      


    if(sendBeacon==1)                                   //sending CWID
    {
      if(keyDownTimer>0)
        {    
          if((keyDownTimer>100) &&( keyDownTimer < CWIDkeyDownTime-100))                                //Key down between Idents
           {
            setKey(1);
           }
           else
           {
            setKey(0);
           }
        keyDownTimer--;
        }    
     else
      {
       int ret=morseKey();                              //get the next key from morse string
       if(ret==-1)                                      // Ident finished
        {
        keyDownTimer=CWIDkeyDownTime + 100;             //key down for this time between idents. Add 1 second to force a minimum key up gap between idents. 
        }
       else
        {  
          setKey(ret);
        }
      }
  
    }




    waterfall();

    if(configCounter>0)                                       //save the config after 5 seconds of inactivity.
    {
      configCounter=configCounter-1;                                                                                                  
      if(configCounter==0)
        {
        writeConfig();
        }
    }
    
   
   if(firstpass==1)
   {
   setTx(1);                                              //seems to be needed to initialise Pluto
   setTx(0);
   firstpass=0;
   }
    
    while(runTimeMs() < (lastClock + 10))                //delay until the next iteration at 100 per second (10ms)
    {
    usleep(100);
    }
    lastClock=runTimeMs();
  }
}

long long runTimeMs(void)
{
struct timeval tt;
gettimeofday(&tt,NULL);
if(progStartTime==0)
  {
    progStartTime=tt.tv_sec;
  }
tt.tv_sec=tt.tv_sec - progStartTime;
return ((tt.tv_sec*1000) + (tt.tv_usec/1000));
}

void gen_palette(char colours[][3], int num_grads){
  //allocate some memory, size of palette
  palette = malloc(sizeof(char)*256*3);

  int diff[3];
  float scale=256/(num_grads);
  int pos=0;

  for(int i=0;i<num_grads;i++){
      //get differences in colours for current gradient
      diff[0]=(colours[i+1][0]-colours[i][0])/(scale-1);
      diff[1]=(colours[i+1][1]-colours[i][1])/(scale-1);
      diff[2]=(colours[i+1][2]-colours[i][2])/(scale-1);

      //create the palette built up of multiple gradients
      for(int n=0;n<scale;n++){
          palette[pos*3+2]=colours[i][0]+(n*diff[0]);
          palette[pos*3+1]=colours[i][1]+(n*diff[1]);
          palette[pos*3]=colours[i][2]+(n*diff[2]);
          pos++;
      }

  }
}

void flushUDP(void)
{
int ret;
  do
  {
  ret = fread(&inbuf,sizeof(float),1,txfftstream);
  }
  while (ret>0);
  
  do
  {
  ret = fread(&inbuf,sizeof(float),1,fftstream);
  }
  while (ret>0); 
}


void waterfall()
{
  int level,level2;
  int ret;
  int baselevel;
  int bwbaroffset;
  float scaling;
  int fftref;
  int centreShift=0;


     //check if data avilable to read
      if((transmitting==1) && (satMode()==0))
        {
        ret = fread(&inbuf,sizeof(float),1,txfftstream);
        fftref=10;
        }
      else
        {
          ret = fread(&inbuf,sizeof(float),1,fftstream);
          fftref=FFTRef;
        }
      
      if(ret>0)
    {    
    
        //shift buffer
        for(int r=rows-1;r>0;r--)
        {  
          for(int p=0;p<points;p++)
          {  
            buf[p][r]=buf[p][r-1];
          }
        }
    
        buf[0+points/2][0]=inbuf[0];    //use the read value
    
        //Read in float values, shift centre and store in buffer 1st 'row'
        for(int p=1;p<points;p++)
        {                                               
          if((transmitting==1) && (satMode()==0))
          {
          fread(&inbuf,sizeof(float),1,txfftstream);
          }
        else
          {
          fread(&inbuf,sizeof(float),1,fftstream);
          }
          
          if(p<points/2)
          {
            buf[p+points/2][0]=inbuf[0];
          }else
          {
            buf[p-points/2][0]=inbuf[0];
          }
        }
 
 
        if(((mode==CW)||(mode==CWN)) && (transmitting==1) && (satMode()==0))
          {
          bwbaroffset=800/HzPerBin;
          }
        else
          {
           bwbaroffset=0;
          }
        
 
        //use raw values to calculate S meter value
        //use highest reading within the receiver bandwidth
        
         sMeterPeak=-200;
         for (int p=points/2+bwBarStart-bwbaroffset;p<points/2+bwBarEnd-bwbaroffset;p++)
          {
           if(buf[p][0]>sMeterPeak)
             {
             sMeterPeak=buf[p][0];
             }
           
           }       
 
    
        //RF level adjustment
    
 
        baselevel=fftref-80;
        scaling = 255.0/(float)(fftref-baselevel);
        
        
    
        //draw waterfall
        for(int r=0;r<rows;r++)
        {  
          for(int p=0;p<points;p++)
          {                                                                                                           
            //limit values displayed to range specified
            if (buf[p][r]<baselevel) buf[p][r]=baselevel;
            if (buf[p][r]>fftref) buf[p][r]=fftref;
    
            //scale to 0-255
            level = (buf[p][r]-baselevel)*scaling;   
            setPixel(p+FFTX,FFTY+20+r,palette[level*3+2],palette[level*3+1],palette[level*3]);
          }
        }
    
        //clear spectrum area
        for(int r=0;r<spectrum_rows+1;r++)
        { 
          for(int p=0;p<points;p++)
          {   
            setPixel(p+FFTX,FFTY-r,0,0,0);
          }
        }
    
        //draw spectrum line
        
        scaling = spectrum_rows/(float)(fftref-baselevel);
        for(int p=0;p<points-1;p++)
        {  
            //limit values displayed to range specified
            if (buf[p][0]<baselevel) buf[p][0]=baselevel;
            if (buf[p][0]>fftref) buf[p][0]=fftref;
    
            //scale to display height
            level = (buf[p][0]-baselevel)*scaling;   
            level2 = (buf[p+1][0]-baselevel)*scaling;
            drawLine(p+FFTX, FFTY-level, p+1+FFTX, FFTY-level2,255,255,255);
        }
          
          //draw Bandwidth indicator
          int p=points/2;
          
          bwBarStart=rxFilterLow/HzPerBin;
          bwBarEnd=rxFilterHigh/HzPerBin;
          
          if(bwBarStart < -255) bwBarStart = -255;
          if(bwBarEnd  > 255) bwBarEnd = 255;
          
        
          
          if (((mode==CW) || (mode==CWN)) && (transmitting==0 && satMode()== 0))
          {
           centreShift=800/HzPerBin;
          }
          else
          {
           centreShift=0;          
          }

          if(bwBarStart > -255) drawLine(p+FFTX+bwBarStart-bwbaroffset, FFTY-spectrum_rows+5, p+FFTX+bwBarStart-bwbaroffset, FFTY-spectrum_rows,255,140,0);
          drawLine(p+FFTX+bwBarStart-bwbaroffset, FFTY-spectrum_rows, p+FFTX+bwBarEnd-bwbaroffset, FFTY-spectrum_rows,255,140,0);
          
          if(bwBarEnd < 255) drawLine(p+FFTX+bwBarEnd-bwbaroffset, FFTY-spectrum_rows+5, p+FFTX+bwBarEnd-bwbaroffset, FFTY-spectrum_rows,255,140,0);  
          //draw centre line (displayed frequency)
          drawLine(p+FFTX+centreShift, FFTY-10, p+FFTX+centreShift, FFTY-spectrum_rows,255,0,0);  
          
          
          //draw x Axis. 
          int ticks[11] = {0,21,43,64,85,107,128,149,171,192,213};    // Rounded tick spacing at 21.333 pixels per tick 
          drawLine(FFTX,FFTY +3,FFTX + points,FFTY+3,0,255,0);
          for(int tick = 0;tick < 11;tick++)
            {
             drawLine(FFTX + p + ticks[tick],FFTY+3,FFTX + p + ticks[tick],FFTY +5,0,255,0);
             drawLine(FFTX + p - ticks[tick],FFTY+3,FFTX + p - ticks[tick],FFTY +5,0,255,0); 
            }
          
          //draw scale
          setForeColour(0,255,0);
          textSize=1;
          gotoXY(p+FFTX-12,FFTY+8);
          displayStr(" 0 ");
          switch(bandFFTBW[band])
          {
              case 0:                 //48KHz BW
              gotoXY(p+FFTX-ticks[5]-24,FFTY+8);
              displayStr(" -10k   ");
              gotoXY(p+FFTX-ticks[10]-24,FFTY+8);
              displayStr(" -20k   ");
              gotoXY(p+FFTX+ticks[5]-24,FFTY+8);
              displayStr(" +10k   ");                                                                                             
              gotoXY(p+FFTX+ticks[10]-24,FFTY+8);
              displayStr(" +20k   ");
            break;
            case 1:                 //24KHz BW
              gotoXY(p+FFTX-ticks[5]-20,FFTY+8);
              displayStr(" -5k   ");
              gotoXY(p+FFTX-ticks[10]-24,FFTY+8);
              displayStr(" -10k   ");
              gotoXY(p+FFTX+ticks[5]-20,FFTY+8);
              displayStr(" +5k   ");                                                                                             
              gotoXY(p+FFTX+ticks[10]-24,FFTY+8);
              displayStr(" +10k   ");
            break;
            case 2:                 //12KHz BW
              gotoXY(p+FFTX-ticks[5]-28,FFTY+8);
              displayStr(" -2.5k   ");
              gotoXY(p+FFTX-ticks[10]-20,FFTY+8);
              displayStr(" -5k   ");
              gotoXY(p+FFTX+ticks[5]-28,FFTY+8);
              displayStr(" +2.5k   ");                                                                                             
              gotoXY(p+FFTX+ticks[10]-20,FFTY+8);
              displayStr(" +5k   ");
            break;
            case 3:                 //6KHz BW
              gotoXY(p+FFTX-ticks[5]-32,FFTY+8);
              displayStr(" -1.25k   ");
              gotoXY(p+FFTX-ticks[10]-28,FFTY+8);
              displayStr(" -2.5k   ");
              gotoXY(p+FFTX+ticks[5]-32,FFTY+8);
              displayStr(" +1.25k   ");                                                                                             
              gotoXY(p+FFTX+ticks[10]-28,FFTY+8);
              displayStr(" +2.5k   ");
            break;
          }

 

          if((transmitting==0) || (satMode()==1))
          {
          S_Meter();
          }
          
          else
          {
          P_Meter();
          }
   }       
}


void S_Meter(void)
{

          char smStr[10];
          static int sMeterCount=0;
         
          sMeterPeak=sMeterPeak-bandSmeterZero[band];                   //adjust offset to give positive values for s-meter
          int dbOver=0;
          int sValue=0;
          
          if(bandRxGain[band]==100)                  //if we are in RF AGC mode
            {
             sMeterPeak=sMeterPeak + maxGain(freq) - readPlutoRxGain();       //compensate for reduced gain due to AGC action
            }       
            
          if(sMeterPeak < 0) sMeterPeak=0;
          if(sMeterPeak >= sMeter)
            {
            sMeter=sMeterPeak;                                    //fast attack
            } 
          else
            {
            if(sMeter > 0) sMeter=sMeter-2;                    //slow decay
            }
            
 
          if(sMeter<55)
            {
            sValue=sMeter/6;
            }
          else
            {
            sValue=9;
            dbOver=sMeter-54;
            }
 
          
          
          //Draw S meter
          drawLine(sMeterX,sMeterY,sMeterX+sMeterWidth,sMeterY,255,255,255);
          drawLine(sMeterX,sMeterY,sMeterX,sMeterY+sMeterHeight,255,255,255);
          drawLine(sMeterX,sMeterY+sMeterHeight,sMeterX+sMeterWidth,sMeterY+sMeterHeight,255,255,255);
          drawLine(sMeterX+sMeterWidth,sMeterY,sMeterX+sMeterWidth,sMeterY+sMeterHeight,255,255,255);          

          for(int ln=0;ln<10;ln++)
          {
          if(sMeter<55)
          {
          drawLine(sMeterX+5,sMeterY+5+ln,sMeterX+6+sMeter*2,sMeterY+5+ln,0,255,0);
          drawLine(sMeterX+6+sMeter*2,sMeterY+5+ln,sMeterX+6+160,sMeterY+5+ln,0,0,0);
          }
          else
          {
          int redbit=sMeterX+6+110+(sMeter-55)*2;
          if(redbit > (sMeterX+6+160)) redbit= sMeterX+6+160;
          drawLine(sMeterX+5,sMeterY+5+ln,sMeterX+6+110,sMeterY+5+ln,0,255,0);
          drawLine(sMeterX+6+110,sMeterY+5+ln,redbit,sMeterY+5+ln,255,0,0);
          drawLine(redbit,sMeterY+5+ln,sMeterX+6+160,sMeterY+5+ln,0,0,0);
          }

          }
    
          //draw the squelch setting bar
          for(int ln=0;ln<2;ln++)
          {
            if(squelch<55)
            {
            drawLine(sMeterX+5,sMeterY-4+ln,sMeterX+6+squelch*2,sMeterY-4+ln,0,255,0);
            drawLine(sMeterX+6+squelch*2,sMeterY-4+ln,sMeterX+6+160,sMeterY-4+ln,0,0,0);
            }
            else
            {
            int redbit=sMeterX+6+110+(squelch-55)*2;
            if(redbit > (sMeterX+6+160)) redbit= sMeterX+6+160;
            drawLine(sMeterX+5,sMeterY-4+ln,sMeterX+6+110,sMeterY-4+ln,0,255,0);
            drawLine(sMeterX+6+110,sMeterY-4+ln,redbit,sMeterY-4+ln,255,0,0);
            drawLine(redbit,sMeterY-4+ln,sMeterX+6+160,sMeterY-4+ln,0,0,0);
            }
          }       
 
          sMeterCount++;
          if(sMeterCount>5)
          {
              sMeterCount=0;
              textSize=2;
              setForeColour(0,255,0);
              gotoXY(sMeterX+10,sMeterY+20);
              if (sMeterType == 0)
              {
              sprintf(smStr,"S%d",sValue);
              displayStr(smStr);
              if(dbOver>0)
                {
                sprintf(smStr,"+%ddB  ",dbOver);
                displayStr(smStr);
                }
              else
                {
                displayStr("       ");
                }
              }
              else
              {
                sprintf(smStr,"%.0f dB    ",sMeter);
                displayStr(smStr);
              }
           }
 

            if((sMeter < squelch) && (squelch > 0))
            {
            squelchGate = 0;
            if(squelchGate != lastSquelchGate)
              {
              setMute(1);
              lastSquelchGate=squelchGate;
              }
            }
            else
            {
            squelchGate = 1;
            if(squelchGate != lastSquelchGate)
              {
              setMute(0);
              lastSquelchGate=squelchGate;
              }
            
            }



}

 void P_Meter(void)
{

          char smStr[10];
          static int sMeterCount=0;
         
          sMeterPeak=(sMeterPeak+50)*2.1;  
                  
          if(sMeterPeak < 0) sMeterPeak=0;
          if(sMeterPeak >100) sMeterPeak=100;     // Now Scaled between 0 and 100
          
        
        
          
          
          
          if(sMeterPeak >= sMeter)
            {
            sMeter=sMeterPeak;                                    //fast attack
            } 
          else
            {
            if(sMeter > 0) 
             {
             if((mode==CW) || (mode==CWN))
                {
                sMeter=sMeter-10;                               //fast decay for CW
                }
              else if((mode==USB) || (mode==LSB))
                {
                sMeter=sMeter-3;                                 //Slow Decay for SSB. 
                }
              else
                {
                sMeter=sMeter-0.5;                               //very slow decay
                }
              }
             
             }
            
             if(sMeter<0) sMeter=0;
          
          //Draw PO meter
          drawLine(sMeterX,sMeterY,sMeterX+sMeterWidth,sMeterY,255,255,255);
          drawLine(sMeterX,sMeterY,sMeterX,sMeterY+sMeterHeight,255,255,255);
          drawLine(sMeterX,sMeterY+sMeterHeight,sMeterX+sMeterWidth,sMeterY+sMeterHeight,255,255,255);
          drawLine(sMeterX+sMeterWidth,sMeterY,sMeterX+sMeterWidth,sMeterY+sMeterHeight,255,255,255);          

          for(int ln=0;ln<10;ln++)
          {
          drawLine(sMeterX+5,sMeterY+5+ln,sMeterX+6+sMeter*1.5,sMeterY+5+ln,0,255,0);
          drawLine(sMeterX+6+sMeter*1.5,sMeterY+5+ln,sMeterX+6+160,sMeterY+5+ln,0,0,0);
          }
 
          sMeterCount++;
          if(sMeterCount>5)
          {
              sMeterCount=0;
              textSize=2;
              setForeColour(0,255,0);
              gotoXY(sMeterX+10,sMeterY+20);
              displayStr("Tx Level");
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
  portsdownPresent=0;
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
              printf("Found Mouse at %s\n",mousePath); 
            }
           if(p==1) 
           {
             sprintf(touchPath,"/dev/input/%s",handler[1]);
             touchPresent=1;
             printf("Found Touch at %s\n",touchPath);
           }
         }
      }   
    }
  fclose(fp);
  if(ln)  free(ln);
  
  if ((fp = fopen("/home/pi/rpidatv/bin/rpidatvgui", "r")))                      //test to see if Portsdown file is present. If so we change the exit behaviour. 
  {
    fclose(fp);
    portsdownPresent=1;
  }
    
    plutoPresent=1;      //this will be reset by setPlutoFreq if Pluto is not present.  
  
}

void displayError(char*st)
{
  gotoXY(errorX,errorY);
  setForeColour(255,0,0);
  textSize=2;
  displayStr(st);
}

void initPluto(void)
{
    plutoctx = iio_create_context(NULL,plutoip);
      if(iio_err(plutoctx)!=0)
      {
        plutoPresent=0;
        displayError("Pluto not responding");
        return;
      }
      else
      {
      plutophy = iio_context_find_device(plutoctx, "ad9361-phy"); 
      }
}


/* write Pluto Channel attribute: long long int */
int wr_ch_lli(struct iio_channel *chn, const char* what, long long val)
{
	const struct iio_attr *attr = iio_channel_find_attr(chn, what);
  
    if((iio_attr_write_longlong(attr, val)) !=0)
      {
        return -1;
      }        
    else
      {
        return 0;
      }  
}


/* write Pluto Channel attribute: String */
int wr_ch_str(struct iio_channel *chn, const char* what, const char* str)
{
	const struct iio_attr *attr = iio_channel_find_attr(chn, what);
  
    if((iio_attr_write_string(attr, str)) !=0)
      {
        return -1;
      }        
    else
      {
        return 0;
      }  
}


/* write Pluto Debug attribute: String */
int wr_db_str(struct iio_device *dev, const char* what, const char* str)
{
	const struct iio_attr *attr = iio_device_find_debug_attr(dev, what);
  
    if((iio_attr_write_string(attr, str)) !=0)
      {
        return -1;
      }        
    else
      {
        return 0;
      }  
}


/* write Pluto Channel attribute: Double */
int wr_ch_dbl(struct iio_channel *chn, const char* what, double val)
{
	const struct iio_attr *attr = iio_channel_find_attr(chn, what);
  
    if((iio_attr_write_double(attr, val)) !=0)
      {
        return -1;
      }        
    else
      {
        return 0;
      }  
}




/* write Pluto Channel attribute: bool */
int wr_ch_bool(struct iio_channel *chn, const char* what, bool val)
{
	const struct iio_attr *attr = iio_channel_find_attr(chn, what);
  
    if((iio_attr_write_bool(attr, val)) !=0)
      {
        return -1;
      }        
    else
      {
        return 0;
      }  
}

/* Read Pluto Channel attribute: Double */
int rd_ch_dbl(struct iio_channel *chn, const char* what, double* val)
{
	const struct iio_attr *attr = iio_channel_find_attr(chn, what);
  
    if((iio_attr_read_double(attr, val)) !=0)
      {
        return -1;
      }        
    else
      {
        return 0;
      }  
}

void setPlutoRxFreq(long long rxfreq)
{
int ret;
   if(plutoPresent)
    { 
      ret=wr_ch_lli(iio_device_find_channel(plutophy, "altvoltage0", true),"frequency", rxfreq); //Rx LO Freq
      if(ret<0)
      {
      displayError("Pluto not responding");
      }
    }
  
}

void setPlutoTxFreq(long long txfreq)
{
int ret;
   if(plutoPresent)
    { 
      ret=wr_ch_lli(iio_device_find_channel(plutophy, "altvoltage1", true),"frequency", txfreq); //Tx LO Freq
   if(ret<0)
      {
      displayError("Pluto not responding");
      }
    }
  
}

void setPlutoTxAtt(int att)
{ 
  if(plutoPresent)
    {
      wr_ch_dbl(iio_device_find_channel(plutophy, "voltage0", true),"hardwaregain", (double)att); //set Tx Attenuator     
    }
}

void setPlutoRxGain(int gain)
{ 
  if(plutoPresent)
    {
     if(gain>maxGain(freq))
        {
          wr_ch_str(iio_device_find_channel(plutophy, "voltage0", false),"gain_control_mode", "slow_attack");  //set Auto Gain
        }
        else
        {
        wr_ch_str(iio_device_find_channel(plutophy, "voltage0", false),"gain_control_mode", "manual");  //set Manual  Gain control
        wr_ch_dbl(iio_device_find_channel(plutophy, "voltage0", false),"hardwaregain", (double)gain); //set Rx Gain 
        }
    } 
}


int readPlutoRxGain(void)
{
double ret;
      if(plutoPresent)
      {
        rd_ch_dbl(iio_device_find_channel(plutophy, "voltage0", false),"hardwaregain", &ret); //Read current Rx Gain
        return (int) ret; 
      }
      else
      {
      return 73;
      }

}



void PlutoTxEnable(int txon)
{
  if(plutoPresent)
    { 
      if(txon==0)
        {
        wr_ch_bool(iio_device_find_channel(plutophy, "altvoltage1", true),"powerdown", true); //turn off TX LO
        }
      else
        {
        wr_ch_bool(iio_device_find_channel(plutophy, "altvoltage1", true),"powerdown", false); //turn on TX LO
        }
    }

}

void PlutoRxEnable(int rxon)
{
  if(plutoPresent)
    {
      if(rxon==0)
        {
        wr_ch_bool(iio_device_find_channel(plutophy, "altvoltage0", true),"powerdown", true); //turn off RX LO
        }
      else
        {
        wr_ch_bool(iio_device_find_channel(plutophy, "altvoltage0", true),"powerdown", false); //turn on RX LO
        }
    }

}

void setPlutoGpo(int p)
{
  char pins[10]; 
   
  sprintf(pins,"0x27 0x%x0",p);
  pins[9]=0;

  if(plutoPresent)
    {
      wr_db_str(plutophy,"direct_reg_access",pins);
    }
}


void initUDP(void)
{
   struct sockaddr_in myaddr;
   int fdr; 
   int fdt; 
   
//initialise Receive UDP receiver for FFT Receiver stream
   fdr=socket(AF_INET,SOCK_DGRAM,0);
   memset((char *)&myaddr,0,sizeof(myaddr));                      //Set any valid address for receiving UDP packets
   myaddr.sin_family = AF_INET;                                  //Network Connection
   myaddr.sin_addr.s_addr = htonl(INADDR_ANY);                   //Any Address
   myaddr.sin_port = htons(RXPORT);                              //set UDP POrt to listen on
   bind(fdr,(struct sockaddr *)&myaddr,sizeof(myaddr));          //bind the socket to the address  
   fftstream=fdopen(fdr,"r");                                    //open as a stream
   fcntl(fileno(fftstream), F_SETFL, O_RDONLY | O_NONBLOCK);    //set it as nonblocking
   
//initialise Receive UDP receiver for FFT Transmitter stream
   fdr=socket(AF_INET,SOCK_DGRAM,0);
   memset((char *)&myaddr,0,sizeof(myaddr));                      //Set any valid address for receiving UDP packets
   myaddr.sin_family = AF_INET;                                   //Network Connection
   myaddr.sin_addr.s_addr = htonl(INADDR_ANY);                   //Any Address
   myaddr.sin_port = htons(TXPORT);                               //set UDP POrt to listen on
   bind(fdr,(struct sockaddr *)&myaddr,sizeof(myaddr));          //bind the socket to the address  
   txfftstream=fdopen(fdr,"r");                                  //open as a stream
   fcntl(fileno(txfftstream), F_SETFL, O_RDONLY | O_NONBLOCK);   //set it as nonblocking
      
}



void initFifos()
{
 if(access("/tmp/langstoneTRx",F_OK)==-1)   //does fifo exist already?
    {
        mkfifo("/tmp/langstoneTRx", 0666);
    }
}


void sendFifo(char * s)
{
  char fs[50];
  int ret;
  int retry;
  int success;
  strcpy(fs,s);
  strcat(fs,"\n");
  
  success = 0;
  do
  {
  fifofd=open("/tmp/langstoneTRx",O_WRONLY|O_NONBLOCK);
  retry=0;
    do
     {
       ret=write(fifofd,fs,strlen(fs));
       usleep(5000);
       retry++;
     }
   while((ret==-1)&(retry<10));   
  if(ret==-1)
    {
      displayError("Waiting for Lang_TRX.py");
      success = 0;
     }
  else
     {
     success = 1;
     }
  close(fifofd);
  usleep(1000);
  }
  while(success == 0);
}

void initGPIO(void)
{
  gpiohandle = lgGpiochipOpen(4);
  lgGpioClaimInput(gpiohandle,LG_SET_PULL_UP,pttPin); 
  lgGpioClaimInput(gpiohandle,LG_SET_PULL_UP,keyPin); 
  lgGpioClaimOutput(gpiohandle,0,txPin,0); 
  lgGpioClaimOutput(gpiohandle,0,bandPin1,0); 
  lgGpioClaimOutput(gpiohandle,0,bandPin1alt,0); 
  lgGpioClaimOutput(gpiohandle,0,bandPin2,0); 
  lgGpioClaimOutput(gpiohandle,0,bandPin3,0); 
  lgGpioClaimOutput(gpiohandle,0,bandPin4,0); 
  lgGpioClaimOutput(gpiohandle,0,bandPin5,0); 
  lgGpioClaimOutput(gpiohandle,0,bandPin6,0); 
  lgGpioClaimOutput(gpiohandle,0,bandPin7,0); 
  lgGpioClaimOutput(gpiohandle,0,bandPin8,0); 
  lastKey=1;
}

void processGPIO(void)
{
int p1=1;
int k1=1;

  p1=lgGpioRead(gpiohandle,pttPin);
  k1=lgGpioRead(gpiohandle,keyPin);

  
    if(p1==0)             //if the hardware PTT has been pressed
        {
          if(ptt==0)
            {
              ptt=1;
              if(ptts==1) setPtts(0);                       //turn off the touchscreen PTT.
              setTx(ptt|ptts);
            }
        }
     else
        {
          if(ptt==1)
            {
              ptt=0;
              setTx(ptt|ptts);
            }
        }

      if(k1!=lastKey)
    {    
    setKey(!k1);
    lastKey=k1;
    } 

    if((mode==CW) ||(mode==CWN))
      {
        if(k1==0)          //key down
          {
            if((ptt|ptts)==0)   //not transmitting
              { 
                setTx(1);    
              }
            breakInTimer=breakInTime;
          }
        else
          {
            if((breakInTimer>0) & ((ptt|ptts)==0))
            {
            breakInTimer--;
            if(breakInTimer==0)
              {
              setTx(0);
              }
            }
          }
       }
     
}



void sqlButton(int show)
{
 char sqlStr[5]; 
  gotoXY(sqlButtonX,sqlButtonY);
  if(show==1)
    {
      setForeColour(0,255,0);
    }
  else
    {
      setForeColour(0,0,0);
    }
  displayButton("SQL");
  textSize=2;
  gotoXY(sqlButtonX+30,sqlButtonY-25);
  displayStr("   ");
  gotoXY(sqlButtonX+30,sqlButtonY-25);
  sprintf(sqlStr,"%d",squelch);
  displayStr(sqlStr);
}

void ritButton(int show)
{
 char ritStr[5]; 
 int to;
  if(show==1)
    {
      setForeColour(0,255,0);
    }
  else
    {
      setForeColour(0,0,0);
      gotoXY(ritButtonX,ritButtonY+buttonSpaceY);
      displayButton("Zero"); 
    }
  gotoXY(ritButtonX,ritButtonY);
  displayButton("RIT");
  textSize=2;
  to=0;
  if(abs(rit)>0) to=8;
  if(abs(rit)>9) to=16;
  if(abs(rit)>99) to=24;
  if(abs(rit)>999) to=32;
  gotoXY(ritButtonX,ritButtonY-25);
  displayStr("         ");
  gotoXY(ritButtonX+38-to,ritButtonY-25);
  if (rit==0)
  {
  sprintf(ritStr,"0");
  }
  else
  {
    sprintf(ritStr,"%+d",rit);
  }
  displayStr(ritStr);

}

void initGUI()
{
  clearScreen();


// Volume Button
  gotoXY(volButtonX,volButtonY);
  setForeColour(0,255,0);
  displayButton("Vol");
 

 //bottom row of buttons
  displayMenu();
  setBand(band);
  
//Squelch button now visible in all modes. 
    sqlButton(1);

    clearWaterfall();

}


void clearWaterfall(void)
{
  for(int r=0;r<rows;r++)
   {  
   for(int p=0;p<points;p++)
    {  
    buf[p][r]=-100;
    }
   }
   flushUDP();
}


void initSDR(void)
{
  setBand(band);
  setMode(mode); 
  setVolume(volume);
  setSquelch(squelch);
  setRit(0);
  setSSBMic(SSBMic);
  setFMMic(FMMic);
  setAMMic(AMMic);
  setFreqInc();
  lastLOhz=0;
  setFreq(freq);
  setTx(0);
}

void displayMenu()
{
gotoXY(funcButtonsX,funcButtonsY);
  setForeColour(0,255,0);
  displayButton("BAND");
  displayButton("MODE");
  
  if((mode==FM) && (bandRepShift[band]!=0))
  {
  if((ptt | ptts) && (bandDuplex[band]>0))
    {
    displayButton("1750");
    }
  else
    {
    displayButton("DUP ");
    }
  }
  else
  {
  displayButton("    ");
  }

  displayButton("SET");
  if(satMode()==1) 
    {
    displayButton("MONI");
    }
  else
  {
    displayButton("    ");
  }
  if(sendBeacon > 0)
    {
    setForeColour(255,0,0);
    if(sendBeacon==1)
      {
        displayButton("CWID");
      } 
     else
      {
        displayButton("DOTS");
      } 
    }
  else
    {
    setForeColour(0,255,0);
    displayButton("BEACON");
    }

  if(ptt|ptts)
    {
    setForeColour(255,0,0);  
    }
  else
    {
    setForeColour(0,255,0);
    }   
  displayButton("PTT");

}


void displayPopupMode(void)
{
clearPopUp();
gotoXY(popupX,popupY);
  setForeColour(0,255,0);
  for(int n=0;n<nummode;n++)
  {
    displayButton(modename[n]);
  }
popupSel=MODE;
}

void displayPopupBand(void)
{
char bstr[6];
int b;
clearPopUp();
gotoXY(popupX,popupY);
  setForeColour(0,255,0);
 displayButton("More..");   
  for(int n=0;n<6;n++)
  {
    b=bandFreq[n+popupFirstBand];
    sprintf(bstr,"%d", b);
    displayButton(bstr);
  }
popupSel=BAND;
}

void displayPopupBeacon(void)
{
clearPopUp();
gotoXY(popupX + buttonSpaceX*5,popupY);
setForeColour(0,255,0);
displayButton("DOTS");
displayButton("CWID");
popupSel=BEACON;
}


void clearPopUp(void)
{
for(int py=popupY;py<popupY+buttonHeight+1;py++)
{
  for(int px=0;px<800;px++)
  {
  setPixel(px,py,0,0,0);
  }
}
popupSel=NONE;
displayMenu();
}

                                                           
void processMouse(int mbut)
{
  if(mbut==128)       //scroll whell turned 
    {
      if((inputMode==FREQ) && (dialLock==0))
      {
        freq=freq+(mouseScroll*freqInc);
        mouseScroll=0;
        if(((freq + bandRxOffset[band])/bandRxHarmonic[band]) < minHwFreq) freq=(minHwFreq - bandRxOffset[band])/bandRxHarmonic[band];
        if(((freq + bandRxOffset[band])/bandRxHarmonic[band]) > maxHwFreq) freq=(maxHwFreq - bandRxOffset[band])/bandRxHarmonic[band];
        setFreq(freq);
        return;      
      }
      
      if(mouseScroll>0) mouseScroll=1;                     //prevent large changes when adjusting. 
      if(mouseScroll<0) mouseScroll=-1;
      
      if(inputMode==SETTINGS)
      {
        changeSetting();
        return;
      }
      if(inputMode==VOLUME)
      {
        volume=volume+mouseScroll;
        mouseScroll=0;
        if(volume < 0) volume=0;
        if(volume > maxvol) volume=maxvol;
        setVolume(volume);
        return;      
      }    
      if(inputMode==SQUELCH)
      {
        squelch=squelch+mouseScroll;
        mouseScroll=0;
        if(squelch < 0) squelch=0;
        if(squelch > maxsql) squelch=maxsql;
        bandSquelch[band][mode]=squelch;
        setSquelch(squelch);
        return;      
      }   
      if(inputMode==RIT)
      {
        rit=rit+mouseScroll*10;
        mouseScroll=0;
        if(rit < minrit) rit=minrit;
        if(rit > maxrit) rit=maxrit;
        setRit(rit);
        return;      
      }     
    }
    
  if(mbut==1+128)      //Left Mouse Button down
    {
    
      if((inputMode==SETTINGS)&&((settingNo==CWID)||(settingNo==BAND_BITS_RX)||(settingNo==BAND_BITS_TX)))
       {
         setIndex=setIndex-1;
         if(setIndex<0) setIndex=0;
         displaySetting(settingNo);
       }
      else
       {
        tuneDigit=tuneDigit-1;
        if(tuneDigit<0) tuneDigit=0;
        if(tuneDigit==5) tuneDigit=4;
        if(tuneDigit==9) tuneDigit=8;
        setFreqInc();
        setFreq(freq);
        twoButTimer=20;
        lastBut=lastBut | 1;
        if((inputMode==SETTINGS) && (settingNo==BAND_BITS_RX))
          {
          displaySetting(BAND_BITS_RX);
          }
        if((inputMode==SETTINGS) && (settingNo==BAND_BITS_TX))
          {
          displaySetting(BAND_BITS_TX);
          }
       }  
    }
    
  if(mbut==2+128)      //Right Mouse Button down
    {
      if((inputMode==SETTINGS)&&((settingNo==CWID)||(settingNo==BAND_BITS_RX)||(settingNo==BAND_BITS_TX)))
       {
         setIndex=setIndex+1;
         if(setIndex>maxSetIndex) setIndex=maxSetIndex;
         displaySetting(settingNo);
       }
      else  
       {
         tuneDigit=tuneDigit+1;
         if(tuneDigit > maxTuneDigit) tuneDigit=maxTuneDigit;
         if(tuneDigit==5) tuneDigit=6;
         if(tuneDigit==9) tuneDigit=10;
         setFreqInc();
         setFreq(freq); 
         twoButTimer=20;
         lastBut=lastBut | 2;
         if((inputMode==SETTINGS) && (settingNo==BAND_BITS_RX))
           {
             displaySetting(BAND_BITS_RX);
           }
         if((inputMode==SETTINGS) && (settingNo==BAND_BITS_TX))
           {
             displaySetting(BAND_BITS_TX);
           }
        }          
    }
 
  if((mbut==3+128) | (lastBut==3))       //Middle button down or both buttons within 100ms
     {
      if(dialLock==0)
        {
          setDialLock(1);
        }
      else
        {
         setDialLock(0); 
        }
       lastBut=0;
     }
    
  if(mbut==4+128)      //Extra Button down
    {
 
    }   
    
  if(mbut==5+128)      //Side Button down
    {
 
    }
  
    
}

void setFreqInc()
{
  if(tuneDigit==0) freqInc=10000.0;
  if(tuneDigit==1) freqInc=1000.0;
  if(tuneDigit==2) freqInc=100.0;
  if(tuneDigit==3) freqInc=10.0;
  if(tuneDigit==4) freqInc=1.0;
  if(tuneDigit==5) tuneDigit=6;
  if(tuneDigit==6) freqInc=0.1;
  if(tuneDigit==7) freqInc=0.01;
  if(tuneDigit==8) freqInc=0.001;                                    
  if(tuneDigit==9) tuneDigit=10;
  if(tuneDigit==10) freqInc=0.0001;
  if(tuneDigit==11) freqInc=0.00001;   
}
       


void processTouch()
{ 


// Volume Button   


if(buttonTouched(volButtonX,volButtonY))    //Vol
    {
      if(inputMode==VOLUME)
        {
        setInputMode(FREQ);
        }
      else
        {
        setInputMode(VOLUME);
        }
      return;
    }
                                                        


// Squelch Button   


if(buttonTouched(sqlButtonX,sqlButtonY))    //sql
    {
      if(inputMode==SQUELCH)
        {
        setInputMode(FREQ);
        }
      else
        {
        setInputMode(SQUELCH);
        }
      return;
    }


//RIT Button

if(buttonTouched(ritButtonX,ritButtonY))    //rit
    {
     if(mode!=4)
     {
      if(inputMode==RIT)
        {
        setInputMode(FREQ);
        }
      else
        {
        setInputMode(RIT);
        }
      return;
      }
    }

//RIT Zero Button

if(buttonTouched(ritButtonX,ritButtonY+buttonSpaceY))    //rit zero
    {
     setRit(0);
     setInputMode(FREQ);
    }
    
if(buttonTouched(sMeterX,sMeterY))                        //touch on s-Meter
{
   if(sMeterType == 0)
   {
     sMeterType=1;
   }
   else
   {
     sMeterType=0;
   }
}

//Function Buttons


if(buttonTouched(funcButtonsX,funcButtonsY))    //Button 1 = BAND or MENU
    {
    if((inputMode==FREQ) && (popupSel!=BAND))
    {
      writeConfig();
      displayPopupBand();
      return;
    }
    else
    {
      setInputMode(FREQ);
      clearPopUp();
      return; 
    }
      
        
    }      
if(buttonTouched(funcButtonsX+buttonSpaceX,funcButtonsY))    //Button 2 = MODE or Blank
    {
     if((inputMode==FREQ) && (popupSel!=MODE))
      {
      displayPopupMode();
      return;
      }
      else
      {
      setInputMode(FREQ);
      clearPopUp();
      return;
      }
    }
      
if(buttonTouched(funcButtonsX+buttonSpaceX*2,funcButtonsY))  // Button 3 =Blank or DUP or 1750 or NEXT
    {
     if(inputMode==FREQ)
      {
      if((mode==FM) && (bandRepShift[band]!=0))
      {
        if((ptt | ptts) && (bandDuplex[band]>0))
        {  
        send1750(); 
        }
        else
        {
        if(bandDuplex[band]==0) 
          {
          bandDuplex[band]=1;
          setMode(mode);
          }
         else 
          {
          bandDuplex[band]=0;
          setMode(mode);
          }

        }
       
      }      
      return;
      }
      else if(inputMode==SETTINGS)
      {
      settingNo=settingNo+1;
      if(settingNo==numSettings) settingNo=0;
      displaySetting(settingNo);
      return;
      }
      else
      {
      setInputMode(FREQ);
      }
    }
      
if(buttonTouched(funcButtonsX+buttonSpaceX*3,funcButtonsY))    // Button4 =SET or PREV
    {
     if(inputMode==FREQ)
      {
      setInputMode(SETTINGS);
      return;
      }
    else  if (inputMode==SETTINGS)
      {
      settingNo=settingNo-1;
      if(settingNo<0) settingNo=numSettings-1;
      displaySetting(settingNo);
      return;
      }
    else
      {
      setInputMode(FREQ);
      return;
      }
    }
       
if(buttonTouched(funcButtonsX+buttonSpaceX*4,funcButtonsY))    //Button 5 =MONI (only allowed in Sat mode)  or Blank
    {
    if(inputMode==FREQ)
      {
      if(satMode()==1)
        {
        if(moni==1) setMoni(0); else setMoni(1);
        }      
      return;
      } 
    else
      {
      setInputMode(FREQ);
      }
      
 
    }      

if(buttonTouched(funcButtonsX+buttonSpaceX*5,funcButtonsY))    //Button 6 = BEACON  or Exit to Portsdown
    {
    if(inputMode==FREQ)
      {
      if(sendBeacon==0)
        {
         displayPopupBeacon();
        }
      else
        {
        setBeacon(0);
        }
 
      return;
      }
      else if (inputMode==SETTINGS)
      {
         setBandBits(0);
         sendFifo("H0");        //unlock the flowgraph so that it can exit
         sendFifo("Q");       //kill the SDR
         clearScreen();
         writeConfig();
         iio_context_destroy(plutoctx);
         sleep(2);                  
         exit(0);
      }
      else
      {
      setInputMode(FREQ);
      }
    } 
         
if(buttonTouched(funcButtonsX+buttonSpaceX*6,funcButtonsY))   //Button 7 = PTT  or OFF
    {
    if(inputMode==FREQ)
      {
      if(ptts==0)
        {
          setPtts(1);
        }
      else
        {
          setPtts(0);
        }
      return;
      }
      else if (inputMode==SETTINGS)
      {
      setBandBits(0);
      sendFifo("H");        //unlock the flowgraph so that it can exit
      sendFifo("Q");       //kill the SDR 
      writeConfig();
      iio_context_destroy(plutoctx);
      system("sudo cp /home/pi/Langstone/splash.bgra /dev/fb0");
      sleep(2);
      system("sudo poweroff");                          
      return;      
      }
      else
      {
      setInputMode(FREQ);
      }
      

    }      


   
//Touch on Frequency Digits moves cursor to digit and sets tuning step. Removes dial lock if it is set.  

if((touchY>freqDisplayY) & (touchY < freqDisplayY+freqDisplayCharHeight) & (touchX>freqDisplayX) & (touchX < freqDisplayX+12*freqDisplayCharWidth))   
  {
    if(inputMode==FREQ) setDialLock(0);
    int tx=touchX-freqDisplayX;
    tx=tx/freqDisplayCharWidth;
    tuneDigit=tx;
    setFreqInc();
    setFreq(freq);
    return;
  }


//touch on spectrum display increments the FFT Bandwidth
if((touchY < FFTY) & (touchY > FFTY - spectrum_rows) & (touchX > FFTX) & (touchX < FFTX + points))   
  {
    bandFFTBW[band]++;
    if(bandFFTBW[band] > 3)  bandFFTBW[band] = 0;
    setFFTBW(bandFFTBW[band]);
    return;
  }



if(popupSel==MODE)
{
  for(int n=0;n<nummode;n++)
  {
     if(buttonTouched(popupX+(n*buttonSpaceX),popupY))                                
       {
       mode=n;
       setMode(mode);
       clearPopUp();
       }
  }
}

if(popupSel==BAND)
{
  if(buttonTouched(popupX,popupY))
  {
  popupFirstBand=popupFirstBand+6;
  if(bands24)
  {
    if(popupFirstBand>23) popupFirstBand=0;
  }
  else
  {
    if(popupFirstBand>11) popupFirstBand=0;
  }
  displayPopupBand();
  }
  
  for(int n=1;n<7;n++)
  {
     if(buttonTouched(popupX+(n*buttonSpaceX),popupY))                                
       {
       band=n-1+popupFirstBand;
       setBand(band);
       clearPopUp();
       }
  }

 
}


if(popupSel==BEACON)
  {
    if(buttonTouched(popupX+ 5* buttonSpaceX,popupY))       //DOTS
      {
      setBeacon(2);
      clearPopUp();
      }
     if(buttonTouched(popupX+ 6* buttonSpaceX,popupY))       //CWID
      {
      setBeacon(1);
      clearPopUp();
      }     
  }







}


int buttonTouched(int bx,int by)
{
  return ((touchX > bx) & (touchX < bx+buttonWidth) & (touchY > by) & (touchY < by+buttonHeight));
}

void setBand(int b)
{
  freq=bandFreq[band];
  setFreq(freq);
  mode=bandMode[band];
  setMode(mode);
  setFFTBW(bandFFTBW[band]);
  setBandBits(bandBitsRx[band]);
  squelch=bandSquelch[band][mode];
  setSquelch(squelch);
  setCTCSS(bandCTCSS[band]);
  FFTRef=bandFFTRef[band];
  TxAtt=bandTxAtt[band];
  setPlutoTxAtt(TxAtt);
  setPlutoRxGain(bandRxGain[band]);
  configCounter=configDelay;
}

void setPtts(int p)
{
 if(p==1)
 {
  ptts=1;
  setTx(ptt|ptts);
  gotoXY(funcButtonsX+buttonSpaceX*6,funcButtonsY);
  setForeColour(255,0,0);
  displayButton("PTT"); 
 }
  else
 {
  ptts=0;
  if(sendBeacon>0)
   {
    setBeacon(0);
    setMode(mode);
    gotoXY(funcButtonsX+buttonSpaceX*5,funcButtonsY);
    setForeColour(0,255,0);
    displayButton("BEACON");  
    }

  setTx(ptt|ptts);
  gotoXY(funcButtonsX+buttonSpaceX*6,funcButtonsY);
  setForeColour(0,255,0);
  displayButton("PTT");
  gotoXY(funcButtonsX+buttonSpaceX*5,funcButtonsY);
  setForeColour(0,255,0);
  displayButton("BEACON");   
 }

}




void setBeacon(int b)
{
 if(b > 0)
   {
      sendBeacon=b;
      morseReset();
      keyDownTimer=300;
      lastmode=mode;
      mode=CW;
      setMode(mode);
      if(lastmode==USB) freq=freq+0.0008;         //If we are working SSb add an offset of 800Hz to bring pips into Rx bandwidth.
      if(lastmode==LSB) freq=freq-0.0008;
      setFreq(freq);                  
      if(!(ptt|ptts))                     //if not already transmitting
        {
          setTx(1);                          //goto transmit
        }
      ptts=1;                             //latch the transmit on
      gotoXY(funcButtonsX+buttonSpaceX*5,funcButtonsY);
      setForeColour(255,0,0);
      displayButton("BEACON");  
      gotoXY(funcButtonsX+buttonSpaceX*6,funcButtonsY);
      setForeColour(255,0,0);
      displayButton("PTT");       
   }
  else
    {
      sendBeacon=0;                                                                                           
      ptts=0;
      setTx(0);
      setKey(0);
      usleep(100000);
      mode=lastmode;
      setMode(mode);
      if(mode==USB) freq=freq-0.0008;
      if(mode==LSB) freq=freq+0.0008;
      setFreq(freq);
      gotoXY(funcButtonsX+buttonSpaceX*5,funcButtonsY);
      setForeColour(0,255,0);
      displayButton("BEACON");  
      gotoXY(funcButtonsX+buttonSpaceX*6,funcButtonsY);
      setForeColour(0,255,0);
      displayButton("PTT");       
    }

}

 
void setVolume(int vol)
{
  char volStr[10];
  sprintf(volStr,"V%d",vol);
  sendFifo(volStr);
  setForeColour(0,255,0);
  textSize=2;
  gotoXY(volButtonX+30,volButtonY-25);
  displayStr("   ");
  gotoXY(volButtonX+30,volButtonY-25);
  sprintf(volStr,"%d",vol);
  displayStr(volStr);
  
  configCounter=configDelay;
}

void setSquelch(int sql)
{
  char sqlStr[10];
  sprintf(sqlStr,"S%d",sql);
  setForeColour(0,255,0);
  textSize=2;
  gotoXY(sqlButtonX+30,sqlButtonY-25);
  displayStr("   ");
  gotoXY(sqlButtonX+30,sqlButtonY-25);
  sprintf(sqlStr,"%d",sql);
  displayStr(sqlStr);
  configCounter=configDelay;
}

void setInputMode(int m)

{
if(inputMode==SETTINGS)
  {
  gotoXY(0,settingY);
  setForeColour(255,255,255);
  textSize=2;
  displayStr("                                                ");
  gotoXY(0,settingY+8);
  displayStr("                                                ");
  writeConfig();
  displayMenu();
  }
if(inputMode==VOLUME)
  {
    gotoXY(volButtonX,volButtonY);
    setForeColour(0,255,0);
    displayButton("Vol");
  }
if(inputMode==SQUELCH)
  {
    gotoXY(sqlButtonX,sqlButtonY);
    setForeColour(0,255,0);
    displayButton("SQL"); 
  }
if(inputMode==RIT)
  {
    gotoXY(ritButtonX,ritButtonY);
    setForeColour(0,255,0);
    displayButton("RIT");
    gotoXY(ritButtonX,ritButtonY+buttonSpaceY);
    setForeColour(0,0,0);
    displayButton("Zero");
  }
  
inputMode=m;

if(inputMode==FREQ)
  {
  setFreq(freq);
  }

if(inputMode==SETTINGS)
  {
    clearPopUp();
    gotoXY(funcButtonsX,funcButtonsY);
    setForeColour(0,255,0);
    displayButton("MENU");
    displayButton(" ");
    displayButton("NEXT");
    displayButton("PREV");
    displayButton(" ");
    setForeColour(255,0,0);
    if (portsdownPresent==1)
    {
        displayButton2x12("EXIT TO","PORTSDOWN");
    }
    else
    {
        displayButton1x12("EXIT");
    }
    displayButton1x12("SHUTDOWN");
    mouseScroll=0;
    displaySetting(settingNo); 
  }
if(inputMode==VOLUME)
  {
    gotoXY(volButtonX,volButtonY);
    setForeColour(255,0,0);
    displayButton("Vol");
  }
if(inputMode==SQUELCH)
  {
    gotoXY(sqlButtonX,sqlButtonY);
    setForeColour(255,0,0);
    displayButton("SQL"); 
  }
if(inputMode==RIT)
  {
    gotoXY(ritButtonX,ritButtonY);
    setForeColour(255,0,0);
    displayButton("RIT");
    gotoXY(ritButtonX,ritButtonY+buttonSpaceY);
    setForeColour(255,0,0);
    displayButton("Zero");
  }

}

void setRit(int ri)
{
  char ritStr[10];
  int to;
  if(!((mode==FM) || (mode==AM)))
  {
  rit=ri;
  setForeColour(0,255,0);
  textSize=2;
  to=28; 
  gotoXY(ritButtonX,ritButtonY-25);
  displayStr("         ");
  gotoXY(ritButtonX+38-to,ritButtonY-25);
  if (rit==0)
  {
  sprintf(ritStr," 0.00");
  }
  else
  {
    sprintf(ritStr,"%+3.2f",rit/1000.0);
  }
  displayStr(ritStr);
  setFreq(freq);
  }
}


void setSSBMic(int mic)
{
  char micStr[10];
  sprintf(micStr,"G%d",mic);
  sendFifo(micStr);
}

void setFMMic(int mic)
{
  char micStr[10];
  sprintf(micStr,"g%d",mic);
  sendFifo(micStr);
}

void setAMMic(int mic)
{
  char micStr[10];
  sprintf(micStr,"d%d",mic);
  sendFifo(micStr);
}

void setCTCSS(int t)
{
  char ctStr[10];
  sprintf(ctStr,"C%d",CTCSSTone[t]);
  sendFifo(ctStr);
}

void setKey(int k)
{
  char kStr[5];
  sprintf(kStr,"K%d",k);
  sendFifo(kStr);
}

void setMute(int m)
{
  if(squelchGate == 0) m=1;
  char mStr[5];
  sprintf(mStr,"U%d",m);
  sendFifo(mStr);
}

void setRxFilter(int low,int high)
{
  char filtStr[10];
  sprintf(filtStr,"I%d",low);
  sendFifo(filtStr);                                             
  sprintf(filtStr,"F%d",high);
  sendFifo(filtStr);
  
  rxFilterLow = low;
  rxFilterHigh = high;  
}

void setTxFilter(int low,int high)
{
  char filtStr[10];
  sprintf(filtStr,"i%d",low);
  sendFifo(filtStr);
  sprintf(filtStr,"f%d",high);
  sendFifo(filtStr);  
}


void setFFTBW(int bw)
{
  char BWStr[10];
  sprintf(BWStr,"W%d",bw);
  sendFifo(BWStr);
  
  switch(bw)
  {
  case 0:             //48KHz   Sample rate
  HzPerBin = 94;
  break;
  case 1:             //24KHz   Sample rate
  HzPerBin = 47;
  break; 
  case 2:             //12KHz   Sample rate
  HzPerBin = 23;
  break;
  case 3:             //6KHz   Sample rate
  HzPerBin = 12;
  break;
  }
}


void setMode(int md)
{
  bandMode[band]=md;
  gotoXY(modeX,modeY);
  setForeColour(255,255,0);
  textSize=2;
  displayStr(modename[md]);
  if((md==FM)&&(bandDuplex[band]==1))
    {
    displayStr("DUP");
    }
  else
    {
    displayStr("   ");
    }      
  if(md==USB)
    {
    sendFifo("M0");    //USB
    setTxFilter(300,3000);    //USB Filter Setting
    setRxFilter(bandSSBFiltLow[band],bandSSBFiltHigh[band]);    //USB Filter Setting    configured in settings.
    setFreq(freq);    //set the frequency to adjust for CW offset.
    ritButton(1);
    setRit(0);
    } 
  
  if(md==LSB)
    {
    sendFifo("M1");    //LSB
    setTxFilter(-3000,-300); // LSB Filter Setting
    setRxFilter(-1*bandSSBFiltHigh[band],-1*bandSSBFiltLow[band]); // LSB Filter Setting
    setFreq(freq);    //set the frequency to adjust for CW offset.
    ritButton(1);
    setRit(0);
    } 
  
  if(md==CW)
    {
    sendFifo("M2");    //CW
    setRxFilter(bandSSBFiltLow[band],bandSSBFiltHigh[band]);   // USB filter settings used for CW Wide Filter
    setTxFilter(-100,100); // CW Filter Setting
    setFreq(freq);    //set the frequency to adjust for CW offset.
    ritButton(1);
    setRit(0);
    } 
    
  if(md==CWN)
    {
    sendFifo("M3");    //CWN
    setRxFilter(600,1000);    //CW Narrow Filter
    setTxFilter(-100,100); // CW Filter Setting
    setFreq(freq);    //set the frequency to adjust for CW offset.
    ritButton(1);
    setRit(0);
    } 
  if(md==FM)
    {
    sendFifo("M4");    //FM
    setRxFilter(-7500,7500);    //FM Filter
    setTxFilter(-7500,7500);    //FM Filter  
    setFreq(freq);    //set the frequency to adjust for CW offset.
    ritButton(0);
    setRit(0);
    } 
  if(md==AM)
    {
    sendFifo("M5");    //AM
    setRxFilter(-3000,3000);    //AM Filter 
    setTxFilter(-3000,3000);    //AM Filter 
    setFreq(freq);    //set the frequency to adjust for CW offset.
    ritButton(0);
    setRit(0);
    } 

    squelch=bandSquelch[band][mode];
    setSquelch(squelch);
                                                                 
configCounter=configDelay;
}

void setTxPin(int v)
{
  if(v==1) lgGpioWrite(gpiohandle,txPin,1); else lgGpioWrite(gpiohandle,txPin,0);  
}

void setTx(int pt)
{
  if((pt==1)&&(transmitting==0))
    {
    if(firstpass == 0)                                      //don't set the Output pins if we are still initialising
        { 
         setTxPin(1);
         setBandBits(bandBitsTx[band]);
         plutoGpo=plutoGpo | 0x10;
         setPlutoGpo(plutoGpo);                               //set the Pluto GPO Pin 
        } 
      usleep(TXDELAY);
      setHwTxFreq(freq);
      if((mode==FM)&&(bandDuplex[band]==1))
        {
        displayFreq(freq+bandRepShift[band]);
        displayMenu();
        }
      PlutoTxEnable(1);
      if (moni==0) setMute(1);                        //mute the receiver
      if(satMode()==0)
      {
        sMeter=0;
        setHwRxFreq(freq+10.0);               //offset the Rx frequency to prevent unwanted mixing. (happens even if disabled!) 
        PlutoRxEnable(0);
      }
      if(satMode()==0)
      {
        clearWaterfall();
      }
      sendFifo("T");
      gotoXY(txX,txY);
      setForeColour(255,0,0);
      textSize=2;
      displayStr("Tx");
      transmitting=1;  
    }
  else if((pt==0)&&(transmitting==1))
    {
      if(satMode()==0)
      {
      sMeter=0;
      clearWaterfall();
      }
      
      sendFifo("R");
      setMute(0);                  //unmute the receiver 
      setHwTxFreq(freq+10.0);           //offset the Tx freq to prevent unwanted spurious
      PlutoTxEnable(0);
      PlutoRxEnable(1);
      setHwRxFreq(freq);
      if((mode==FM)&&(bandDuplex[band]==1))
        {
        displayFreq(freq);
        displayMenu();
        }
      gotoXY(txX,txY);
      setForeColour(0,255,0);
      textSize=2;
      displayStr("Rx");                                                                           
      transmitting=0;
      usleep(RXDELAY);
      setTxPin(0);
      setBandBits(bandBitsRx[band]);
      plutoGpo=plutoGpo & 0xEF;
      setPlutoGpo(plutoGpo);                               //clear the Pluto GPO Pin 
    }
}

void setHwRxFreq(double fr)
{
  long long rxoffsethz;
  long long LOrxfreqhz;
  long long rxfreqhz;
  double frRx;
  double frTx;
  
  frRx=fr+bandRxOffset[band];
  
  
  if(bandRxHarmonic[band] < 2)
  {
    if(frRx > maxHwFreq)
    {
    frRx = maxHwFreq;
    }
    if(frRx < minHwFreq)
    {
    frRx = minHwFreq;
    }
  }
  
  rxfreqhz=frRx*1000000;
  
  if (rxfreqhz<69900000) rxfreqhz=69900000;         //this is the lowest frequency we can receive with a pluto 
  
  if(rxfreqhz<70100000)
  {
  rxoffsethz=(rxfreqhz-70000000);        //Special case for receiving below 70.100     Use the offset of +-100KHz
  LOrxfreqhz=70000000;
  }
  else
  {
  rxoffsethz=(rxfreqhz % 100000)+50000;        //use just the +50Khz to +150Khz positive side of the sampled spectrum. This avoids seeing the DC hump .
  LOrxfreqhz=rxfreqhz-rxoffsethz;
  }

  if( bandRxHarmonic[band]>1)                                //allow for harmonic mixing for higher bands (10GHz)
    {
      LOrxfreqhz=LOrxfreqhz/bandRxHarmonic[band];
    }
  

  rxoffsethz=rxoffsethz+rit;
  if((mode==CW)|(mode==CWN))
    {
     rxoffsethz=rxoffsethz-800;         //offset  for CW tone of 800 Hz    
    }
  if(LOrxfreqhz!=lastLOhz)         
    {
      setPlutoRxFreq(LOrxfreqhz);          //Control Pluto directly to bypass problems with Gnu Radio Sink
      lastLOhz=LOrxfreqhz;
    }
  
  char offsetStr[32];
  sprintf(offsetStr,"O%d",rxoffsethz);   //send the rx offset tuning value 
  sendFifo(offsetStr);
}

void setHwTxFreq(double fr)
{                                                                        
  long long txfreqhz;
  double frTx;
  
  frTx=fr+bandTxOffset[band];
  
 
  if(bandTxHarmonic[band] < 2)
  {
    if(frTx > maxHwFreq)
    {
    frTx = maxHwFreq;
    }
    if(frTx < minHwFreq)
    {
    frTx = minHwFreq;
    }
  }
  
  if((mode==FM)&&(bandDuplex[band]==1))
    {
    frTx=frTx+bandRepShift[band];
    }
 
  txfreqhz=frTx*1000000;
   
  if(bandTxHarmonic[band]>1)                    //allow for Harmonic mixing for higher bands or for external multiplier such as Hydra
    {
    txfreqhz= txfreqhz/bandTxHarmonic[band];
    } 
  
  setPlutoTxFreq(txfreqhz);          //Control Pluto directly to bypass problems with Gnu Radio Sink
}

void displayFreq(double fr)
{
  long long freqhz;
  char digit[16]; 
  
  fr=fr+0.0000001;   // correction for rounding errors.
  freqhz=fr*1000000;    
  freqhz=freqhz+100000000000;     //force it to be 12 digits long
  sprintf(digit,"%lld",freqhz);
  
  gotoXY(freqDisplayX,freqDisplayY);
  setForeColour(0,0,255);
  textSize=6;
  if(digit[1]>'0') displayChar(digit[1]); else displayChar(' ');
  if((digit[1]>'0') | (digit[2]>'0')) displayChar(digit[2]); else displayChar(' ');
  if((digit[1]>'0') | (digit[2]>'0') | digit[3]>'0')  displayChar(digit[3]); else displayChar(' ');
  displayChar(digit[4]);
  displayChar(digit[5]);
  displayChar('.');
  displayChar(digit[6]);
  displayChar(digit[7]);
  displayChar(digit[8]);
  displayChar('.');
  displayChar(digit[9]);
  displayChar(digit[10]);
  
// Underline the currently selected tuning digit
  
  for(int dtd=0;dtd<12;dtd++)
    {
      gotoXY(freqDisplayX+dtd*freqDisplayCharWidth+4,freqDisplayY+freqDisplayCharHeight+5);
      int bb = 0;
      if (dtd==tuneDigit) bb=255;
      for(int p=0; p < freqDisplayCharWidth; p++)
        {
          setPixel(currentX+p,currentY+1,0,0,bb);
          setPixel(currentX+p,currentY+2,0,0,bb);
          setPixel(currentX+p,currentY+3,0,0,bb);
        }
    }

}

void setFreq(double fr)
{

  
  if(ptt | ptts) 
  {
    setHwTxFreq(fr);        //set Hardware Tx frequency if we are transmitting
  }
  else
  {
  setHwRxFreq(fr);       //set Hardware Rx frequency if we are receiving
  }

displayFreq(fr);
 
//set XVTR, SAT, SPLIT or MULT indication if needed.
  gotoXY(txvtrX,txvtrY);
  setForeColour(0,255,0);
  textSize=2;
  if(multMode()==1)
    {
    displayStr(" MULT  ");
    }  
  else if( txvtrMode()==1)
    {
     displayStr(" XVTR  "); 
    }
  else if(satMode()==1)
    {
    displayStr("  SAT  ");
    }
  else if(splitMode()==1)
    {
    displayStr(" SPLIT ");
    } 
  else
    {
      displayStr("       ");
    } 
 
   gotoXY(funcButtonsX+buttonSpaceX*4,funcButtonsY);
   setForeColour(0,255,0);
   if(inputMode==FREQ)
   {
     if(satMode()==1)
      {
       displayButton("MONI");
       setMoni(moni);
      }  
      else
      {
       displayButton("    ");
       setMoni(0);
      }
    }
 
 configCounter=configDelay;                       //write config after this amount of inactivity    
}

int satMode(void)
{
if(((abs(bandTxOffset[band]-bandRxOffset[band]) > 1 ) & (bandRxOffset[band]!=0) ) & bandRxHarmonic[band]<2  & bandTxHarmonic[band]<2 )     // if we have a differnt Rx and Tx offset and we are not multiplying then we must be in Sat mode. 
  {
  return 1;
  }
else
  {
  return 0;
  }
}

int txvtrMode(void)
{
if((abs(bandTxOffset[band]-bandRxOffset[band]) <1) & (abs(bandTxOffset[band]) >1 )  )       //if the tx and rx offset are the same and non zero then we are in Transverter mode
  {
  return 1;
  }
else
  {
  return 0;
  }
}

int splitMode(void)
{
if((abs(bandTxOffset[band])>0) & (bandRxOffset[band]==0))     //  if tx Offset is non zero and rxoffset is zero then we are in split mode. 
  {
  return 1;
  }
else
  {
  return 0;
  }
}

int multMode(void)
{
if((bandRxHarmonic[band]>1) || (bandTxHarmonic[band] >1))     //  if either of the Harmonic modes is set then we are in Mult mode. 
  {
  return 1;
  }
else
  {
  return 0;
  }
}

void setDialLock(int d)
{
  if(d==0)
  {
    dialLock=0;
    gotoXY(diallockX,diallockY);
    textSize=2;
    displayStr("    ");
  }
  else
  {
    dialLock=1;
    gotoXY(diallockX,diallockY);
    textSize=2;
    setForeColour(255,0,0);
    displayStr("LOCK");
  }
}

void setMoni(int m)
{
  if(m==1)
    {
     setMute(0);
     moni=1;
     gotoXY(moniX,moniY);
     textSize=2;
     setForeColour(0,255,0);
     displayStr("MONI");
    } 
  else
    {
     if (ptt | ptts) setMute(1);
     moni=0;
     gotoXY(moniX,moniY);
     textSize=2;
     displayStr("    ");
    }
}

void send1750(void)
{
sendFifo("B1");
gotoXY(funcButtonsX+buttonSpaceX*2,funcButtonsY);
setForeColour(255,0,0);
displayButton("1750");
usleep(BurstLength);
sendFifo("B0");
gotoXY(funcButtonsX+buttonSpaceX*2,funcButtonsY);
setForeColour(0,255,0);
displayButton("1750");
}

void setBandBits(int b)
{
    if(b & 0x01) 
        {
        lgGpioWrite(gpiohandle,bandPin1,1);
        lgGpioWrite(gpiohandle,bandPin1alt,1);
        }
    else
        {
        lgGpioWrite(gpiohandle,bandPin1,0);
        lgGpioWrite(gpiohandle,bandPin1alt,0);
        }
        
    if(b & 0x02) 
        {
        lgGpioWrite(gpiohandle,bandPin2,1);
        }
    else
        {
        lgGpioWrite(gpiohandle,bandPin2,0);
        }   
    
    if(b & 0x04) 
        {
        lgGpioWrite(gpiohandle,bandPin3,1);
        }
    else
        {
        lgGpioWrite(gpiohandle,bandPin3,0);
        }   
    
    if(b & 0x08) 
        {
        lgGpioWrite(gpiohandle,bandPin4,1);
        }
    else
        {
        lgGpioWrite(gpiohandle,bandPin4,0);
        }   
    
    if(b & 0x10) 
        {
        lgGpioWrite(gpiohandle,bandPin5,1);
        }
    else
        {
        lgGpioWrite(gpiohandle,bandPin5,0);
        }   
    
    if(b & 0x20) 
        {
        lgGpioWrite(gpiohandle,bandPin6,1);
        }
    else
        {
        lgGpioWrite(gpiohandle,bandPin6,0);
        }   
        
    if(b & 0x40) 
        {
        lgGpioWrite(gpiohandle,bandPin7,1);
        }
    else
        {
        lgGpioWrite(gpiohandle,bandPin7,0);
        }   
        
    if(b & 0x80) 
        {
        lgGpioWrite(gpiohandle,bandPin8,1);
        }
    else
        {
        lgGpioWrite(gpiohandle,bandPin8,0);
        }       

//  copy bits 0,1 and 2to Pluto GPO Pins if enabled

 if(bandBitsToPluto==1)
 {
  if(b & 0x01) 
      {
      plutoGpo=plutoGpo | 0x20;
      }
  else
      {
      plutoGpo=plutoGpo & 0xDF;
      }
      
  if(b & 0x02) 
      {
      plutoGpo=plutoGpo | 0x40;
      }
  else
      {
      plutoGpo=plutoGpo & 0xBF;
      }   
  
  if(b & 0x04) 
      {
      plutoGpo=plutoGpo | 0x80;
      }
  else
      {;
      plutoGpo=plutoGpo & 0x7F;
      }   
  setPlutoGpo(plutoGpo);
 } 
 else
 {
   plutoGpo=plutoGpo & 0x1F;
   setPlutoGpo(plutoGpo);
 }
 
  
}


void changeSetting(void)
{
  if(settingNo==SSB_MIC)        //SSB Mic Gain
      {
      SSBMic=SSBMic+mouseScroll;
      mouseScroll=0;
      if(SSBMic<0) SSBMic=0;
      if(SSBMic>maxSSBMic) SSBMic=maxSSBMic;
      setSSBMic(SSBMic);
      displaySetting(settingNo);
      }
   if(settingNo==FM_MIC)        // FM Mic Gain
      {
      FMMic=FMMic+mouseScroll;
      mouseScroll=0;
      if(FMMic<0) FMMic=0;
      if(FMMic>maxFMMic) FMMic=maxFMMic;
      setFMMic(FMMic);
      displaySetting(settingNo);
      }
   if(settingNo==AM_MIC)        // AM Mic Gain
      {
      AMMic=AMMic+mouseScroll;
      mouseScroll=0;
      if(AMMic<0) AMMic=0;
      if(AMMic>maxAMMic) AMMic=maxAMMic;
      setAMMic(AMMic);
      displaySetting(settingNo);
      }
    if(settingNo==REP_SHIFT)        //Repeater Shift
      {
        bandRepShift[band]=bandRepShift[band]+mouseScroll*freqInc;
        mouseScroll=0;
        setFreq(freq);
        displaySetting(settingNo);
      }  
    if(settingNo==CTCSS)        //CTCSS Tone
      {
        bandCTCSS[band]=bandCTCSS[band]+mouseScroll;
        if(bandCTCSS[band] < 0 ) bandCTCSS[band]=0;
        if(bandCTCSS[band] > (NUMCTCSS - 1) ) bandCTCSS[band]= NUMCTCSS - 1;        
        mouseScroll=0;
        setCTCSS(bandCTCSS[band]);
        displaySetting(settingNo);
      }  
   if(settingNo==RX_OFFSET)        //Transverter Rx Offset 
      {
        bandRxOffset[band]=bandRxOffset[band]-mouseScroll*freqInc;
        if(bandRxOffset[band] > 99999.9) bandRxOffset[band]= 99999.9;
        if(bandRxOffset[band] < -99999.9) bandRxOffset[band]= -99999.9;
        freq=freq+mouseScroll*freqInc;
        if((freq + bandRxOffset[band]) > maxHwFreq )
        {
          freq = freq - ((freq + bandRxOffset[band]) - maxHwFreq); 
        }
        
        if((freq + bandRxOffset[band]) < minHwFreq )
        {
          freq = freq + ( minHwFreq - (freq + bandRxOffset[band])); 
        }  
            
        mouseScroll=0;
        setFreq(freq);
        displaySetting(settingNo);
      } 
 if(settingNo==RX_HARMONIC)        // RX Harmonic mixing number
      {
      if(mouseScroll>0)
      {
        bandRxHarmonic[band]=5;
      }
      if(mouseScroll<0)
      {
         bandRxHarmonic[band]=1;
      }
      mouseScroll=0;
      setFreq(freq);
      displaySetting(settingNo);                                                                 
      }    
   if(settingNo==TX_OFFSET)        //Transverter Tx Offset
      {
        bandTxOffset[band]=bandTxOffset[band]-mouseScroll*freqInc;
        if(bandTxOffset[band] > 99999.9) bandTxOffset[band]= 99999.9;
        if(bandTxOffset[band] < -99999.9) bandTxOffset[band]= -99999.9;
 
        freq=freq+mouseScroll*freqInc;
  
        if((freq + bandTxOffset[band]) > maxHwFreq )
        {
          freq = freq - ((freq + bandTxOffset[band]) - maxHwFreq); 
        }
        
        if((freq + bandTxOffset[band]) < minHwFreq )
        {
          freq = freq + ( minHwFreq - (freq + bandTxOffset[band])); 
        }
        
        mouseScroll=0;
        setFreq(freq);
        displaySetting(settingNo);
      }  
 if(settingNo==TX_HARMONIC)        // TX Harmonic mixing number or external multiplier
      {
      bandTxHarmonic[band]=bandTxHarmonic[band]+mouseScroll;
      if(bandTxHarmonic[band]<1) bandTxHarmonic[band]=1;
      if(bandTxHarmonic[band]>5) bandTxHarmonic[band]=5;
      if(mouseScroll>0)
      {
        if(bandTxHarmonic[band]==4) bandTxHarmonic[band]=5;
        if(bandTxHarmonic[band]==3) bandTxHarmonic[band]=5;
      }
      else
      {
        if(bandTxHarmonic[band]==4) bandTxHarmonic[band]=2;
        if(bandTxHarmonic[band]==3) bandTxHarmonic[band]=2;
      }
      mouseScroll=0;
      setFreq(freq);
      displaySetting(settingNo);  
      }      
   if(settingNo==BAND_BITS_RX)        // Band Bits Rx
      {
      if(setIndex==7)
        {
        bandBitsRx[band]=bandBitsRx[band] ^ 0x01; 
        }
      if(setIndex==6)
        {
        bandBitsRx[band]=bandBitsRx[band] ^ 0x02; 
        }
      if(setIndex==5)
        {
        bandBitsRx[band]=bandBitsRx[band] ^ 0x04; 
        }
      if(setIndex==4)
        {
        bandBitsRx[band]=bandBitsRx[band] ^ 0x08; 
        }
       if(setIndex==3)
        {
        bandBitsRx[band]=bandBitsRx[band] ^ 0x10; 
        }
      if(setIndex==2)
        {
        bandBitsRx[band]=bandBitsRx[band] ^ 0x20; 
        }
      if(setIndex==1)
        {
        bandBitsRx[band]=bandBitsRx[band] ^ 0x40; 
        }
      if(setIndex==0)
        {
        bandBitsRx[band]=bandBitsRx[band] ^ 0x80; 
        }
      mouseScroll=0;
      if(bandBitsRx[band]<0) bandBitsRx[band]=0;
      if(bandBitsRx[band]>255) bandBitsRx[band]=255;
      setBandBits(bandBitsRx[band]);
      displaySetting(settingNo);  
      }  
if(settingNo==BAND_BITS_TX)        // Band Bits Tx
      {
      if(setIndex==7)
        {
        bandBitsTx[band]=bandBitsTx[band] ^ 0x01; 
        }
      if(setIndex==6)
        {
        bandBitsTx[band]=bandBitsTx[band] ^ 0x02; 
        }
      if(setIndex==5)
        {
        bandBitsTx[band]=bandBitsTx[band] ^ 0x04; 
        }
      if(setIndex==4)
        {
        bandBitsTx[band]=bandBitsTx[band] ^ 0x08; 
        }
       if(setIndex==3)
        {
        bandBitsTx[band]=bandBitsTx[band] ^ 0x10; 
        }
      if(setIndex==2)
        {
        bandBitsTx[band]=bandBitsTx[band] ^ 0x20; 
        }
      if(setIndex==1)
        {
        bandBitsTx[band]=bandBitsTx[band] ^ 0x40; 
        }
      if(setIndex==0)
        {
        bandBitsTx[band]=bandBitsTx[band] ^ 0x80; 
        }
      mouseScroll=0;
      if(bandBitsTx[band]<0) bandBitsTx[band]=0;
      if(bandBitsTx[band]>255) bandBitsTx[band]=255;
      displaySetting(settingNo);  
      }  
    if(settingNo==BAND_BITS_TO_PLUTO)        // Copy Band Bits to Pluto
      {
      if(mouseScroll>0)
      {
        bandBitsToPluto=1;
      }
      if(mouseScroll<0)
      {
         bandBitsToPluto=0;
      }
      mouseScroll=0;
      displaySetting(settingNo);  
      }                 
   if(settingNo==FFT_REF)        // FFT Ref Level
      {
      FFTRef=FFTRef+mouseScroll;
      mouseScroll=0;
      if(FFTRef<-80) FFTRef=-80;
      if(FFTRef>30) FFTRef=30;
      bandFFTRef[band]=FFTRef;
      displaySetting(settingNo);  
      }    
    if(settingNo==TX_ATT)        // Tx Attenuator
      {
      TxAtt=TxAtt+mouseScroll;
      mouseScroll=0;
      if(TxAtt<-89) TxAtt=-89;
      if(TxAtt>0) TxAtt=0;
      bandTxAtt[band]=TxAtt;
      setPlutoTxAtt(TxAtt);
      displaySetting(settingNo);  
      }  
     if(settingNo==RX_GAIN)        // Rx Gain Setting
      {
      if (bandRxGain[band] == 100)
        {
        bandRxGain[band]=maxGain(freq)+1+mouseScroll;
        }
      else
      {
        bandRxGain[band]=bandRxGain[band]+mouseScroll;
      }

      mouseScroll=0;
      if(bandRxGain[band]< minGain(freq)) bandRxGain[band]=minGain(freq);
      if(bandRxGain[band]> maxGain(freq)) bandRxGain[band]=100;
      setPlutoRxGain(bandRxGain[band]);
      displaySetting(settingNo);  
      }                   
    if(settingNo==S_ZERO)        // S Meter Zero
      {
      bandSmeterZero[band]=bandSmeterZero[band]+mouseScroll;
      mouseScroll=0;
      if(bandSmeterZero[band]<-140) bandSmeterZero[band]=-140;
      if(bandSmeterZero[band]>-30) bandSmeterZero[band]=-30;
      displaySetting(settingNo);  
      }   
    if(settingNo==SSB_FILT_LOW)        // SSB Filter Low
      {
      bandSSBFiltLow[band]=bandSSBFiltLow[band]+mouseScroll*10;
      mouseScroll=0;
      if(bandSSBFiltLow[band] < 0) bandSSBFiltLow[band]=0;
      if(bandSSBFiltLow[band] >1000) bandSSBFiltLow[band]=1000;
      setMode(mode);                 //refresh mode to set new filter settings
      displaySetting(settingNo);  
      }                    
    if(settingNo==SSB_FILT_HIGH)        // SSB Filter High
      {
      bandSSBFiltHigh[band]=bandSSBFiltHigh[band]+mouseScroll*10;
      mouseScroll=0;
      if(bandSSBFiltHigh[band] < 1000) bandSSBFiltHigh[band]=1000;
      if(bandSSBFiltHigh[band] > 5000) bandSSBFiltHigh[band]=5000;
      setMode(mode);                 //refresh mode to set new filter settings
      displaySetting(settingNo);  
      }     
    if(settingNo==CW_CARRIER)        // CWID Carrier time
      {
      CWIDkeyDownTime=CWIDkeyDownTime+mouseScroll*100;
      mouseScroll=0;
      if(CWIDkeyDownTime< 0) CWIDkeyDownTime=0;
      if(CWIDkeyDownTime> 12000) CWIDkeyDownTime=12000;
      displaySetting(settingNo);  
      }  
      
     if(settingNo==CWID)            //CW Ident string
     {
     int c;
     c= morseIdent[setIndex];
     c=c+mouseScroll;
     if(mouseScroll>0)
       {
         if(c>95) c=47;
         if((c>57)&&(c<65)) c=65;
         if(c>90) c=95;
       }
     if(mouseScroll<0)
       {
          if(c<47) c=95;
          if((c>90)&&(c<95)) c=90;
          if((c>57)&&(c<65)) c=57;
       }
     
     morseIdent[setIndex] = c;
     mouseScroll=0;
     displaySetting(settingNo); 
     }  
     
  if(settingNo==BREAK_IN_TIME)        // CW Break In Timer
      {
      breakInTime=breakInTime+mouseScroll;
      mouseScroll=0;
      if(breakInTime< 50) breakInTime=50;
      if(breakInTime> 200) breakInTime=200;
      displaySetting(settingNo);  
      } 
      
  if(settingNo==BANDS24)        // 24 band mode
      {
      if(mouseScroll > 0)   bands24= 1;
      if(mouseScroll < 0)   bands24= 0;
      mouseScroll=0;  
      displaySetting(settingNo);  
      }                                                                                                                    
}

               
int minGain(double freq)
{
double rxfreq;

rxfreq=(freq+bandRxOffset[band])/bandRxHarmonic[band];

if(rxfreq<1300)
 {
 return -1;
 }
if((rxfreq>=1300) && (rxfreq<4000))
  {
  return -3;
  }
if(rxfreq>=4000)
  {
  return -10;
  }
  
return 0;
}

int maxGain(double freq)
{
double rxfreq; 

rxfreq=(freq+bandRxOffset[band])/bandRxHarmonic[band];

if(rxfreq<1300)
 {
 return 73;
 }
if((rxfreq>=1300) && (rxfreq<4000))
  {
  return 71;
  }
if(rxfreq>=4000)
  {
  return 62;
  }
  
return 73;
}





void displaySetting(int se)
{
  char valStr[30];
  gotoXY(0,settingY);
  textSize=2;                                                            
  setForeColour(255,255,255);
  displayStr("                                                ");
  gotoXY(0,settingY+8);
  displayStr("                                                ");
  gotoXY(settingX,settingY);
  if((se==TX_OFFSET)||(se==RX_OFFSET)||(se==CWID))
  {
  gotoXY(0,settingY);
  }
  displayStr(settingText[se]);
 
 if(se==SSB_MIC)
  {
  sprintf(valStr,"%d",SSBMic);
  displayStr(valStr);
  }
 if(se==FM_MIC)
  {
  sprintf(valStr,"%d",FMMic);
  displayStr(valStr);
  }
 if(se==AM_MIC)
  {
  sprintf(valStr,"%d",AMMic);
  displayStr(valStr);
  } 
if(se==REP_SHIFT)
  {
  sprintf(valStr,"%.5f",bandRepShift[band]);
  displayStr(valStr);
  }
if(se==CTCSS)
  {
  sprintf(valStr,"%.1f Hz",CTCSSTone[bandCTCSS[band]]/10.0);
  displayStr(valStr);
  }
  if(se==RX_OFFSET)
  {
  sprintf(valStr,"%.5f",bandRxOffset[band]);
  displayStr(valStr);
  displayStr(" Rx Freq= ");
  sprintf(valStr,"%.5f",freq+bandRxOffset[band]);
  displayStr(valStr);
  }
  if(se==RX_HARMONIC)
  {
  sprintf(valStr,"X%d",bandRxHarmonic[band]);
  displayStr(valStr);
  } 
  if(se==TX_OFFSET)
  {
  sprintf(valStr,"%.5f",bandTxOffset[band]);
  displayStr(valStr);
  displayStr(" Tx Freq= ");
  sprintf(valStr,"%.5f",freq+bandTxOffset[band]);
  displayStr(valStr);
  }
  if(se==TX_HARMONIC)
  {
  sprintf(valStr,"X%d",bandTxHarmonic[band]);
  displayStr(valStr);
  }   
  if(se==BAND_BITS_RX)
  { 
  maxSetIndex=7;
  setForeColour(255,255,255);
  for(int b=128;b>0;b=b>>1)
    {
      if(((setIndex==7)&&(b==1)) || ((setIndex==6)&&(b==2))  || ((setIndex==5)&&(b==4)) || ((setIndex==4)&&(b==8)) || ((setIndex==3)&&(b==16))  || ((setIndex==2)&&(b==32))  || ((setIndex==1)&&(b==64))  || ((setIndex==0)&&(b==128)))
        {
          setForeColour(0,255,0);
        }
      else
        {
          setForeColour(255,255,255);
        }
      if(bandBitsRx[band] & b)
        {
        displayChar('1');
        }
      else
        {
        displayChar('0');
        }
    } 
  }
if(se==BAND_BITS_TX)
  { 
  maxSetIndex=7;
  setForeColour(255,255,255);
  for(int b=128;b>0;b=b>>1)
    {
      if(((setIndex==7)&&(b==1)) || ((setIndex==6)&&(b==2))  || ((setIndex==5)&&(b==4)) || ((setIndex==4)&&(b==8)) || ((setIndex==3)&&(b==16))  || ((setIndex==2)&&(b==32))  || ((setIndex==1)&&(b==64))  || ((setIndex==0)&&(b==128)))
        {
          setForeColour(0,255,0);
        }
      else
        {
          setForeColour(255,255,255);
        }
      if(bandBitsTx[band] & b)
        {
        displayChar('1');
        }
      else
        {
        displayChar('0');
        }
    } 
  }
  if(se==BAND_BITS_TO_PLUTO)
  {
    if(bandBitsToPluto==1)
      {
      sprintf(valStr,"Yes");
      }
    else
      {
      sprintf(valStr,"No");
      }
   displayStr(valStr);  
  } 
  if(se==FFT_REF)
  {
  sprintf(valStr,"%d",FFTRef);
  displayStr(valStr);
  }
  if(se==TX_ATT)
  {
  sprintf(valStr,"%d dB",TxAtt);
  displayStr(valStr);
  }
  if(se==RX_GAIN)
  {
    if(bandRxGain[band]>maxGain(freq))
    {
    sprintf(valStr,"Auto");
    }
    else
    {
    sprintf(valStr,"%d dB",bandRxGain[band]);
    }
  displayStr(valStr);
  }
  if(se==S_ZERO)
  {
  sprintf(valStr,"%.0f dB",bandSmeterZero[band]);
  displayStr(valStr);
  }
  if(se==SSB_FILT_LOW)
  {
  sprintf(valStr,"%d Hz",bandSSBFiltLow[band]);
  displayStr(valStr);
  }
  if(se==SSB_FILT_HIGH)
  {
  sprintf(valStr,"%d Hz",bandSSBFiltHigh[band]);
  displayStr(valStr);
  }
  if(se==CW_CARRIER)
  {
  sprintf(valStr,"%d Secs",CWIDkeyDownTime/100);
  displayStr(valStr);
  }
  if(se==CWID)
  {
    maxSetIndex=MORSEIDENTLENGTH-2;
   for(int c=0; c<MORSEIDENTLENGTH;c++)
    {
      if(setIndex==c)
        {
          setForeColour(0,255,0);
        }
      else
        {
          setForeColour(255,255,255);
        }
      if(morseIdent[c]>0)
        {
          if((morseIdent[c]==95) && (setIndex!=c))
            {
              displayChar(32);
            }
          else
            {
              displayChar(morseIdent[c]);
            }
        } 
      else
        {
          if(setIndex>=c)
            {
            setIndex=c;
            morseIdent[c]=95;
            setForeColour(0,255,0);
            displayChar(95);
            morseIdent[c+1]=0;          
            }
          break;
        }   
    }
  }
  
 if(se==BREAK_IN_TIME)
  {
  sprintf(valStr,"%d ms",breakInTime*10);
  displayStr(valStr);
  }  
  
if(se==BANDS24)
  {
    if(bands24 == 0)
    {
      displayStr("No");
    }
    else
    {
        displayStr("Yes");
    }
  }
}

int readConfig(void)
{
FILE * conffile;
char variable[50];
char value[100];
char vname[20];

conffile=fopen("/home/pi/Langstone/Langstone_Pluto.conf","r");

if(conffile==NULL)
  {
    return -1;
  }

while(fscanf(conffile,"%49s %99s [^\n]\n",variable,value) !=EOF)
  {
     
    if(strstr(variable,"CW_IDENT"))
      {
       value[MORSEIDENTLENGTH-1]=0;           //force to maximum length if necessary
       strcpy(morseIdent,value);
      }
    if(strstr(variable,"CWID_KEY_DOWN_TIME"))
      {
        sscanf(value,"%d",&CWIDkeyDownTime);
        CWIDkeyDownTime=CWIDkeyDownTime*100;
      }

    
    for(int b=0;b<numband;b++)
    {
    sprintf(vname,"bandFreq%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%lf",&bandFreq[b]);
    sprintf(vname,"bandMode%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandMode[b]);   
    sprintf(vname,"bandTxOffSet%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%lf",&bandTxOffset[b]); 
    sprintf(vname,"bandTxHarmonic%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandTxHarmonic[b]);             
    sprintf(vname,"bandRxOffSet%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%lf",&bandRxOffset[b]);      
    sprintf(vname,"bandRxHarmonic%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandRxHarmonic[b]);   
    sprintf(vname,"bandRepShift%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%lf",&bandRepShift[b]);
    sprintf(vname,"bandCTCSS%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandCTCSS[b]);
    sprintf(vname,"bandDuplex%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandDuplex[b]);  
 
 //handle old config file format by converting bandbits to bandbitsRx and BandbitsTx   
    sprintf(vname,"bandBits%02d",b);
    if(strstr(variable,vname)) 
    {
    sscanf(value,"%d",&bandBitsRx[b]); 
    sscanf(value,"%d",&bandBitsTx[b]);
    }
       
    sprintf(vname,"bandRxBits%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandBitsRx[b]); 
    sprintf(vname,"bandTxBits%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandBitsTx[b]);    
    sprintf(vname,"bandFFTRef%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandFFTRef[b]);     
    sprintf(vname,"bandSquelchUSB%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandSquelch[b][USB]);
    sprintf(vname,"bandSquelchLSB%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandSquelch[b][LSB]);
    sprintf(vname,"bandSquelchCW%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandSquelch[b][CW]);
    sprintf(vname,"bandSquelchCWN%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandSquelch[b][CWN]);
    sprintf(vname,"bandSquelchFM%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandSquelch[b][FM]);
    sprintf(vname,"bandSquelchAM%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandSquelch[b][AM]);     
    sprintf(vname,"bandTxAtt%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandTxAtt[b]);
    sprintf(vname,"bandRxGain%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandRxGain[b]);    
    sprintf(vname,"bandSmeterZero%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%f",&bandSmeterZero[b]);
    sprintf(vname,"bandSSBFiltLow%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandSSBFiltLow[b]); 
    sprintf(vname,"bandSSBFiltHigh%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandSSBFiltHigh[b]); 
    sprintf(vname,"bandFFTBW%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandFFTBW[b]);    
    }

    
    if(strstr(variable,"currentBand")) sscanf(value,"%d",&band);
    if(strstr(variable,"tuneDigit")) sscanf(value,"%d",&tuneDigit);   
    if(strstr(variable,"mode")) sscanf(value,"%d",&mode);
    if(strstr(variable,"SSBMic")) sscanf(value,"%d",&SSBMic);
    if(strstr(variable,"FMMic")) sscanf(value,"%d",&FMMic);
    if(strstr(variable,"AMMic")) sscanf(value,"%d",&AMMic);
    if(strstr(variable,"volume")) sscanf(value,"%d",&volume);
    if(strstr(variable,"breakInTime")) sscanf(value,"%d",&breakInTime);
    if(strstr(variable,"bandBitsToPluto")) sscanf(value,"%d",&bandBitsToPluto);
    if(strstr(variable,"bands24")) sscanf(value,"%d",&bands24);
    if(mode>nummode-1) mode=0;
            
  }

fclose(conffile);
return 0;

}

int writeConfig(void)
{
FILE * conffile;
char variable[80];
int value;

bandFreq[band]=freq;

for(int i=0;i<MORSEIDENTLENGTH;i++)                                                 //trim morse Ident to remove redundant spaces.
  {
  if((morseIdent[i]==95) && (morseIdent[i+1]==95))                    //find double space
    {
      morseIdent[i]=0;                                                //terminate string here
      break;
    }
  }


conffile=fopen("/home/pi/Langstone/Langstone_Pluto.conf","w");

if(conffile==NULL)
  {
    return -1;
  }

fprintf(conffile,"CW_IDENT %s\n",morseIdent);
fprintf(conffile,"CWID_KEY_DOWN_TIME %d\n",CWIDkeyDownTime/100);

for(int b=0;b<numband;b++)
{
  fprintf(conffile,"bandFreq%02d %lf\n",b,bandFreq[b]);
  fprintf(conffile,"bandMode%02d %d\n",b,bandMode[b]);
  fprintf(conffile,"bandTxOffSet%02d %lf\n",b,bandTxOffset[b]);
  fprintf(conffile,"bandTxHarmonic%02d %d\n",b,bandTxHarmonic[b]);
  fprintf(conffile,"bandRxOffSet%02d %lf\n",b,bandRxOffset[b]);
  fprintf(conffile,"bandRxHarmonic%02d %d\n",b,bandRxHarmonic[b]);
  fprintf(conffile,"bandRepShift%02d %lf\n",b,bandRepShift[b]);
  fprintf(conffile,"bandCTCSS%02d %d\n",b,bandCTCSS[b]);
  fprintf(conffile,"bandDuplex%02d %d\n",b,bandDuplex[b]);
  fprintf(conffile,"bandRxBits%02d %d\n",b,bandBitsRx[b]);
  fprintf(conffile,"bandTxBits%02d %d\n",b,bandBitsTx[b]);
  fprintf(conffile,"bandFFTRef%02d %d\n",b,bandFFTRef[b]);
  fprintf(conffile,"bandSquelchUSB%02d %d\n",b,bandSquelch[b][USB]);
  fprintf(conffile,"bandSquelchLSB%02d %d\n",b,bandSquelch[b][LSB]);
  fprintf(conffile,"bandSquelchCW%02d %d\n",b,bandSquelch[b][CW]);
  fprintf(conffile,"bandSquelchCWN%02d %d\n",b,bandSquelch[b][CWN]);
  fprintf(conffile,"bandSquelchFM%02d %d\n",b,bandSquelch[b][FM]);
  fprintf(conffile,"bandSquelchAM%02d %d\n",b,bandSquelch[b][AM]);
  fprintf(conffile,"bandTxAtt%02d %d\n",b,bandTxAtt[b]);
  fprintf(conffile,"bandRxGain%02d %d\n",b,bandRxGain[b]);
  fprintf(conffile,"bandSmeterZero%02d %f\n",b,bandSmeterZero[b]);
  fprintf(conffile,"bandSSBFiltLow%02d %d\n",b,bandSSBFiltLow[b]);
  fprintf(conffile,"bandSSBFiltHigh%02d %d\n",b,bandSSBFiltHigh[b]);
  fprintf(conffile,"bandFFTBW%02d %d\n",b,bandFFTBW[b]);    
}

fprintf(conffile,"currentBand %d\n",band);
fprintf(conffile,"tuneDigit %d\n",tuneDigit);
fprintf(conffile,"mode %d\n",mode);
fprintf(conffile,"SSBMic %d\n",SSBMic);
fprintf(conffile,"FMMic %d\n",FMMic);
fprintf(conffile,"AMMic %d\n",AMMic);
fprintf(conffile,"volume %d\n",volume);
fprintf(conffile,"breakInTime %d\n",breakInTime);
fprintf(conffile,"bandBitsToPluto %d\n",bandBitsToPluto);
fprintf(conffile,"bands24 %d\n",bands24);

fclose(conffile);
return 0;

}


