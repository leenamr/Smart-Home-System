//milli second Delay
void msDelay(unsigned int mss);
unsigned int mss;
unsigned int ms=0;
unsigned int cc=0;

//micro second Delay
void usDelay(unsigned int uss);
unsigned int us = 0;

// LCD Configuration
sbit LCD_RS at RB4_bit;
sbit LCD_EN at RB5_bit;
sbit LCD_D4 at RB0_bit;
sbit LCD_D5 at RB1_bit;
sbit LCD_D6 at RB2_bit;
sbit LCD_D7 at RB3_bit;

sbit LCD_RS_Direction at TRISB4_bit;
sbit LCD_EN_Direction at TRISB5_bit;
sbit LCD_D4_Direction at TRISB0_bit;
sbit LCD_D5_Direction at TRISB1_bit;
sbit LCD_D6_Direction at TRISB2_bit;
sbit LCD_D7_Direction at TRISB3_bit;

//For LCD format
unsigned int tens , ones , decimal;

// Ultrasonic Function
int Distance();
int dist;


// Fan Functions
void CCPPWM_init(void);

void TempADCinit(void);
float result;
float TempADCread(void);
float tempreading;
unsigned int t;


void CCPPWM_init(void);
float temperature = 0;
unsigned int speed = 0;
float tRead;
unsigned char TempRead(void);

void main() {

    // Initialize LCD
    Lcd_Init();
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);

       // Initialize ADC and PWM modules
    TempADCinit();   // Initialize ADC for temperature sensor
    CCPPWM_init();
     
    // Set TRIS for ports
    TRISA = 0xFF;    // Configure PORTA as input (LM35 sensors)
    TRISC = 0b00100000;
    PORTC = 0x03;  // Initialize PORTC to LOW

    while (1) {

        // Display temperature on LCD
        Lcd_Out(1, 1, "Temp: ");
        tens = (unsigned int)temperature / 10;             // Extract tens digit
        ones = (unsigned int)temperature % 10;             // Extract ones digit
        decimal = ((temperature - (unsigned int)temperature) * 10); // Extract 1 decimal digit
        Lcd_Chr(1, 7, tens + '0');                        // Display tens digit
        Lcd_Chr(1, 8, ones + '0');                        // Display ones digit
        Lcd_Chr(1, 9, '.');                               // Display decimal point
        Lcd_Chr(1, 10, decimal + '0');                    // Display decimal digit
        Lcd_Chr(1, 11, 'C');                              // Display "C" for Celsius

        // Display speed on LCD
        Lcd_Out(2, 1, "Speed: ");
        tens = speed / 10;                                // Extract tens digit of speed
        ones = speed % 10;                                // Extract ones digit of speed
        Lcd_Chr(2, 8, tens + '0');                       // Display tens digit
        Lcd_Chr(2, 9, ones + '0');                       // Display ones digit
        Lcd_Out(2, 10, "%");                             // Display percentage sign

        CCPR1L = TempRead();
        dist =  Distance();
        Delay_ms(1000);        // Delay for stability


    }
}

// Delay Functions
void msDelay(unsigned int mss){
    for(ms = 0;ms < (mss); ms++){
      for(cc = 0; cc < 155; cc++);//1ms delay
    }
}

void usDelay(unsigned int uss){
    for(us = 0; us < uss; us++){
      asm NOP;//0.5 uS
      asm NOP;//0.5uS
    }
}

int Distance() {
    int d = 0;
T1CON = 0x10; // Use internal clock, no prescaler
delay_ms(10);

// Reset Timer1
TMR1H = 0;
TMR1L = 0;

    PORTC = PORTC | 0b00010000; // Trigger HIGH
    delay_ms(1);               // 1 ms delay
    PORTC = PORTC & 0b11101111; // Trigger LOW

    while (!(PORTC & 0b00100000));

    T1CON = T1CON | 0b00000001; // Start Timer

    while (PORTC & 0b00100000);

    T1CON = T1CON & 0b11111110; // Stop Timer

    d = (TMR1L | (TMR1H << 8)); // Read Timer1 value
    d = d / 58.82;           // Convert time to distance (cm)
    delay_ms(10);
    T1CON = 0x01;

    // Check Distance and Control LED
    if (d < 10) {
        PORTC = PORTC | 0x80;  // Turn ON LED at RC7 (Set bit 7 of PORTC)
    } else {
        PORTC = PORTC & 0x7F;  // Turn OFF LED at RC7 (Clear bit 7 of PORTC)
    }

    return d; // Return the calculated distance
}

//Fan Control
void CCPPWM_init(void){
  T2CON = 0x07;//enable Timer2 at Fosc/4 with 1:16 prescaler (8 uS percount 2000uS to count 250 counts)
  CCP1CON = 0x0C;//enable PWM for CCP1
  PR2 = 250;// 250 counts =8uS *250 =2ms period
  CCPR1L= 125;
}

void TempADCinit(void) {
    ADCON0 = 0x41;// ADC ON, Don't GO, Channel 0, Fosc/16
    ADCON1 = 0xC0;// All channels Analog, 500 KHz, right justified
}

float TempADCread(void) {
    float result;

    ADCON0 = ADCON0 | 0x04; // Start ADC conversion
    while (ADCON0 & 0x04); // Wait for conversion to complete

    result = (ADRESH << 8) | ADRESL;
    return result;
}

// Function to read temperature and set fan speed
unsigned char TempRead(void) {
    TempADCinit();
    tempreading = TempADCread();
    
    // Convert ADC value to voltage and then to temperature
    tRead = (float)tempreading / 1023.0;  // Normalize ADC value (0-1)
    tRead = tRead * 5.0;               // Convert to voltage (0-5V)
    tRead = tRead / 0.01;              // Convert voltage to temperature (10mV/°C)
     tRead = 40 - tRead;

    temperature = (tRead/4) + 50;               // Store the calculated temperature

    // Set fan speed based on temperature
    if (temperature > 50.0) {
        return speed = 99;
    } else if (temperature > 35.0) {
        return speed = 55;
    } else if (temperature > 27.0) {
        return speed = 35;
    } else {
        return speed = 0; // Fan OFF
    }
}
