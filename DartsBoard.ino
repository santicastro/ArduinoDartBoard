
#include <LiquidCrystal.h>
#include <Wire.h>

const int NONE=0;

int inPins[] = {
  4, 5, 6, 7};
int outPins[] = {
  A0, A1, A2, A3};


typedef struct button {
  byte value;
  byte type;
};

// type values are
#define DOUBLE_BUTTON 1
#define TRIPLE_BUTTON 2
#define NORMAL_BUTTON 4
#define INTERNAL_BUTTON 12 //8 + 4, so it's also normal button
#define EXTERNAL_BUTTON 20 // 16 + 4

struct button buttons[169];

int player_count = 2, current_player;

LiquidCrystal lcd(0x0);

byte char_0_hits[8] = { 
  B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00000 };
byte char_1_hits[8] = { 
  B10000, B01000, B01000, B00100, B00100, B00010, B00010, B00001 };
byte char_2_hits[8] = { 
  B10001, B01010, B01010, B00100, B00100, B01010, B01010, B10001 };
byte char_3_hits[8] = { 
  B11111, B11011, B11011, B10101, B10101, B11011, B11011, B11111 };
byte char_selection[8] = { 
  B10000, B01000, B00100, B00010, B00010, B00100, B01000, B10000 };
byte char_dart[8] = { 
  B01010, B11011, B01110, B01110, B01110, B01110, B00100, B00100 };
#define SELECTION_CHAR 4
#define DART_CHAR 5

#define BUTTON_NEXT 2
#define BUTTON_OPTION 3
void setup() {
  int i;
  for(i=0; i<4; i++){
    pinMode(outPins[i], OUTPUT);
    pinMode(inPins[i], INPUT);
  }
  pinMode(BUTTON_NEXT, INPUT);
  digitalWrite(BUTTON_NEXT, HIGH);
  pinMode(BUTTON_OPTION, INPUT);
  digitalWrite(BUTTON_OPTION, HIGH);
  Serial.begin(9600);
  Serial.println("started");

  lcd.begin(20, 4);
  lcd.createChar(0, char_0_hits);
  lcd.createChar(1, char_1_hits);
  lcd.createChar(2, char_2_hits);
  lcd.createChar(3, char_3_hits);
  lcd.createChar(SELECTION_CHAR, char_selection);
  lcd.createChar(DART_CHAR, char_dart);

  lcd.setCursor(0,0);
  lcd.print("-= Darts =-");

  setupButtonMatrix();

  setupSounds();
  // playWin();
  //mapButtons();
  startGame();
}

int readButtons(){
  if(digitalRead(BUTTON_NEXT)==LOW){
    return BUTTON_NEXT;
  }
  if(digitalRead(BUTTON_OPTION)==LOW){
    return BUTTON_OPTION;
  }
  return NONE;
}


void loop() {
  loopCricket();
}

void buttonTest(){
  int button;
  button=readMatrix();
  if(button!=NONE){
    lcd.setCursor(0,2);
    lcd.print(button);
    lcd.print("    ");

    lcd.setCursor(0,3);
    struct button b=buttons[button];
    lcd.print(b.value);
    lcd.print(' ');
    lcd.print(b.type);
    lcd.print(' ');

    //Serial.println(button);
    delay(200);
  }
}

int readMatrix(){
  int row, input;
  for(row=0; row<10; row++){
    // Write row
    PORTC = (PINC & B11110000) | row;
    delayMicroseconds(50);
    // Read columns
    input = ((PIND & B11110000) >> 4) ^ B1111 ;
    // Reset rows
    if(input!=0){ //this means that a button is pressed
      PORTC = (PINC & B11110000);
      return row<<4 | input;      
    }
  }
  PORTC = (PINC & B11110000);
  return 0;
}


