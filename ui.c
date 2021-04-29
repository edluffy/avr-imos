#include <stdio.h>
#include "ui.h"

/* ----------- uiSlider ---------- */

void uiSliderMove(uiSlider * const me, uint16_t pos)
{
	if((me->pos=pos) != me->oldpos)
		uiSliderDraw(me);
	me->oldpos = me->pos;
}

void uiSliderStatus(uiSlider * const me, uint16_t color)
{
	if((me->color=color) != me->oldcolor)
		uiSliderDraw(me);
	me->oldcolor = me->color;
}

void uiSliderDraw(uiSlider * const me)
{
	rectangle mask = {me->x, me->x+100, me->y-2, me->y+2};
	rectangle thinBar = {me->x, me->x+100, me->y, me->y};
	rectangle thickBar = {me->x+me->pos, me->x+me->pos+28, me->y-2, me->y+2};
	rectangle leftSideBar = {me->x, me->x, me->y-5, me->y+5};
	rectangle rightSideBar = {me->x+100, me->x+100, me->y-5, me->y+5};
	
	fill_rectangle(mask, BLACK);
	fill_rectangle(thinBar, me->color);
	fill_rectangle(thickBar, me->color);
	fill_rectangle(leftSideBar, me->color);
	fill_rectangle(rightSideBar, me->color);
}

/* ----------- uiText -------*/

void uiTextRefresh(uiText * const me, double newval)
{
    uiTextDraw(me);
}

void uiTextDraw(uiText * const me)
{
	char strbuf[140];
	sprintf(strbuf, "%s%i", me->text, 140);

	display.x = me->x, display.y = me->y;
	display_string(me->text);
}

/* ----------- uiTextBox -------*/

void uiTextBoxSelect(uiTextBox * const me, bool nhover)
{
	if((me->hover=nhover) != me->oldhover){
		uiTextBoxHide(me);
		uiTextBoxDraw(me);
	}
	me->oldhover = me->hover;
}

void uiTextBoxDraw(uiTextBox * const me)
{
	uint8_t textw = getTextw(me->text);

	if(me->hover){
		rectangle frame = {me->x-3, me->x+textw+10, me->y-3, me->y+8};
		fill_rectangle(frame, CYAN);
	}

	rectangle background = {me->x-2, me->x+textw+9, me->y-2, me->y+7};
	fill_rectangle(background, BLACK);

	rectangle rtip = {me->x, me->x+5, me->y, me->y+5};
	fill_rectangle(rtip, me->color);

	display.x = me->x+10;
	display.y = me->y;
	display_string(me->text);
}

void uiTextBoxHide(uiTextBox * const me)
{
	uint8_t textw = getTextw(me->text);

	rectangle mask = {me->x-3, me->x+textw+10, me->y-3, me->y+8};
	fill_rectangle(mask, BLACK);
}

/* ----------- uiCursor -------*/

void uiCursorMove(uiCursor * const me, uint16_t newpos)
{
	if(newpos != me->pos){
		uiCursorHide(me);
		me->pos = me->oldpos = newpos;
		uiCursorDraw(me);
	}
}

void uiCursorDraw(uiCursor * const me)
{
	rectangle line;
	char ch = (me->old) ? ' ' : me->arrow;
	uint16_t col;
	if(me->old) col = BLACK;
	else if(me->hover) col = GREEN;
	else col = me->color;

	switch(me->arrow){
		case '^':
			line.left = me->pos, line.right = me->pos;
			line.top = SPAD, line.bottom = SCBOTTOM-SPAD;
			display.x = me->pos-2, display.y = SCBOTTOM-SPAD+1;
			break;
		case '<':
			line.left=SPAD, line.right = SCRIGHT-SPAD;
			line.top = me->pos, line.bottom = me->pos;
			display.x = SCRIGHT-SPAD, display.y = me->pos-3;
			break;
		case '>':
		default:
			line.left=SPAD, line.right = SCRIGHT-SPAD;
			line.top = me->pos, line.bottom = me->pos;
			display.x = SPAD-6, display.y = me->pos-3;
			break;
	}

	if(me->trace) fill_rectangle(line, col);
	display_char(ch);
}

