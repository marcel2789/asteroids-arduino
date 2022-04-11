#include <LiquidCrystal_I2C.h>

// Sprites
byte spritePlayerNormal[8] = {
  0b00000,
  0b10000,
  0b11100,
  0b11111,
  0b11111,
  0b11100,
  0b10000,
  0b00000
};
byte spritePlayerTop[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b10000,
  0b11100,
  0b11111
};
byte spritePlayerBottom[8] = {
  0b11111,
  0b11100,
  0b10000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
byte spriteStoneNormal[8] = {
  0b01110,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b01110
};
byte spriteStoneTop[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b01110,
  0b11111,
  0b11111,
  0b11111
};
byte spriteStoneBottom[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
byte spritePlayerStoneTop[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00000,
  0b10000,
  0b11100,
  0b11111
};
byte spritePlayerStoneBottom[8] = {
  0b11111,
  0b11100,
  0b10000,
  0b00000,
  0b01110,
  0b11111,
  0b11111,
  0b11111
};
byte spritePlayerNormalBroken[8] = {
  0b10000,
  0b11100,
  0b00111,
  0b11100,
  0b11011,
  0b11100,
  0b10000,
  0b00000
};
byte spritePlayerTopBroken[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b10000,
  0b11100,
  0b00111,
  0b11100
};
byte spritePlayerBottomBroken[8] = {
  0b11011,
  0b11100,
  0b10000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
byte spriteUpArrow[8] = {
  0b00000,
  0b00100,
  0b01110,
  0b11111,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
byte spriteDownArrow[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

int stone[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows
int up = 1;
int down = 1;
int playerpos = 1;
int fps = 10;
int counter = 0;
int stonepos = 15;
int nextstone = 0;
int score = 0;
int difficulty = 5;

String screen = "start";

void setup()
{
  lcd.init(); // initialize the lcd
  lcd.backlight();
  lcd.clear();
  switchScreen(screen);

  pinMode(13, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);

  randomSeed(analogRead(A1));
}


void loop()
{
  long fstart = millis();
  lcd.clear();
  if (screen == "start") {
    startScreen();
  }
  if (screen == "ingame") {
    ingameScreen();
  }
  if (screen == "gameover") {
    gameOverScreen();
  }
  counter++;
  if (counter == 60) counter = 0;
  long fend = millis();
  if (1000/fps - (fend - fstart) > 0) delay(1000/fps - (fend-fstart));
}

void startScreen() 
{
  lcd.setCursor(6,0);
  lcd.print("ARDUINOIDS");
  lcd.setCursor(8,1);
  lcd.print("PRESS UP");
  drawPlayer(0,2);
  while (digitalRead(13) == 1) {}
  tone(11, 700, 100);
  while (digitalRead(13) == 0) {}
  for (int i = 0; i<10; i++) {
    lcd.clear();
    lcd.setCursor(6+i,0);
    lcd.print("ARDUINOIDS");
    lcd.setCursor(8+i,1);
    lcd.print("PRESS UP");
    drawPlayer(0,2);
    tone(11, 300+i*7);
    delay(1000/fps);
  }
  noTone(11);
  delay(100);
  switchScreen("ingame");
}

void ingameScreen()
{
  // Update
  if ((counter % difficulty) == 0) {
    stone[stonepos] = -1;
    if (stonepos == nextstone) {
      stone[stonepos] = random(0,5);
      nextstone = random(7,11) + stonepos;
      if (nextstone > 15) {
        nextstone = nextstone - 16;
      }
    }
    stonepos++;
    score++;
    if (stonepos == 16) {
      stonepos = 0;
    }
  }
  if (score > 500) {
    difficulty = 1;
  } else if (score > 200) {
    difficulty = 2;
  } else if (score > 100) {
    difficulty = 3;
  } else if (score > 20) {
    difficulty = 4;
  }

  bool collision = false;
  if (playerpos - 1 == stone[stonepos] && playerpos - 1 != -1) collision = true;
  if (playerpos  == stone[stonepos]) collision = true;
  if (playerpos + 1 == stone[stonepos]) collision = true;
  if (collision) switchScreen("gameover");

  // Input
  if (digitalRead(13) == 0 && digitalRead(13) != up) {
    playerpos--;
    up = 0;
    tone(11, 700, 100);
  }
  if (digitalRead(13) == 1 && digitalRead(13) != up) {
    up = 1;
  }
  if (digitalRead(12) == 0 && digitalRead(12) != down) {
    playerpos++;
    down = 0;
    tone(11, 500, 100);
  }
  if (digitalRead(12) == 1 && digitalRead(12) != down) {
    down = 1;
  }
  if (playerpos < 0) playerpos = 0;
  if (playerpos > 4) playerpos = 4;

  // Draw
  for (int i = 0; i<16; i++) {
    if (i - stonepos < 0) {
      drawStone(16 + i - stonepos, stone[i]);
    } else {
      drawStone(i - stonepos, stone[i]);
    }
  }
  drawPlayer(0,playerpos);
  lcd.setCursor(16 - String(score).length(),0);
  lcd.print(score);
}

void gameOverScreen() 
{
  lcd.setCursor(7,0);
  lcd.print("GAME OVER");
  tone(11, 700, 500);
  delay(500);
  tone(11, 500, 500);
  delay(500);
  tone(11, 300, 500);
  delay(500);
  lcd.setCursor(6,1);
  lcd.print("SCORE:");
  lcd.setCursor(16 - String(score).length(), 1);
  lcd.print(score);
  drawPlayer(0,playerpos);
  while (digitalRead(13) == 1) {}
  while (digitalRead(13) == 0) {}
  delay(100);
  switchScreen("start");
}

void switchScreen(String name) 
{
  if (name == "start") {
    screen = "start";
    lcd.createChar(1, spritePlayerTop);
    lcd.createChar(2, spritePlayerBottom);
  }
  if (name == "ingame") {
    screen = "ingame";
    lcd.createChar(0, spritePlayerNormal);
    lcd.createChar(1, spritePlayerTop);
    lcd.createChar(2, spritePlayerBottom);
    lcd.createChar(3, spriteStoneNormal);
    lcd.createChar(4, spriteStoneTop);
    lcd.createChar(5, spriteStoneBottom);
    lcd.createChar(6, spritePlayerStoneTop);
    lcd.createChar(7, spritePlayerStoneBottom);

    playerpos = 2;
    stonepos = 15;
    nextstone = 0;
    score = 0;
    difficulty = 5;

    for (int i = 0; i < 16; i++) {
      stone[i] = -1;
    }
  }
  if (name == "gameover") {
    screen = "gameover";
    lcd.createChar(0, spritePlayerNormalBroken);
    lcd.createChar(1, spritePlayerTopBroken);
    lcd.createChar(2, spritePlayerBottomBroken);
  }
}

void drawStone(int x, int y) 
{
  if (y == 0) {
    lcd.setCursor(x, 0);
    lcd.write((byte)5);
  }
  if (y == 1) {
    lcd.setCursor(x, 0);
    lcd.write((byte)3);
  }
  if (y == 2) {
    lcd.setCursor(x, 0);
    lcd.write((byte)4);
    lcd.setCursor(x, 1);
    lcd.write((byte)5);
  }
  if (y == 3) {
    lcd.setCursor(x, 1);
    lcd.write((byte)3);
  }
  if (y == 4) {
    lcd.setCursor(x, 1);
    lcd.write((byte)4);
  }
}

void drawPlayer(int x, int y) 
{
  if (y == 0) {
    if (y + 2 == stone[stonepos]) {
      lcd.setCursor(x, 0);
      lcd.write((byte)7);
    } else {
      lcd.setCursor(x, 0);
      lcd.write((byte)2);
    }
  }
  if (y == 1) {
    lcd.setCursor(x, 0);
    lcd.write((byte)0);
  }
  if (y == 2) {
    if (y - 2 == stone[stonepos]) {
      lcd.setCursor(x, 0);
      lcd.write((byte)6);
    } else {
      lcd.setCursor(x, 0);
      lcd.write((byte)1);
    }
    if (y + 2 == stone[stonepos]) {
      lcd.setCursor(x, 1);
      lcd.write((byte)7);
    } else {
      lcd.setCursor(x, 1);
      lcd.write((byte)2);
    }
  }
  if (y == 3) {
    lcd.setCursor(x, 1);
    lcd.write((byte)0);
  }
  if (y == 4) {
    if (y - 2 == stone[stonepos]) {
      lcd.setCursor(x, 1);
      lcd.write((byte)6);
    } else {
      lcd.setCursor(x, 1);
      lcd.write((byte)1);
    }
  }
}
