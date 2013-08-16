

/* Play Melody
 * -----------
 *
 * Program to play a simple theme_melody
 *
 * Tones are created by quickly pulsing a speaker on and off 
 *   using PWM, to create signature frequencies.
 *
 * Each note has a frequency, created by varying the period of 
 *  vibration, measured in microseconds. We'll use pulse-width
 *  modulation (PWM) to create that vibration.
 
 * We calculate the pulse-width to be half the period; we pulse 
 *  the speaker HIGH for 'pulse-width' microseconds, then LOW 
 *  for 'pulse-width' microseconds.
 *  This pulsing creates a vibration of the desired frequency.
 *
 * (cleft) 2005 D. Cuartielles for K3
 * Refactoring and comments 2006 clay.shirky@nyu.edu
 * See NOTES in comments at end for possible improvements
 */

// TONES  ==========================================
// Start by defining the relationship between 
//       note, period, &  frequency. 
#define	c	3831	//	261	Hz
#define	d	3401	//	294	Hz
#define	e	3040	//	329	Hz
#define	f	2865	//	349	Hz
#define	g	2558	//	391	Hz
#define	gS	2410	//	415	Hz
#define	a	2273	//	440	Hz
#define	aS	2198	//	455	Hz
#define	b	2146	//	466	Hz
#define	cH	1912	//	523	Hz
#define	cSH	1805	//	554	Hz
#define	dH	1704	//	587	Hz
#define	dSH	1608	//	622	Hz
#define	eH	1517	//	659	Hz
#define	fH	1433	//	698	Hz
#define	fSH	1351	//	740	Hz
#define	gH	1276	//	784	Hz
#define	gSH	1205	//	830	Hz
#define	aH	1136	//	880	Hz
// Define a special note, 'R', to represent a rest
#define  R     0

// SETUP ============================================
// Set up speaker on a PWM pin (digital 9, 10 or 11)
int speakerOut = 9;

void setupSounds() { 
  pinMode(speakerOut, OUTPUT);
}


// Set overall tempo
long tempo = 10000;
// Set length of pause between notes
int pause = 1000;
// Loop variable to increase Rest length
int rest_count = 100; //<-BLETCHEROUS HACK; See NOTES

// Initialize core variables
int tone_ = 0;
int beat = 0;
long duration  = 0;

// PLAY TONE  ==============================================
// Pulse the speaker to play a tone for a particular duration
void playTone() {
  long elapsed_time = 0;
  if (tone_ > 0) { // if this isn't a Rest beat, while the tone has 
    //  played less long than 'duration', pulse speaker HIGH and LOW
    while (elapsed_time < duration) {

      digitalWrite(speakerOut,HIGH);
      delayMicroseconds(tone_ / 2);

      // DOWN
      digitalWrite(speakerOut, LOW);
      delayMicroseconds(tone_ / 2);

      // Keep track of how long we pulsed
      elapsed_time += (tone_);
    } 
  }
  else { // Rest beat; loop times delay
    for (int j = 0; j < rest_count; j++) { // See NOTE on rest_count
      delayMicroseconds(duration);  
    }                                
  }                                 
}

void playMelody(int melody[], int beats[], int lenght){
  //tone(8, 250,500);
  for (int i=0; i<lenght; i++) {
    tone_ = melody[i];
    beat = beats[i];

    duration = beat * tempo; // Set up timing

    playTone(); 
    // A pause between notes...
    delayMicroseconds(pause);
  }

}

// MELODY and TIMING  =======================================
//  theme_melody[] is an array of notes, accompanied by theme_beats[], 
//  which sets each note's relative length (higher #, longer note) 
int theme_melody[] = {  
  cH,  b,  g,  cH,  b,   e,  R,  cH,  c,  g, a, cH };
int theme_beats[]  = { 
  16, 16, 16,  8,  8,  16, 32, 16, 16, 16, 8, 8 }; 
int theme_lenght = sizeof(theme_melody) / sizeof(int); // Melody length, for looping.
void playTheme(){
  playMelody(theme_melody, theme_beats, theme_lenght);
}

int win_melody[] = {  
a, a, a, f, cH, a, f, cH, a, eH, eH, eH, fH, cH, gS, f, cH, a, aH, a, a, aH, 
gSH, gH, fSH, fH, fSH, R, aS, dSH, dH, cSH, cH, b, cH, R, f, gS, f, a, cH, a, 
cH, eH, aH, a, a, aH, gSH, gH, fSH, fH, fSH, R, aS, dSH, dH, cSH, cH, b, cH, 
R, f, gS, f, cH, a, f, c, a };
int win_beats[]  = { 
  
50, 50, 50, 35, 15, 50, 35, 15, 100, 50, 50, 50, 35, 15, 50, 35, 15, 100, 50, 35, 15, 50, 25, 25, 13, 13, 25, 20, 10, 50, 25, 25, 13, 13, 25, 20, 13, 50, 38, 13, 50, 38, 13, 100, 50, 35, 15, 50, 25, 25, 13, 13, 25, 20, 25, 50, 25, 25, 13, 13, 25, 20, 25, 50, 38, 13, 50, 38, 13, 100
}; 
int win_lenght = sizeof(win_melody) / sizeof(int); // Melody length, for looping.
void playWin(){
  playMelody(win_melody, win_beats, win_lenght);  
}

int dartok_melody[] = {   
  cH };
int dartok_beats[]  = {  
  24 }; 
int dartok_lenght = sizeof(dartok_melody) /  sizeof(int); // Melody length, for looping.
void playDartOk(int repeat){
  while(repeat-- > 0){
    playMelody(dartok_melody, dartok_beats, dartok_lenght);
    if(repeat>0){
      delay(10);
    }
  }
}

int dartfail_melody[] = {  
  e};
int dartfail_beats[]  = { 
  24 }; 
int dartfail_lenght = sizeof(dartfail_melody) /  sizeof(int); // Melody length, for looping.
void playDartFail(int repeat){
  while(repeat-- > 0){
    playMelody(dartfail_melody, dartfail_beats, dartfail_lenght);  
    if(repeat>0){
      delay(10);
    }
  }
}
/*
 * NOTES
 * The program purports to hold a tone for 'duration' microseconds.
 *  Lies lies lies! It holds for at least 'duration' microseconds, _plus_
 *  any overhead created by incremeting elapsed_time (could be in excess of 
 *  3K microseconds) _plus_ overhead of looping and two digitalWrites()
 *  
 * As a result, a tone of 'duration' plays much more slowly than a rest
 *  of 'duration.' rest_count creates a loop variable to bring 'rest' theme_beats 
 *  in line with 'tone' theme_beats of the same length. 
 * 
 * rest_count will be affected by chip architecture and speed, as well as 
 *  overhead from any program mods. Past behavior is no guarantee of future 
 *  performance. Your mileage may vary. Light fuse and get away.
 *  
 * This could use a number of enhancements:
 * ADD code to let the programmer specify how many times the theme_melody should
 *     loop before stopping
 * ADD another octave
 * MOVE tempo, pause, and rest_count to #define statements
 * RE-WRITE to include volume, using analogWrite, as with the second program at
 *          http://www.arduino.cc/en/Tutorial/PlayMelody
 * ADD code to make the tempo settable by pot or other input device
 * ADD code to take tempo or volume settable by serial communication 
 *          (Requires 0005 or higher.)
 * ADD code to create a tone offset (higer or lower) through pot etc
 * REPLACE random theme_melody with opening bars to 'Smoke on the Water'
 */


