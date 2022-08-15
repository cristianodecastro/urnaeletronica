#ifndef LCD_H_
#define LCD_H_

void functionSet(void);
void entryModeSet(char, char);
void displayOnOffControl(char, char, char);
char getBit(char, char);
void sendChar(char);
void setDdRamAddress(char);
void cursorHome();
void sendString_setAdress(char*, char, char);
void sendString(char*);
void clearDisplay();

#endif // LCD_H_