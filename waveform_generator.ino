/*
 *  Arduino Uno waveform generator using R-2R resistor ladder as DAC
 *  ~50KHz max frequency
 */

#define LT_LEN ((uint16_t)256) //Lookup table length
#define DEFAULT_FREQ 100.0 // default waveform frequency
#define F_CLK  331122.51496207058823529411764705 //aproximate value for the frequency the loop runs at

#define DAC_BITS 6
#define ACC_LEN 16

#define SINE 0
#define RAMP 1
#define TRIANGLE 2

uint8_t samples[LT_LEN] ={0}; /* Lookup table */
uint8_t max_dac_amplitude = ((uint8_t)1 << DAC_BITS) - 1; 
uint32_t prec_bits = ((uint32_t)1 << ACC_LEN);
uint32_t phase_acc_max = ((uint32_t)1 << ACC_LEN);
float frequency = DEFAULT_FREQ;

/* We will use them in ISR */
volatile uint8_t current_waveform = TRIANGLE; 
volatile uint32_t phase_acumulator = 0;
volatile uint32_t tuning_word = ((frequency * phase_acc_max) / F_CLK) * prec_bits;

void generate_waveform(){
uint32_t i;
  switch(current_waveform){
    case SINE:
      for (i = 0; i < LT_LEN; ++i) {
      samples[i] = (max_dac_amplitude * (1 + sin((float)i * 2 * PI / LT_LEN))/2);
      }
    break;
    case RAMP:
      for (i = 0; i < LT_LEN; ++i) {
        samples[i] = i*((float)max_dac_amplitude / LT_LEN);
      }
      break;
    case TRIANGLE:
      float dist = ((float)max_dac_amplitude / (LT_LEN/2));
      for(i=0;i<LT_LEN;++i){
        if(i<LT_LEN/2)
          samples[i] = (i+1)*dist;
        else 
          samples[i] = (LT_LEN - i - 1)*dist;
      }
      break;
  } 
}

void setup() {

  /* Configure portB as output */
  DDRB = 0xFF;
  generate_waveform();
}

/*
void changeFreq(void){

Interrupt function for changing frequency with pushbutton 
To be implemeted 

}
*/

void loop (){
/* Make the loop as fast as possible */
while(1){
    phase_acumulator += tuning_word;
    PORTB=samples[phase_acumulator >> 24]; /* Use last eight bits as lookup table index*/
}
}

