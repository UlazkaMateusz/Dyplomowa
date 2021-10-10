


    // Domyślna wartośc tempa 120
    // note_time = 60 * 1000 / tempo / divisions


        #define note_time 125

        #define note_time_dot 188
    // Ilosć cykli wykorzystywana w _delay_ms
    // Domyślna wartość to 1.2 MHZ
    #define F_CPU 1200000

    #include <avr/io.h>
    #include <avr/interrupt.h>
    #include <avr/pgmspace.h>
    #include <util/delay.h>
    #include <stdbool.h> 
    
    // Pin pod którym podłączony jest brzęczyk
    #define	BUZZER_PIN	PB0

    // Tempo - ilość ćwierć nut w minucie
    // Disivions - wartość dzielenia ćwierćnuty 
    // Jeżeli divisions wynosi 1, to nuta o długości 1 jest ćwierćnutą
    // Jeżeli divisions wynosi 2, to nuta o długości 1 jest ósemką 

    // Pozycja nuty w tablicy oktawy
    typedef enum notes {
        C = 0,
        CS = 1,
        DF = 1,
        D = 2,
        DS = 3,
        EF = 3,
        E = 4,
        F = 5,
        FS = 6,
        GF = 6,
        G = 7,
        GS = 8,
        AF = 8,
        A = 9,
        AS = 10,
        BF = 10,
        B = 11,
    } notes;

    typedef enum notes notes;

    #define	N_1	(_BV(CS00))
    #define	N_8	(_BV(CS01))
    #define	N_64	(_BV(CS01)|_BV(CS00))
    #define	N_256	(_BV(CS02))
    #define	N_1024	(_BV(CS02)|_BV(CS00))

    // Struktura określająca pojedyńczą nutę
    // N jest preskalerem o wartościa 1, 8, 64, 256 oraz 1024
    // OCRxn jest XXX o wartościach w przedziale 0..255
    typedef struct s_note {
        uint8_t N;
        uint8_t OCRxn; 
    } note_t;

    typedef struct s_octave {
        note_t note_C;
        note_t note_CS;
        note_t note_D;
        note_t note_DS;
        note_t note_E;
        note_t note_F;
        note_t note_FS;
        note_t note_G;
        note_t note_GS;
        note_t note_A;
        note_t note_AS;
        note_t note_B;
    } octave_t;


    PROGMEM const octave_t octaves[] = {
        { // octave 0
        .note_C = {N_256, 142}, // 16.35 Hz
        .note_CS = {N_256, 134}, // 17.32 Hz
        .note_D = {N_256, 127}, // 18.35 Hz
        .note_DS = {N_256, 120}, // 19.45 Hz
        .note_E = {N_256, 113}, // 20.60 Hz
        .note_F = {N_256, 106}, // 21.83 Hz
        .note_FS = {N_256, 100}, // 23.12 Hz
        .note_G = {N_256, 95}, // 24.50 Hz
        .note_GS = {N_256, 89}, // 25.96 Hz
        .note_A = {N_256, 84}, // 27.50 Hz
        .note_AS = {N_256, 79}, // 29.14 Hz
        .note_B = {N_256, 75} // 30.87 Hz
        },
        { // octave 1
        .note_C = {N_256, 71}, // 32.70 Hz
        .note_CS = {N_256, 67}, // 34.65 Hz
        .note_D = {N_256, 63}, // 36.71 Hz
        .note_DS = {N_256, 59}, // 38.89 Hz
        .note_E = {N_256, 56}, // 41.20 Hz
        .note_F = {N_256, 53}, // 43.65 Hz
        .note_FS = {N_256, 50}, // 46.25 Hz
        .note_G = {N_256, 47}, // 49.00 Hz
        .note_GS = {N_256, 44}, // 51.91 Hz
        .note_A = {N_256, 42}, // 55.00 Hz
        .note_AS = {N_256, 39}, // 58.27 Hz
        .note_B = {N_256, 37} // 61.74 Hz
        },
        { // octave 2
        .note_C = {N_64, 142}, // 65.41 Hz
        .note_CS = {N_64, 134}, // 69.30 Hz
        .note_D = {N_64, 127}, // 73.42 Hz
        .note_DS = {N_64, 120}, // 77.78 Hz
        .note_E = {N_64, 113}, // 82.41 Hz
        .note_F = {N_64, 106}, // 87.31 Hz
        .note_FS = {N_64, 100}, // 92.50 Hz
        .note_G = {N_64, 95}, // 98.00 Hz
        .note_GS = {N_64, 89}, // 103.83 Hz
        .note_A = {N_64, 84}, // 110.00 Hz
        .note_AS = {N_64, 79}, // 116.54 Hz
        .note_B = {N_64, 75} // 123.47 Hz
        },
        { // octave 3
        .note_C = {N_64, 71}, // 130.81 Hz
        .note_CS = {N_64, 67}, // 138.59 Hz
        .note_D = {N_64, 63}, // 146.83 Hz
        .note_DS = {N_64, 59}, // 155.56 Hz
        .note_E = {N_64, 56}, // 164.81 Hz
        .note_F = {N_64, 53}, // 174.61 Hz
        .note_FS = {N_64, 50}, // 185.00 Hz
        .note_G = {N_64, 47}, // 196.00 Hz
        .note_GS = {N_64, 44}, // 207.65 Hz
        .note_A = {N_64, 42}, // 220.00 Hz
        .note_AS = {N_64, 39}, // 233.08 Hz
        .note_B = {N_64, 37} // 246.94 Hz
        },
        { // octave 4
        .note_C = {N_64, 35}, // 261.63 Hz
        .note_CS = {N_64, 33}, // 277.18 Hz
        .note_D = {N_64, 31}, // 293.66 Hz
        .note_DS = {N_64, 29}, // 311.13 Hz
        .note_E = {N_64, 27}, // 329.63 Hz
        .note_F = {N_64, 26}, // 349.23 Hz
        .note_FS = {N_64, 24}, // 369.99 Hz
        .note_G = {N_64, 23}, // 392.00 Hz
        .note_GS = {N_64, 22}, // 415.30 Hz
        .note_A = {N_64, 20}, // 440.00 Hz
        .note_AS = {N_64, 19}, // 466.16 Hz
        .note_B = {N_64, 18} // 493.88 Hz
        },
        {  // octave 5
        .note_C = {N_8, 142}, // 523.25 Hz
        .note_CS = {N_8, 134}, // 554.37 Hz
        .note_D = {N_8, 127}, // 587.33 Hz
        .note_DS = {N_8, 120}, // 622.25 Hz
        .note_E = {N_8, 113}, // 659.25 Hz
        .note_F = {N_8, 106}, // 349.23 Hz
        .note_FS = {N_8, 100}, // 369.99 Hz
        .note_G = {N_8, 95}, // 392.00 Hz
        .note_GS = {N_8, 89}, // 415.30 Hz
        .note_A = {N_8, 84}, // 440.00 Hz
        .note_AS = {N_8, 79}, // 466.16 Hz
        .note_B = {N_8, 75} // 493.88 Hz
        },
        {  // octave 6
        .note_C = {N_8, 71}, // 1046.50 Hz
        .note_CS = {N_8, 67}, // 1108.73 Hz
        .note_D = {N_8, 63}, // 1174.66 Hz
        .note_DS = {N_8, 59}, // 1244.51 Hz
        .note_E = {N_8, 56}, // 1318.51 Hz
        .note_F = {N_8, 53}, // 1396.91 Hz
        .note_FS = {N_8, 50}, // 1479.98 Hz
        .note_G = {N_8, 47}, // 1567.98 Hz
        .note_GS = {N_8, 44}, // 1661.22 Hz
        .note_A = {N_8, 42}, // 1760.00 Hz
        .note_AS = {N_8, 39}, // 1864.66 Hz
        .note_B = {N_8, 37} // 1975.53 Hz
        }
    };

    inline void init()
    {

        DDRB = 0b00000001; // set BUZZER pin as OUTPUT
        PORTB = 0b00000000; // set all pins to LOW
        TCCR0A |= (1<<WGM01); // set timer mode to Fast PWM
        TCCR0A |= (1<<COM0A0); 
    }

    #define play_pause(duration) stop_sound(duration * note_time)
    #define stop_sound(duration_ms) TCCR0B &= ~((1<<CS02)|(1<<CS01)|(1<<CS00)); _delay_ms(duration_ms)

    #define wait_note(duration) _delay_ms(duration * note_time)
    #define wait_note_with_dot(duration) _delay_ms(duration * note_time_dot)

    void set_note(int octave, notes note)
    {
        // Pobranie adresu odpowiedniej nuty w tablicy oktaw
        // pgm_read_word_near - pozwala na adresu danych zapisanych w pamięci XXX
        // sizeof(octave_t) * octave - wybór oktawy
        // sizeof(note_t) * note - wybór nuty w oktawie
        uint32_t ret = pgm_read_word_near((uint8_t *)&octaves + sizeof(octave_t) * octave + sizeof(note_t) * note);

        // Zmiana ze wskaźnika na strukturę
        note_t*  val = (note_t *)&ret;

        // Ustawienie XXX na XXX w celu XXX
        // F = F_CPU / (2 * N * (1 + OCRnx))
        // F - częstotliwość nuty
        // N - preskaler
        // OCRnx - XXX mieszcząca się w zakresie 0..255
        TCCR0B = (TCCR0B & ~((1<<CS02)|(1<<CS01)|(1<<CS00))) | val->N;
        OCR0A = val->OCRxn - 1;
    }



