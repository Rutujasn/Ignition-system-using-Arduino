const int pulsePin = 2;                 
const int camPin = 3;                   
const int outputPin = 7;              
unsigned long currentTime=0;
unsigned long highcurrentTime=0;
volatile unsigned long lastPulseTime = 0;
volatile unsigned long timeDifference = 0;
volatile unsigned long timeDifference1 = 0;
volatile unsigned long sumtimeDifference = 0;
volatile unsigned long avgtimeDifference;
volatile unsigned long eachdegreetime = 0; 
unsigned long pulseTime = 0;
volatile unsigned long BTDC = 0; 
unsigned long BTDCTime=0;
double Onerotationtime = 0;
float Onedegreetime = 0;
double RPM = 0;
bool camFallflag = false;
bool avg = true;
bool miss = false;
volatile int pulsecount = 0;


void setup() {
  pinMode(pulsePin, INPUT);
  pinMode(camPin, INPUT);
  pinMode(outputPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(pulsePin), pulseISR, RISING);
  attachInterrupt(digitalPinToInterrupt(camPin), camFall, FALLING);
  Serial.begin(2000000);
}

void loop() {
  if (timeDifference > 0) {
    Serial.print("TD: ");
    Serial.println(timeDifference);
    Serial.print("PC: ");
    Serial.println(pulsecount);

    // Calculate Average
    if (avg && pulsecount <= 4) {
      sumtimeDifference += timeDifference;
      if (pulsecount == 4) {
        timeDifference1 = timeDifference;
        avg = false;
        avgtimeDifference = sumtimeDifference / 4.0;
        Serial.println(avgtimeDifference);
        sumtimeDifference = 0;
        miss = true;
      }
    }


    // Each degree time -  BTDC
     if (pulsecount == 2) {
    
        eachdegreetime =  avgtimeDifference/6;
        int requiredDegree = 90;
        BTDC = eachdegreetime * requiredDegree ;
        pulseTime = currentTime + BTDC;
        //Serial.println(BTDC); 
    } 

    
      // Check if pulse generation condition is met
      if (camFallflag && currentTime >= pulseTime && currentTime <= pulseTime + 2000  ) {

        Serial.println("Cam");
        digitalWrite(outputPin, HIGH);
        delayMicroseconds(500);
        digitalWrite(outputPin, LOW);

        //highcurrentTime = currentTime;
        
      }

      // if(currentTime >= highcurrentTime + 1100){

      //   digitalWrite(outputPin, LOW);
      //   camFallflag = false; // Reset camFall flag
      //   highcurrentTime = 0;


      // }

      // RPM at 50 pulsecount
      if (pulsecount == 50){
      Onerotationtime = timeDifference1 * 60;
      
      RPM = (60000 / Onerotationtime) * 1000;
      Serial.print("RPM: ");
      Serial.println(RPM);
      
      }

    if (miss && timeDifference >  2 * avgtimeDifference) {      
      pulsecount = 0;   
      avg = true;
      miss = false;
      Serial.println("Missssssssssss");
      Serial.println();

      
    }
    timeDifference = 0;
  }
}


void pulseISR() {
  pulsecount++;
  currentTime = micros();
  timeDifference = currentTime - lastPulseTime;
  lastPulseTime = currentTime;
}

void camFall() {
  camFallflag = true; // Allow generating pulse in the next cycle
}
