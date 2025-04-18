
void UART3_Init_Config(unsigned long baudrate)
{
    PPS_Mapping(69, _INPUT, _U3RX);
    PPS_Mapping(68, _OUTPUT, _U3TX);
    UART3_Init(baudrate);
    Delay_ms(100);
}

void ADC_Init() {
    AD1CON1 = 0x0000;          // Manual sampling
    AD1CON1bits.SSRC = 7;      // Auto-convert
    AD1CON2 = 0x0000;          // AVDD and AVSS as reference, single channel
    AD1CON3 = 0x0002;          // ADC clock settings
    AD1CHS0bits.CH0SA = 27;    // Select AN26 as input channel for temp A OR 27 FOR B
    AD1CON1bits.ADON = 1;      // Turn on ADC
}

// Read ADC value
unsigned int ADC_Read() {
    AD1CON1bits.SAMP = 1;      // Start sampling
    Delay_us(10);              // Sample time
    AD1CON1bits.SAMP = 0;      // Stop sampling, start conversion
    while (!AD1CON1bits.DONE); // Wait for conversion to complete
    return ADC1BUF0;           // Return ADC result
}

void Read_Pressure_2bar() {
    unsigned int adc_value;
    float voltage, pressure_bar;
    char txt[64];

    float Vref = 5.0;          // ADC referans gerilimi
    float Vs = 5.0;            // Sensör besleme gerilimi
    float Pmin = 0.0;
    float Pmax = 2.0;

    // ADC'den veri oku (AN28)
    AD1CHS0bits.CH0SA = 28;       // AN28 seçildi
    adc_value = ADC_Read();       // ADC degeri alindi
    voltage = ((float)adc_value / 1023.0) * Vref;

    // Basinç hesaplama (formül datasheet'e göre)
    pressure_bar = ((voltage - 0.1 * Vs) / (0.8 * Vs)) * (Pmax - Pmin) + Pmin;

    // UART ile yazdir
    sprintf(txt, "ADC: %u, Vout: %.3f V, Pressure: %.3f bar\r\n", adc_value, voltage, pressure_bar);
    UART3_Write_Text(txt);
}

void main() {
    PLLFBD = 70;
    CLKDIV = 0x0000;

    UART3_Init_Config(9600);
    UART3_Write_Text("TEST STARTED!\r\n");

    ADC_Init();

    while (1) {
    Read_Pressure_2bar();
    Delay_ms(1000);
}

}