void uiCursorHide(uiCursor * const me)
{
	me->old = true;
	uiCursorDraw(me);
	me->old = false;
}

/* ----------- uiGrid ------- */
void uiGridDraw(uiGrid * const me)
{
	int pos, n;
	rectangle xGrid[NXGRID];
	rectangle yGrid[NYGRID];

	/* Form Grid */ 
	for(pos=SCTOP+SPAD, n=0; pos <= SCBOTTOM-SPAD; pos+=me->major, n++){
		xGrid[n].left = SCLEFT+SPAD;
		xGrid[n].right = SCRIGHT-SPAD;

		xGrid[n].top = xGrid[n].bottom = pos;
		fill_rectangle(xGrid[n], DARKGREY);
	}

	for(pos=SCLEFT+SPAD, n=0; pos <= SCRIGHT-SPAD; pos+=me->major, n++){
		yGrid[n].top = SCTOP+SPAD;
		yGrid[n].bottom = SCBOTTOM-SPAD;

		yGrid[n].left = yGrid[n].right = pos;
		fill_rectangle(yGrid[n], DARKGREY);
	}

	fill_rectangle(yGrid[0], LIGHTGREY);
	fill_rectangle(yGrid[NYGRID-1], LIGHTGREY);

	fill_rectangle(xGrid[0], LIGHTGREY);
	fill_rectangle(xGrid[NXGRID-1], LIGHTGREY);
}

void uiGridHide(uiGrid * const me)
{
	rectangle bg = {SCLEFT+SPAD, SCRIGHT-SPAD, SCTOP+SPAD, SCBOTTOM-SPAD};
	fill_rectangle(bg, BLACK);
}

/* ----------- uiPlot -------*/
void uiPlotUpdate(uiPlot * const me, double nscale, uint16_t nstartp, uint8_t noffset)
{
	me->scale = nscale, me->startp = nstartp, me->offset = noffset;
	uiPlotDraw(me);

}

void uiPlotDraw(uiPlot * const me)
{
	int n, pos, low, high, ccount;

	ccount = 0;
	for(n=1, pos=me->startp; n < SWIDTH; n++, pos++){

		if(me->offset-me->data[pos]*me->scale < 20 ||
				me->offset-me->data[pos]*me->scale > 220){
			ccount++; continue;
		}
		if(me->offset-me->data[pos-1]*me->scale < 20 ||
				me->offset-me->data[pos-1]*me->scale > 220){
			ccount++; continue;
		}

		if(me->data[pos-1] < me->data[pos])
			low = pos-1, high = pos;
		else
			low = pos, high = pos-1;

		fill_rectangle(me->dp[n], BLACK);

		me->dp[n].top = me->offset - me->data[high]*me->scale;
		me->dp[n].bottom = me->offset - me->data[low]*me->scale;

		me->dp[n].left = me->dp[n].right = n+SCLEFT+SPAD;
		fill_rectangle(me->dp[n], YELLOW);
	}

	me->clipping = (ccount > 10) ? true : false;
}

/* ----------- Utility ---------- */

uint8_t getTextw(const char *text)
{
	uint8_t cn = 0;
	while(text[cn++] != '\0');
	return cn*CHAR_W; 
}

void debugPrint(volatile int n)
{
	char msg[10];
	itoa(n, msg, 10);

	display.x = 160, display.y = 202;
	display_string("     ");

	display.x = 160, display.y = 202;
	display_string(msg);
}

// where (x, y) are coords of top left corner, shadow = 0 (delete) or 1 (keep)
void uiMoveRect(rectangle *r, int x, int y, int color, int shadow)
{
	int width = r->right - r->left;
	int height = r->bottom - r->top; 

	if(!shadow)
		fill_rectangle(*r, BLACK);

	r->left = x;
	r->right = x+width;

	r->top = y;
	r->bottom = y+height;

	fill_rectangle(*r, color);
}
