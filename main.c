#include "util/delay.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "ui.h"
#include "io.h"


/* TODO: 
* graphical imos start screen
* add SLIDER, pause when clicked
* add CURSOR optionBox (with two horizontal cursors)
* add OPTION optionBox
*
* draw extra info on graph when in certain options
* draw red grid outline when overlap
*/

enum menuState {CURSOR, MINIMAP, MEASURE, SCALE, TIMEBASE, OFFSET, TRIGGER };

int main(void)
{
	init_control();
	init_lcd();
	init_adc();
	sei();

	set_orientation(West);

	rectangle dataP[SWIDTH];
	for(int i = 0; i < SWIDTH; i++)
		dataP[i].left=dataP[i].right=dataP[i].top=dataP[i].bottom=0;


	uiGrid grid = {.major=20, .minor=5};
	uiPlot plot = {.dp=dataP, .data=adcbuf};

	uiSlider minimap = {.x=110, .y=12, .pos=10, .color=GREEN};

	bool clearMeasureFlag = false;
	uiText dVDisplay = {.x=SPAD+5, .y=23, .text="dV: ", .unit="V"};
	uiText dTDisplay = {.x=SPAD+5, .y=35, .text="dT: ", .unit="ms"};

	uiText vmaxDisplay = {.x=SPAD+5, .y=161, .text="Vmax: ", .unit="V"};
	uiText vminDisplay = {.x=SPAD+5, .y=173, .text="Vmin: ", .unit="V"};
	uiText vppDisplay = {.x=SPAD+5, .y=185, .text="Vpp : ", .unit="V"};
	uiText vavrDisplay = {.x=SPAD+5, .y=197, .text="Vavr: ", .unit="V"};
	uiText vrmsDisplay = {.x=SPAD+5, .y=209, .text="Vrms: ", .unit="V"};

	uiCursor offsetCursor  = {.arrow='<', .color=BLACK, .trace=false};
	uiCursor triggerCursor = {.arrow='>', .color=BLACK, .trace=false};
	uiCursor measureCursor[4] = {
		{.arrow='^', .color=ORANGE,  .trace=true},
	 	{.arrow='^', .color=ORANGE,  .trace=true},
	 	{.arrow='>', .color=RED,  .trace=true},
	 	{.arrow='>', .color=RED,  .trace=true}
	};

	uiTextBox optionBox[7] = {
		{.x=20,  .y=9         , .text="CURSOR",   .color=CYAN},
		{.x=0, .y=0           , .text="",         .color=BLACK}, /* Placeholder */
		{.x=250, .y=9         , .text="MEASURE",  .color=GREEN},
		{.x=20,  .y=SCBOTTOM-9, .text="SCALE",    .color=BLUE},
		{.x=90,  .y=SCBOTTOM-9, .text="TIMEBASE", .color=YELLOW},
		{.x=180, .y=SCBOTTOM-9, .text="OFFSET",   .color=MAGENTA},
		{.x=260, .y=SCBOTTOM-9, .text="TRIGGER",  .color=RED}
	};
		
	for(int i=0; i < 7; i++)
		uiTextBoxDraw(&optionBox[i]);

	for/*ever*/(;;)
	{
		//ioCheckTrigger();

		/* ------- Grid and Plotting -------- */
		if(!ioPaused && cnt%2==0) start_adc();
		uiGridDraw(&grid);
		uiPlotUpdate(&plot, scale/100.0, scrollpos*10, offset);

		/* ------- Minimap -------- */
		uiSliderMove(&minimap, scrollpos);
		if(pbstate == MINIMAP)
			uiSliderStatus(&minimap, CYAN);
		else if(plot.clipping)
			uiSliderStatus(&minimap, RED);
		else if(ioPaused)
			uiSliderStatus(&minimap, LIGHTGREY);
/*		else if(ioTrigState == 0) // Rising and above
			uiSliderStatus(&minimap, GREEN);
		else if(ioTrigState == 1) // Timeout
			uiSliderStatus(&minimap, ORANGE); */
		else // Not triggering
			uiSliderStatus(&minimap, GREEN);

		/* ------- Option Boxes -------- */
		if(pbstate != MINIMAP)
			uiTextBoxSelect(&optionBox[pbstate], true);
		uiTextBoxSelect(&optionBox[lastpb], false);

		/* ------- Measurement Menu -------- */
		if(ioShowMeasure){
			uiTextRefresh(&dVDisplay, ioGetdV());
			uiTextRefresh(&dTDisplay, ioGetdT());

			uiTextRefresh(&vmaxDisplay, ioGetMax());
			uiTextRefresh(&vminDisplay, ioGetMin());
			uiTextRefresh(&vppDisplay, ioGetMax()-ioGetMin());
			uiTextRefresh(&vavrDisplay, ioGetAvr());
			uiTextRefresh(&vrmsDisplay, 0.707*ioGetMax());
			clearMeasureFlag = true;
		}

		if(!ioShowMeasure && clearMeasureFlag)
			clearMeasureFlag = false, uiGridHide(&grid);

		/* ------- User Input -------- */
		switch(pbstate){
                case CURSOR:
                  rp = &mcpos[ioCursor];
                  rspec = 255;
                  rstep = 1, rmax = 300, rmin = 20;

                  for (int i = 0; i < 4; i++) {
                    measureCursor[i].hover = (i == ioCursor) ? true : false;
                    uiCursorMove(&measureCursor[i], mcpos[i]);
                    uiCursorDraw(&measureCursor[i]);
                  }
                  break;
                case MINIMAP:
                  rp = &scrollpos, rspec = 254;
                  rstep = 1, rmax = 72, rmin = 1;

                  for (int i = 0; i < 4; i++)
                    uiCursorHide(&measureCursor[i]);
                  break;

                case MEASURE:
                  rp = 0, rspec = 253;
                  rstep = rmax = rmin = 0;
                  break;

                case SCALE:
                  rp = &scale, rspec = 50;
                  rstep = 5, rmax = 100, rmin = 0;

                  break;

                case TIMEBASE:
                  rp = &timebase, rspec = 0;
                  rstep = 2, rmax = 100, rmin = 0;
                  break;

                case OFFSET:
                  rp = &offset, rspec = 120;
                  rstep = 5, rmax = 220, rmin = 20;

                  uiCursorMove(&offsetCursor, offset);
                  break;

                case TRIGGER:
                  rp = &trigger, rspec = 120;
                  rstep = 5, rmax = 220, rmin = 20;

                  uiCursorMove(&triggerCursor, trigger);
                  break;

                case 7: // NOTHING
                default:
                  break;

		}
	}

}
