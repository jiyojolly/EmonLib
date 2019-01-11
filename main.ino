Created by Jiyo Palatti, March 13 2015
GNU GPL

// This #include statement was automatically added by the Spark IDE.
#include <SparkIntervalTimer.h>

// This #include statement was automatically added by the Spark IDE.
#include <EmonLib_98b0.h>

volatile int i=0;               // Variable to use as a counter
volatile boolean zero_cross=0;  // Boolean to store a "switch" to tell us if we have crossed zero
int AC_pin = D6;                // Output to Opto Triac
int dim = 128;                    // Dimming level (0-128)  0 = on, 128 = 0ff
int inc=1;                      // counting up or down, 1=up, -1=down
int freqStep = 75;    // This is the delay-per-brightness step in microseconds.



double realPower;       
double apparentPower;   
double powerFactor;    
double supplyVoltage;  
double Irms;


IntervalTimer myTimer;
EnergyMonitor emon1; 


int dimmer(String command);
void zero_cross_detect(void);
void dim_check(void);

void setup()

{  
        
      
      emon1.voltage(0, 263.415, 12.398);  // Voltage: input pin, calibration, phase_shift
      emon1.current(1, .312);       // Current: input pin, calibration.
      //SPARK VARIABLES
      Spark.variable("currpow", &dim, INT);     //CHECK CURRENT POWER LEVEL
      Spark.variable("rpower", &realPower, DOUBLE);
      Spark.variable("apower", &apparentPower, DOUBLE);
      Spark.variable("pfactor", &powerFactor, DOUBLE);
      Spark.variable("Vin", &supplyVoltage, DOUBLE);
      Spark.variable("Iin", &Irms, DOUBLE);
      //SPARK FUNCTIONS
      Spark.function("powlevel", dimmer);   //SET REQUIRED POWERLEVEL
      
      pinMode(D0,INPUT);                                 //SET INTERRUPT PIN
      pinMode(AC_pin, OUTPUT);                          // Set the Triac pin as output
      
      attachInterrupt(D0, zero_cross_detect, RISING);    // Attach an Interupt to Pin 0) for Zero Cross Detection
      myTimer.begin(dim_check, freqStep, uSec);    
  
}




void measure()
{
  emon1.calcVI(20,2000); // Calculate all. No.of half wavelengths (crossings), time-out
  emon1.serialprint();
  realPower       = emon1.realPower;        //extract Real Power into variable
  apparentPower   = emon1.apparentPower;    //extract Apparent Power into variable
  powerFactor     = emon1.powerFactor;      //extract Power Factor into Variable
  supplyVoltage   = emon1.Vrms;             //extract Vrms into Variable
  Irms            = emon1.Irms;             //extract Irms into Variable
} 

void zero_cross_detect() {    
  zero_cross = true;               // set the boolean to true to tell our dimming function that a zero cross has occured
  i=0;
  digitalWrite(AC_pin, LOW);       // turn off TRIAC (and AC)
}                                 

void dim_check() {                   
  if(zero_cross == true) {              
    if(i>=dim) {                     
      digitalWrite(AC_pin, HIGH); // turn on light       
      i=0;  // reset time step counter                         
      zero_cross = false; //reset zero cross detection
    } 
    else {
      i++; // increment time step counter                     
    }                                
  }                                  
}                                   

int dimmer(String command)  //SPARK FUNCTION DEFINITION
{ 
    int x;
    x=command.toInt();
    dim=128-x;
    return dim;//for checking power level set 

}

void loop()
{
   
    
    if(dim<128)
    {
        measure();
        
    }
    
}
