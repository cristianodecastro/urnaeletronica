#ifndef LCD_H_
#define LCD_H_

#define DOWN_ARROW_CHARACTER 0x00
#define RIGHT_OPTION_CHARACTER 0x01
#define RETURN_CHARACTER 0x02
#define LEFT_ARROW_CHARACTER 0x7F

void functionSet(void);
void entryModeSet(char, char);
void displayOnOffControl(char, char, char);
char getBit(char, char);
void sendChar(char);
void setDdRamAddress(char);
void setCgRamAddress(char);
void cursorHome();
void sendString_setAdress(char*, char, char);
void sendString(char*);
void clearDisplay();
void storage_special_characters();

#endif // LCD_H_