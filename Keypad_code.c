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

// Keypad Configuration
char keypadPort at PORTD;
char keyIndex;
char key = '\0';  // no key pressed set as default
unsigned char i;
char enteredPassword[4];  //  Array to store entered password
char correctPassword[4] = {'2', '0', '0', '3'};  // Array with the Predefined password
int match = 1;  // Variable to track if the passwords match

// Functions Configuration
char KeypadMapping(unsigned char keyIndex);
char GetKey();
void doorOpen();
void doorClose();
void CheckPassword();


void main() {

    // Initialize LCD
    Lcd_Init();
    Lcd_Cmd(_LCD_CURSOR_OFF);
    
    // Initialize Keypad
    Keypad_Init();

    // Set TRIS for ports
    TRISD = 0xF0;  // Configure PORTD for keypad
    TRISC = 0b00100000;  // Configure PORTC for servo motor
    PORTC = 0x00;  // Initialize PORTC to LOW

    while (1) {
      CheckPassword();
        delay_ms(500);
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


// Keypad Mapping
char KeypadMapping(unsigned char keyIndex) {
    switch (keyIndex) {
        case  1: return '1';
        case  2: return '4';
        case  3: return '7';
        case  4: return '*';
        case  5: return '2';
        case  6: return '5';
        case  7: return '8';
        case  8: return '0';
        case  9: return '3';
        case 10: return '6';
        case 11: return '9';
        case 12: return '#';
        case 13: return 'A';
        case 14: return 'B';
        case 15: return 'C';
        case 16: return 'D';
        default: return '\0';  // Invalid key
    }
}

// Get Key Function
char GetKey() {
    // Wait for a valid key press
    do {
        keyIndex = Keypad_Key_Click();  // Wait for a key click
        msDelay(50);  // Debounce delay
    } while (!keyIndex);
    // Map the key index to the corresponding character
    key = KeypadMapping(keyIndex);
    // Wait for key release
    while (Keypad_Key_Click()) {
        msDelay(50);  // Ensure the key is released
    }

    return key;
}

// Servo Motor (Door) Control Functions
void doorOpen() {
    unsigned int i;
    for (i = 0; i < 50; i++) { // Generate 50 pulses for ~1 second
        PORTC = PORTC | 0x08;  // Set RC3 HIGH (0x08 = 00001000)
        usDelay(1500);        // Pulse of 1500us for 90-degree position
        PORTC = PORTC & 0xF7;  // Clear RC3 LOW (0xF7 = 11110111)
        usDelay(18500);       // Remaining time for 20ms period
    }
}

void doorClose() {
    unsigned int i;
    for (i = 0; i < 50; i++) { // Generate 50 pulses for ~1 second
        PORTC = PORTC | 0x08;  // Set RC3 HIGH (0x08 = 00001000)
        usDelay(800);         // Pulse of 800us for 0-degree position
        PORTC = PORTC & 0xF7;  // Clear RC3 LOW (0xF7 = 11110111)
        usDelay(19200);       // Remaining time for 20ms period
    }
}

// Password Input and Validation Function
void CheckPassword() {

    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1, 1, "Enter Password:");

    // Read 4 digits from the keypad
    for (i = 0; i < 4; i++) {
        do {
            enteredPassword[i] = GetKey();  // Wait for a valid key press
        } while (!enteredPassword[i]);


        Lcd_Chr(2, i + 1, enteredPassword[i]);   // Display each entered digit
        msDelay(300);
    }

    // Compare the entered password with the correct password
    for (i = 0; i < 4; i++) {
        if (enteredPassword[i] != correctPassword[i]) {
            match = 0;  // Set match to 0 if any character doesn't match
            break;
        }
    }

    if (match) {
        Lcd_Cmd(_LCD_CLEAR);
        Lcd_Out(1, 1, "Welcome Home <3");
        doorOpen();  // Open the door
        msDelay(2000); // Keep the door open for 2 seconds
        doorClose();  // Close the door
    } else {
        Lcd_Cmd(_LCD_CLEAR);
        Lcd_Out(1, 1, "Wrong Password");
        PORTC = PORTC | 0x40; // Turn on buzzer (RC6)
        msDelay(1000);
        PORTC = PORTC & 0xBF; // Turn off buzzer

    msDelay(2000); // Delay before clearing screen
}
}