void mapButtons(){
  int  last_code=0, code=0;

  // To map the buttons press the one showing on LCD and after finish replace the initialization
  // function with the text received on serial port");
  for(byte value=1; value<=20; value++){
    lcd.setCursor(0,0);
    lcd.print("-- "); 
    lcd.print(value);
    lcd.print(" double   "); 
    // read double value
    while(code == last_code || code==0){
      code = readMatrixW();
      delay(50);
    }
    last_code = code;
    printInitCode(value, code, DOUBLE_BUTTON);

    lcd.setCursor(0,0);
    lcd.print("-- "); 
    lcd.print(value);
    lcd.print(" external   "); 
    // read double value
    while(code == last_code || code==0){
      code = readMatrixW();
      delay(50);
    }
    last_code = code;
    printInitCode(value, code,  EXTERNAL_BUTTON);

    lcd.setCursor(0,0);
    lcd.print("-- "); 
    lcd.print(value);
    lcd.print(" triple    "); 
    // read double value
    while(code == last_code || code==0){
      code = readMatrixW();
      delay(50);
    }
    last_code = code;
    printInitCode(value, code,  TRIPLE_BUTTON);

    lcd.setCursor(0,0);
    lcd.print("-- "); 
    lcd.print(value);
    lcd.print(" internal   "); 
    // read double value
    while(code == last_code || code==0){
      code = readMatrixW();
      delay(50);
    }
    last_code = code;
    printInitCode(value, code,  INTERNAL_BUTTON);
  }
}

void printInitCode(byte value, byte code, byte buttonType){
  Serial.print("button btn");
  Serial.print(code);
  Serial.print(" = {");
  Serial.print(value);
  Serial.print(",");
  Serial.print(buttonType);
  Serial.println("};");

  Serial.print("buttons[");
  Serial.print(code);
  Serial.print("] = btn");
  Serial.print(code);
  Serial.println(";");
}

int readMatrixW(){
  int val=0;
  //read internal value
  while(val==0){
    val = readMatrix();
  }  
  lcd.setCursor(0,3);
  lcd.print(val);
  return val;
}



///////////////////////////////////////////////////////
///// CRICKET GAME
///////////////////////////////////////////////////////

#define P15 0
#define P16 1
#define P17 2
#define P18 3
#define P19 4
#define P20 5
#define P25 6

struct score{
  int points;
  byte cells[7];
};

int getCellIndex(byte value){
  if(value==25){
    return P25;
  }
  else if(value>=15 && value<=20){
    return value-15;
  }
  return -1;
}

struct score scores[4];
int shootCount=3;

void startGame(){
  shootCount=0;

//  scores[0].points = 75;
//  for(int i=P15; i<P25; i++){
//    scores[0].cells[i]=3;
//  }
//  scores[0].cells[P25] = 2;
//
//
//  scores[1].points = 15;
//  for(int i=P15; i<P25; i++){
//    scores[1].cells[i]=3;
//  }
//  scores[1].cells[P25] = 2;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("    -= Cricket =-   ");
  lcd.setCursor(0,1);
  lcd.print("No. of players: ");
  lcd.print(player_count);

  lcd.setCursor(0,3);
  lcd.print("Starting...");
  playTheme();
  printScores();
}

boolean turnFinished(){
  return shootCount==3;
}

void loopCricket(){
  unsigned long time;
  while(true){
    if(gameFinished()){
      shootCount=3;
      showResults();
      if(checkFunctionButtons()!=NONE){
        asm volatile ("  jmp 0");
      }
    }
    else if(turnFinished()){
      //blink
      if(millis()-time<850){
        lcd.noDisplay();
      }
      else {
        lcd.display();
        if(millis()-time>1500){
          time = millis();
        }
      }
      checkFunctionButtons();
    }
    else{
      lcd.display();
      boolean wait = true;
      while(wait){
        int code = readMatrix();
        if(code!=0){
          addShoot(buttons[code]);
          wait=false;
        }
        if(checkFunctionButtons()!=NONE){
          wait=false;
          ;
        }
      }
    }
  }
}
int checkFunctionButtons(){
  int code = readButtons();
  if(code!=NONE){
    if(code==BUTTON_OPTION){
      Serial.println("paso");
      addShoot(buttons[0]);
    }
    else if(code==BUTTON_NEXT){
      nextTurn(); 
    }
    delay(5);
    while(code==readButtons()){
      delayMicroseconds(50);
    }
  }
  return code;
}

