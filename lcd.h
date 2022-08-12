#ifndef LCD_H_
#define LCD_H_

void functionSet(void);
void entryModeSet(char, char);
void displayOnOffControl(char, char, char);
char getBit(char, char);
void sendChar(char);
void setDdRamAddress(char);
void sendString(char*);

#endif // LCD_H_