int main() 
{
    init();

    set_note(
        4,
        C);
    
            wait_note_with_dot(3);
        
    play_pause(1);

    play_pause(2);

    set_note(
        4,
        C);
    
            wait_note(2);
        
    set_note(
        4,
        E);
    
            wait_note_with_dot(3);
        
    set_note(
        4,
        C);
    
            wait_note(1);
        
    set_note(
        4,
        C);
    
            wait_note(2);
        
    set_note(
        3,
        B);
    
            wait_note(2);
        
    set_note(
        3,
        A);
    
            wait_note(8);
        
    set_note(
        3,
        G);
    
            wait_note(8);
        
    set_note(
        4,
        C);
    
            wait_note_with_dot(3);
        
    play_pause(1);

    play_pause(2);

    set_note(
        4,
        C);
    
            wait_note(2);
        
    set_note(
        4,
        E);
    
            wait_note_with_dot(3);
        
    set_note(
        4,
        C);
    
            wait_note(1);
        
    set_note(
        4,
        C);
    
            wait_note(2);
        
    set_note(
        3,
        B);
    
            wait_note(2);
        
    set_note(
        3,
        A);
    
            wait_note_with_dot(3);
        
    set_note(
        3,
        B);
    
            wait_note(1);
        
    set_note(
        3,
        B);
    
            wait_note(2);
        
    set_note(
        3,
        A);
    
            wait_note(2);
        
    set_note(
        3,
        G);
    
            wait_note(8);
        
    set_note(
        4,
        C);
    
            wait_note_with_dot(3);
        
    play_pause(1);

    play_pause(2);

    set_note(
        4,
        C);
    
            wait_note(2);
        
    set_note(
        4,
        E);
    
            wait_note_with_dot(3);
        
    set_note(
        4,
        C);
    
            wait_note(1);
        
    set_note(
        4,
        C);
    
            wait_note(2);
        
    set_note(
        3,
        B);
    
            wait_note(2);
        
    set_note(
        3,
        A);
    
            wait_note(8);
        
    set_note(
        3,
        G);
    
            wait_note(8);
        
    set_note(
        4,
        C);
    
            wait_note_with_dot(3);
        
    play_pause(1);

    play_pause(2);

    set_note(
        4,
        C);
    
            wait_note(2);
        
    set_note(
        4,
        E);
    
            wait_note_with_dot(3);
        
    set_note(
        4,
        C);
    
            wait_note(1);
        
    set_note(
        4,
        C);
    
            wait_note(2);
        
    set_note(
        3,
        B);
    
            wait_note(2);
        
    set_note(
        3,
        A);
    
            wait_note_with_dot(3);
        
    set_note(
        3,
        B);
    
            wait_note(1);
        
    set_note(
        3,
        B);
    
            wait_note(2);
        
    set_note(
        3,
        A);
    
            wait_note(2);
        
    set_note(
        3,
        G);
    
            wait_note(8);

    play_pause(16);

}
