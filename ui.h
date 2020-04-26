#ifndef _UIH_
#define _UIH_

#include "avr/io.h"
#include "util/delay.h"
#include <stdlib.h>
#include <stdbool.h>

#include "lcd.h"

#define SCTOP 0
#define SCLEFT 0
#define SCBOTTOM 240
#define SCRIGHT 320

#define SPAD 20
#define SWIDTH 280

#define CHAR_W 6

// Extra colors
#define DARKGREY    0x39E7
#define LIGHTGREY    0x7BEF
#define ORANGE  0xFE00

#define NXGRID 11
#define NYGRID 15

// Utils
uint8_t getTextw(const char *text);
void debugPrint(int n);

/* ----------- uiSlider ---------- */
typedef struct{
	uint16_t x, y;

	uint16_t pos, oldpos;
	uint16_t color, oldcolor;
} uiSlider;

void uiSliderMove(uiSlider * const me, uint16_t pos);
void uiSliderStatus(uiSlider * const me, uint16_t color);
void uiSliderDraw(uiSlider * const me);

/* ----------- uiText -------*/
typedef struct{
	uint16_t x, y;
	char text[7];
	char unit[3];

	double value;
} uiText;

void uiTextDraw(uiText * const me);
void uiTextRefresh(uiText * const me, double newval);

/* ----------- uiTextBox -------*/
typedef struct{
	uint16_t x, y;

	char text[15];
	uint16_t color;
	bool hover, oldhover;
} uiTextBox;

void uiTextBoxSelect(uiTextBox * const me, bool hover);
void uiTextBoxDraw(uiTextBox * const me);
void uiTextBoxHide(uiTextBox * const me);

/* ----------- uiCursor -------*/
typedef struct{
	char arrow;
	uint16_t color;
	bool trace;

	bool old, hover;
	uint16_t pos, oldpos;
} uiCursor;

void uiCursorMove(uiCursor * const me, uint16_t newpos);
void uiCursorDraw(uiCursor * const me);
void uiCursorHide(uiCursor * const me);

/* ----------- uiGrid -------*/
typedef struct{
	uint8_t major, minor;
} uiGrid;

void uiGridDraw(uiGrid * const me);
void uiGridHide(uiGrid * const me);

/* ----------- uiPlot -------*/
typedef struct{
	double scale;
	uint16_t startp;
	uint8_t offset;

	bool clipping;
	rectangle *dp;
	volatile uint8_t *data;
} uiPlot;

void uiPlotUpdate(uiPlot * const me, double scale, uint16_t startp, uint8_t offset);
void uiPlotDraw(uiPlot * const me);

/* ----------- Utility ---------- */

uint8_t getTextw(const char *text);
void debugPrint(volatile int n);
void uiMoveRect(rectangle *r, int x, int y, int color, int shadow);






#endif
