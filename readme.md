# HD44780 AVR

## Notes
* Library compatible with *5* and *3.3* voltage.
* Adapted delays on the risk of the display frequency dropping.

## Tips
* Remember to define *F_CPU=* for each file!
* Remember to configure pins in *lcd.h* file!
* Remember to upload data to the EEPROM memory after flashing, for the relevant functions.

## Overview
```c
void lcd_init(void);
void lcd_clear(void);
void lcd_home(void);
void lcd_setCursor(uint8_t x, uint8_t y);
void lcd_displayOnOff(uint8_t onoff);
void lcd_cursorOnOff(uint8_t onoff);
void lcd_cursorBlinkOnOff(uint8_t onoff);
void lcd_codmtlor(uint8_t cod, uint8_t lor);
void lcd_emlordoi(uint8_t lor, uint8_t doi);
void lcd_char(char c);
void lcd_str(char *str);
void lcd_str_P(const char *str);
void lcd_str_E(char *str);
void lcd_int16(int16_t value);
void lcd_uint16(uint16_t value);
void lcd_int32(int32_t value);
void lcd_uint32(uint32_t value);
void lcd_hex(uint32_t value);
void lcd_createChar(uint8_t code, uint8_t *pattern);
void lcd_createChar_P(uint8_t code, const uint8_t *pattern);
void lcd_createChar_E(uint8_t code, uint8_t *pattern);
```
## License
[MIT License](license.txt)

Copyright (c) 2018, Michal Kozakiewicz, github.com/michal037
