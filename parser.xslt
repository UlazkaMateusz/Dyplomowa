<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text"/>
<xsl:template match="/">


    // Domyślna wartośc tempa 120
    // note_time = 60 * 1000 / tempo / divisions

<xsl:choose>
    <xsl:when test="count(score-partwise/part/measure/direction/sound) > 0">
        #define note_time <xsl:value-of select="round(60 * 1000 div score-partwise/part/measure/direction/sound/@tempo div score-partwise/part/measure/attributes/divisions)"/>

        #define note_time_dot <xsl:value-of select="round(1.5 * 60 * 1000 div score-partwise/part/measure/direction/sound/@tempo div score-partwise/part/measure/attributes/divisions)"/>
    </xsl:when>
    <xsl:otherwise>
        #define note_time <xsl:value-of select="round(60 * 1000 div 120 div score-partwise/part/measure/attributes/divisions)"/>

        #define note_time_dot <xsl:value-of select="round(1.5 * 60 * 1000 div 120 div score-partwise/part/measure/attributes/divisions)"/>
    </xsl:otherwise>
</xsl:choose>

<xsl:text disable-output-escaping="yes">
    // Ilosć cykli wykorzystywana w _delay_ms    // Ilosć cykli wykorzystywana w _delay_ms
    // Domyślna wartość to 1.2 MHZ
    #define F_CPU 1200000

    #include &lt;avr/io.h&gt;
    #include &lt;avr/interrupt.h&gt;
    #include &lt;avr/pgmspace.h&gt;
    #include &lt;util/delay.h&gt;
    #include &lt;stdbool.h&gt; 
    
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

    #define	N_1	    (_BV(CS00))
    #define	N_8	    (_BV(CS01))
    #define	N_64	(_BV(CS01)|_BV(CS00))
    #define	N_256	(_BV(CS02))
    #define	N_1024	(_BV(CS02)|_BV(CS00))

    // Struktura określająca pojedyńczą nutę
    // N jest preskalerem o wartościa 1, 8, 64, 256 oraz 1024
    // OCRxn jest wartoscią w przedziale 0..255
    typedef struct s_note {
        uint8_t N;
        uint8_t OCRxn; 
    } note_t;

    // Struktura przetrzymująca pojedyńczą oktawę
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


    // Zapisanie 8 oktaw w pamięci flash
    PROGMEM const octave_t octaves[] = {
        { 
        .note_C  =   {N_256, 142},
        .note_CS =  {N_256, 134}, 
        .note_D  =   {N_256, 127},
        .note_DS =  {N_256, 120}, 
        .note_E  =   {N_256, 113},
        .note_F  =   {N_256, 106},
        .note_FS =  {N_256, 100}, 
        .note_G  =   {N_256, 95}, 
        .note_GS =  {N_256, 89},  
        .note_A  =   {N_256, 84}, 
        .note_AS =  {N_256, 79},  
        .note_B  =   {N_256, 75}  
        },
        {
        .note_C  =   {N_256, 71}, 
        .note_CS =   {N_256, 67},  
        .note_D  =   {N_64,  254},
        .note_DS =   {N_64,  240}, 
        .note_E  =   {N_64,  227},
        .note_F  =   {N_64,  214},
        .note_FS =   {N_64,  202}, 
        .note_G  =   {N_64,  190},
        .note_GS =   {N_64,  180}, 
        .note_A  =   {N_64,  169},
        .note_AS =   {N_64,  160}, 
        .note_B  =   {N_64,  151} 
        },
        { 
        .note_C  =  {N_64, 142},
        .note_CS =  {N_64, 134},
        .note_D  =  {N_64, 127},
        .note_DS =  {N_64, 120},
        .note_E  =  {N_64, 113},
        .note_F  =  {N_64, 106},
        .note_FS =  {N_64, 100},
        .note_G  =  {N_64, 95},
        .note_GS =  {N_64, 89},
        .note_A  =  {N_64, 84},
        .note_AS =  {N_64, 79},
        .note_B  =  {N_64, 75}
        },
        {
        .note_C  =  {N_64, 71},
        .note_CS =  {N_64, 67},
        .note_D  =  {N_64, 63},
        .note_DS =  {N_64, 59},
        .note_E  =  {N_64, 56},
        .note_F  =  {N_64, 53},
        .note_FS =  {N_64, 50},
        .note_G  =  {N_64, 47},
        .note_GS =  {N_64, 44},
        .note_A  =  {N_64, 42},
        .note_AS =  {N_64, 39},
        .note_B  =  {N_64, 37}
        },
        {
        .note_C  =  {N_64, 35},
        .note_CS =  {N_64, 33},
        .note_D  =  {N_8, 254},
        .note_DS =  {N_8, 240},
        .note_E  =  {N_8, 227},
        .note_F  =  {N_8, 214},
        .note_FS =  {N_8, 202},
        .note_G  =  {N_8, 190},
        .note_GS =  {N_8, 180},
        .note_A  =  {N_8, 169},
        .note_AS =  {N_8, 160},
        .note_B  =  {N_8, 151}
        },
        {
        .note_C  =  {N_8, 142},
        .note_CS =  {N_8, 134},
        .note_D  =  {N_8, 127},
        .note_DS =  {N_8, 120},
        .note_E  =  {N_8, 113},
        .note_F  =  {N_8, 106},
        .note_FS =  {N_8, 100},
        .note_G  =  {N_8, 95},
        .note_GS =  {N_8, 89},
        .note_A  =  {N_8, 84},
        .note_AS =  {N_8, 79},
        .note_B  =  {N_8, 75}
        },
        {
        .note_C  =  {N_8, 71},
        .note_CS =  {N_8, 67},
        .note_D  =  {N_8, 63},
        .note_DS =  {N_8, 59},
        .note_E  =  {N_8, 56},
        .note_F  =  {N_8, 53},
        .note_FS =  {N_8, 50},
        .note_G  =  {N_8, 47},
        .note_GS =  {N_8, 44},
        .note_A  =  {N_8, 42},
        .note_AS =  {N_8, 39},
        .note_B  =  {N_8, 37}
        },
        {
        .note_C  =  {N_8, 34},
        .note_CS =  {N_8, 33},
        .note_D  =  {N_1, 254},
        .note_DS =  {N_1, 240},
        .note_E  =  {N_1, 227},
        .note_F  =  {N_1, 214},
        .note_FS =  {N_1, 202},
        .note_G  =  {N_1, 190},
        .note_GS =  {N_1, 180},
        .note_A  =  {N_1, 169},
        .note_AS =  {N_1, 160},
        .note_B  =  {N_1, 151}
        },
        {
        .note_C  =  {N_1, 142},
        .note_CS =  {N_1, 134},
        .note_D  =  {N_1, 127},
        .note_DS =  {N_1, 120},
        .note_E  =  {N_1, 113},
        .note_F  =  {N_1, 106},
        .note_FS =  {N_1, 100},
        .note_G  =  {N_1, 95},
        .note_GS =  {N_1, 89},
        .note_A  =  {N_1, 84},
        .note_AS =  {N_1, 79},
        .note_B  =  {N_1, 75}
        }
    };

    inline void init()
    {

        DDRB |= _BV(PB0);       // Ustawienie portu PB0 jako wyjście
        TCCR0A |= _BV(WGM01);   // Ustawienie trybu CTC
        TCCR0A |= _BV(COM0A0);  // Wyczyszczenie flagi przy wartosci z registru OCR0A
    }

    // Zamiana długości nuty na czas w ms oraz użycie makra stop_sound
    #define play_pause(duration) stop_sound(duration * note_time)
    
    // Zatrzymanie timera oraz odczekanie określonego czasu
    #define stop_sound(duration_ms) TCCR0B &amp;= ~(_BV(CS02) | _BV(CS01) | _BV(CS00)); _delay_ms(duration_ms)

    // Odczekanie czasu trwania nuty
    #define wait_note(duration) _delay_ms(duration * note_time)
    // Odczekanie czasu trwania nuty z kropką
    #define wait_note_with_dot(duration) _delay_ms(duration * note_time_dot)



    void set_note(int octave, notes note)
    {
        // Pobranie adresu odpowiedniej nuty w tablicy oktaw
        // pgm_read_word_near - pozwala odczytanie danych zapisanych w pamięci flash
        // sizeof(octave_t) * octave - wybór oktawy
        // sizeof(note_t) * note - wybór nuty w oktawie
        uint32_t ret = pgm_read_word_near(
                (uint8_t *)&amp;octaves 
                + sizeof(octave_t) * octave 
                + sizeof(note_t) * note);

        // Zmiana ze wskaźnika na strukturę
        note_t*  val = (note_t *)&amp;ret;

        // Ustawienie odpowiednich wartości rejestrów
        // F = F_CPU / (2 * N * (1 + OCRnx))
        // F - częstotliwość nuty
        // N - preskaler
        // OCRnx - wartość mieszcząca się w zakresie 0..255
        
        // Ustawienie preskalera na odpowiednią wartość
        TCCR0B = (TCCR0B &amp; ~(_BV(CS02) | _BV(CS01)| _BV(CS00))) | val-&gt;N;

        // Ustawienie wartości rejestru OCR0A
        OCR0A = val->OCRxn;
    }

</xsl:text>

int main() 
{
    init();
    while(1)
    {
<xsl:for-each select="score-partwise/part/measure/note">

<xsl:choose>
<xsl:when test="count(rest) > 0">
    play_pause(<xsl:value-of select="duration"/>);
</xsl:when>
<xsl:otherwise>
    set_note(
        <xsl:value-of select="pitch/octave"/>,
        <xsl:value-of select="pitch/step"/><xsl:choose><xsl:when test="accidental = &quot;flat&quot;">F</xsl:when><xsl:when test="accidental = &quot;sharp&quot;">S</xsl:when></xsl:choose>);
    <xsl:choose>
        <xsl:when test="count(dot) > 0">
            wait_note_with_dot(<xsl:value-of select="duration"/>);
        </xsl:when>
        <xsl:otherwise>
            wait_note(<xsl:value-of select="duration"/>);
        </xsl:otherwise>
    </xsl:choose>
</xsl:otherwise>
</xsl:choose>

</xsl:for-each>
    }
}
</xsl:template>
</xsl:stylesheet>

