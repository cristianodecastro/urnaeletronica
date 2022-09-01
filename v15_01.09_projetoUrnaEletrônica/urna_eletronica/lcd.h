#ifndef LCD_H_
#define LCD_H_

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
void print_erroComunicacao();
void print_optionsVotacao();
void consulta_hora(char*, char*);
void troca_hora(char*, char*);
void ver_estado(char*, char*, char*);

#endif // LCD_H_