void showResults(){
  int winnerIndex=0;
  boolean already_played=false;
  for(int player=1; player<player_count; player++){
    if(scores[player].points>scores[winnerIndex].points){
      winnerIndex = player;
    }
    unsigned long time;
    boolean state=true;
    printScores();
    while(true){
      if(millis()-time<1250){
        if(state==false){
          state=true;
          for(int player = 0; player< player_count; player++){
            lcd.setCursor(6,player);
            if(player==winnerIndex){
              lcd.print("Winner !!!    "); 
            }
            else{
              lcd.print("Loser !!!     "); 
            }
          }
        }
        if(!already_played){
          playWin();
          already_played=true;
        }
      }
      else if(millis()-time < 2500){
        if(state=true){
          state=false;
          printScores();
        }
      }
      else{
        time=millis();
        state=false; 
      }
      if(checkFunctionButtons()!=NONE){
        asm volatile ("  jmp 0");
      }
      delay(50);
    }
  }

}


void addShoot(struct button but){
  if(shootCount<3){
    boolean sound_failed = true;
    int sound_multiplier = 0;
    if(but.value==0){ // do nothing
    }
    else{

      int cellIndex=getCellIndex(but.value);
      if(cellIndex>=0){
        //calculate multiplier
        int multiplier = 1;
        if(but.type & TRIPLE_BUTTON){
          multiplier = 3;
        }
        else if(but.type & DOUBLE_BUTTON){
          multiplier = 2;
        }
        sound_multiplier = multiplier;

        int sum = scores[current_player].cells[cellIndex] + multiplier;
        if(sum>3){
          scores[current_player].cells[cellIndex] = 3;
          multiplier = sum-3;

          if(!allClosed(cellIndex)){
            scores[current_player].points += but.value*multiplier;
            sound_failed=false;
          }

        }
        else{
          scores[current_player].cells[cellIndex] = sum;
          sound_failed=false;
        }
      }
    }
    shootCount++; 
    if(sound_failed){
      playDartFail(1);  
    }
    else{
      playDartOk(sound_multiplier);
    }
    printScores();

  }
}

boolean allClosed(int cellIndex){
  int player;
  for(player=0; player<player_count; player++){
    if(scores[player].cells[cellIndex]<3){
      return false;
    }  
  }
  return true;
}

boolean gameFinished(){
  int player;
  for(player=0; player<player_count; player++){
    for(int i=P15; i<=P25; i++){
      if(scores[player].cells[i]!=3){
        return false;
      }
    }
  }
  return true;
}


void nextTurn(){
  if(gameFinished())
    return;
  current_player = (current_player + 1) % player_count;
  shootCount=0;
  printScores();
}

void printScore(struct score sc, int lineNumber, boolean current){
  lcd.setCursor(0, lineNumber);
  if(current){
    lcd.write(byte(SELECTION_CHAR));
  }
  else{
    lcd.print(' ');
  }
  if(sc.points<1000){
    lcd.print(' ');
  }
  if(sc.points<100){
    lcd.print(' ');
  }
  if(sc.points<10){
    lcd.print(' ');
  }
  lcd.print(sc.points);
  lcd.print(' ');
  lcd.write(byte(sc.cells[P15]));
  lcd.print(' ');
  lcd.write(byte(sc.cells[P16]));
  lcd.print(' ');
  lcd.write(byte(sc.cells[P17]));
  lcd.print(' ');
  lcd.write(byte(sc.cells[P18]));
  lcd.print(' ');
  lcd.write(byte(sc.cells[P19]));
  lcd.print(' ');
  lcd.write(byte(sc.cells[P20]));
  lcd.print(' ');
  lcd.write(byte(sc.cells[P25]));
}

void printScores(){
  lcd.clear();
  int player;
  for(player=0; player<player_count; player++){
    printScore(scores[player], player, current_player==player);
  }
  if(shootCount<3){
    lcd.setCursor(19,3);
    lcd.write(byte(DART_CHAR));
  }
  if(shootCount<2){
    lcd.setCursor(19,2);
    lcd.write(byte(DART_CHAR));
  }
  if(shootCount<1){
    lcd.setCursor(19,1);
    lcd.write(byte(DART_CHAR));
  }
}



