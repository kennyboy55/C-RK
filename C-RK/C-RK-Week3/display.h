
#ifndef _Display_H
#define _Display_H

extern void LcdSetPosition(int);
extern void LcdLine(u_char line, char text[]);
extern void LcdScrollLine(u_char line, int pos, char text[]);
extern void LcdClear(void);

#endif 
