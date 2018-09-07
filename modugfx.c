/*
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

//#include STM32_HAL_H

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/objarray.h"
#include "py/objstr.h"

#if MICROPY_HW_HAS_UGFX

//#include "pin.h"
#include "modugfx.h"
#include "gfx.h"
#include "tilda_thread.h"

#include "modugfx/board_ILI9341.h"

//#include "genhdr/pins.h"
//#include "bufhelper.h"

/// \moduleref pyb
///
/// The ugfx module is used to control a wide range of LCDs, as defined in
/// the config headers.
///
/// The connections must also be configured in the configuration
/// header files
///
///     import ugfx
///     ugfx.init()
///
/// Then you can use:
///
///


const char * font_list[] = {"DejaVuSans12","ralewaybold24","ralewayextrabold48","DejaVuSans16","ralewayextrabold16","fixed_7x14","fixed_10x20"};

#include <time.h>

int nanosleep(const struct timespec *rqtp, struct timespec *rmtp)
{
    return clock_nanosleep(CLOCK_REALTIME, 0, rqtp, rmtp);
}

typedef struct _ugfx_obj_t {
    mp_obj_base_t base;
} ugfx_obj_t;

static orientation_t get_orientation(int a){
	if (a == 90)
		return GDISP_ROTATE_90;
	else if (a == 180)
		return GDISP_ROTATE_180;
	else if (a == 270)
		return GDISP_ROTATE_270;
	else
		return GDISP_ROTATE_0;
}

/// \method init()
///
STATIC mp_obj_t ugfx_init(void) {

	gwinSetDefaultFont(gdispOpenFont(font_list[0]));
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);

	gfxInit();

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ugfx_init_obj, ugfx_init);

/// \method spi_clk(a)
///
/// Set the lcd clock frequency (in Hz)
///
STATIC mp_obj_t ugfx_spi_clk(mp_obj_t clk_obj) {

    uint32_t a = mp_obj_get_int(clk_obj);
    gdispSetSPIClock(a);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_spi_clk_obj, ugfx_spi_clk);

/// \method deinit()
///
STATIC mp_obj_t ugfx_deinit(void) {
    gfxDeinit();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ugfx_deinit_obj, ugfx_deinit);

/// \method ball_demo()
///
/// BALL DEMO!!!
///
STATIC mp_obj_t ugfx_ball_demo(void) {
    // extract arguments
    //ugfx_obj_t *self = args[0];

	/*
	 * Copyright (c) 2012, 2013, Joel Bodenmann aka Tectu <joel@unormal.org>
	 * Copyright (c) 2012, 2013, Andrew Hannam aka inmarket
	 * Derived from the 2011 IOCCC submission by peter.eastman@gmail.com
	 *
	 */

#define BALLCOLOR1		Red
#define BALLCOLOR2		Yellow
#define WALLCOLOR		HTML2COLOR(0x303030)
#define BACKCOLOR		HTML2COLOR(0xC0C0C0)
#define FLOORCOLOR		HTML2COLOR(0x606060)
#define SHADOWALPHA		(255-255*0.2)

	coord_t		width, height, x, y, radius, ballx, bally, dx, floor;
	coord_t		minx, miny, maxx, maxy;
	coord_t		ballcx, ballcy;
	color_t		colour;
	float		ii, spin, dy, spinspeed, h, f, g;

	width = gdispGetWidth();
	height = gdispGetHeight();

	radius=height/5+height%2+1;	// The ball radius
	ii = 1.0/radius;			// radius as easy math
	floor=height/5-1;			// floor position
	spin=0.0;					// current spin angle on the ball
	spinspeed=0.1;				// current spin speed of the ball
	ballx=width/2;				// ball x position (relative to the ball center)
	bally=height/4;				// ball y position (relative to the ball center)
	dx=.01*width;				// motion in the x axis
	dy=0.0;						// motion in the y axis
	ballcx = 12*radius/5;		// ball x diameter including the shadow
	ballcy = 21*radius/10;		// ball y diameter including the shadow


	minx = miny = 0; maxx = width; maxy = height;		// The clipping window for this frame.
	while(1) {
		// Draw one frame
		gdispStreamStart(minx, miny, maxx-minx, maxy-miny);
		for (y=miny; h = (bally-y)*ii, y<maxy; y++) {
			for (x=minx; x < maxx; x++) {
				g=(ballx-x)*ii;
				f=-.3*g+.954*h;
				if (g*g < 1-h*h) {
					/* The inside of the ball */
					if ((((int)(9-spin+(.954*g+.3*h)*invsqrt(1-f*f))+(int)(2+f*2))&1))
						colour = BALLCOLOR1;
					else
						colour = BALLCOLOR2;
				} else {
					// The background (walls and floor)
					if (y > height-floor) {
						if (x < height-y || height-y > width-x)
							colour = WALLCOLOR;
						else
							colour = FLOORCOLOR;
					} else if (x<floor || x>width-floor)
						colour = WALLCOLOR;
					else
						colour = BACKCOLOR;

					// The ball shadow is darker
					if (g*(g+.4)+h*(h+.1) < 1)
						colour = gdispBlendColor(colour, Black, SHADOWALPHA);
				}
				gdispStreamColor(colour);	/* pixel to the LCD */
			}
		}
		gdispStreamStop();

		// Force a display update if the controller supports it
		gdispFlush();

		// Calculate the new frame size (note this is a drawing optimisation only)
		minx = ballx - radius; miny = bally - radius;
		maxx = minx + ballcx; maxy = miny + ballcy;
		if (dx > 0) maxx += dx; else minx += dx;
		if (dy > 0) maxy += dy; else miny += dy;
		if (minx < 0) minx = 0;
		if (maxx > width) maxx = width;
		if (miny < 0) miny = 0;
		if (maxy > height) maxy = height;

		// Motion
		spin += spinspeed;
		ballx += dx; bally += dy;
		dx = ballx < radius || ballx > width-radius ? spinspeed=-spinspeed,-dx : dx;
		dy = bally > height-1.75*floor ? -.04*height : dy+.002*height;
	}



    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ugfx_ball_demo_obj, ugfx_ball_demo);


/// \method set_orientation(a)
///
/// Set orientation to 0, 90, 180 or 270 degrees
///
STATIC mp_obj_t ugfx_set_orientation(mp_uint_t n_args, const mp_obj_t *args) {
	if (n_args > 0){
		int a = mp_obj_get_int(args[0]);
		gdispSetOrientation(get_orientation(a));

	}

    return mp_obj_new_int(gdispGetOrientation());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_set_orientation_obj, 0, 1, ugfx_set_orientation);

/// \method power_mode(mode)
///
/// mode can be any of those options: ugfx.POWER_ON, ugfx.POWER_OFF, ugfx.POWER_DEEP_SLEEP, ugfx.POWER_SLEEP
///
STATIC mp_obj_t ugfx_power_mode(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args > 0){
        gdispSetPowerMode(mp_obj_get_int(args[0]));
        return mp_const_none;
    } else {
        return mp_obj_new_int(gdispGetPowerMode());
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_power_mode_obj, 0, 1, ugfx_power_mode);

/// \method width()
///
/// Gets current width of the screen in pixels
///
STATIC mp_obj_t ugfx_width(void) {
    return mp_obj_new_int(gdispGetWidth());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ugfx_width_obj, ugfx_width);



/// \method height()
///
/// Gets current width of the screen in pixels
///
STATIC mp_obj_t ugfx_height(void) {
    return mp_obj_new_int(gdispGetHeight());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ugfx_height_obj, ugfx_height);


/// \method poll()
///
/// calls gfxYield, which will handle widget redrawing when for inputs.
/// Register as follows:
/// tim = pyb.Timer(3)
/// tim.init(freq=60)
/// tim.callback(lambda t:ugfx.poll())
///
STATIC mp_obj_t ugfx_poll(void) {
	gfxYield();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ugfx_poll_obj, ugfx_poll);


/// \method backlight(100)
///
/// Returns or sets backlight (in %)
STATIC mp_obj_t ugfx_backlight(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        gdispSetBacklight(mp_obj_get_int(args[0]));
        return mp_const_none;
    } else {
        return mp_obj_new_int(gdispGetBacklight());
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_backlight_obj, 0, 1, ugfx_backlight);



/// \method get_pixel()
///
/// Gets the colour of the given pixel at (x,y)
///
///
// LWK TODO:- remove the pragma once routine is fixed
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
STATIC mp_obj_t ugfx_get_pixel(mp_obj_t x_in, mp_obj_t y_in) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
	int x = mp_obj_get_int(x_in);
	int y = mp_obj_get_int(y_in);
	return mp_obj_new_int(0);
	//needs sorting, currently returns somewhat dodgy values
    //return mp_obj_new_int(gdispGetPixelColor(x,y));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ugfx_get_pixel_obj, ugfx_get_pixel);
#pragma GCC diagnostic pop


/// \method set_default_font()
///
/// Sets the default font used by widgets.
/// Note, it is only necessary to use a font object if font scaling is used, since
///  in this case memory will need to be cleared once the scaled font is no longer required
///
STATIC mp_obj_t ugfx_set_default_font(mp_obj_t font_obj) {
	ugfx_font_obj_t *fo = font_obj;
	if (MP_OBJ_IS_TYPE(font_obj, &ugfx_font_type)){
		gwinSetDefaultFont(fo->font);
	}else if (MP_OBJ_IS_STR(font_obj)){
		const char *file = mp_obj_str_get_str(font_obj);
		gwinSetDefaultFont(gdispOpenFont(file));
	}else if (MP_OBJ_IS_INT(font_obj)){
		if (mp_obj_get_int(font_obj) < sizeof(font_list)/sizeof(char*)){
			gwinSetDefaultFont(gdispOpenFont(font_list[mp_obj_get_int(font_obj)]));
		}
		else
			nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Invalid font index"));
	}

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_set_default_font_obj, ugfx_set_default_font);


/// \method set_default_style()
///
/// Sets the default style used by widgets.
///
STATIC mp_obj_t ugfx_set_default_style(mp_obj_t style_obj) {
	ugfx_style_obj_t *st = style_obj;
	if (MP_OBJ_IS_TYPE(style_obj, &ugfx_style_type))
		gwinSetDefaultStyle(&(st->style),FALSE);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_set_default_style_obj, ugfx_set_default_style);




/// \method html_color(rgb)
///
/// Converts a 0xRRGGBB into ugfx colour format
///
STATIC mp_obj_t ugfx_html_color(mp_obj_t rgb) {
	int rgb_ = mp_obj_get_int(rgb);
    return mp_obj_new_int(HTML2COLOR(rgb_));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_html_color_obj, ugfx_html_color);
//STATIC MP_DEFINE_CONST_STATICMETHOD_OBJ(ugfx_html_color_obj, (mp_obj_t)&ugfx_html_color_fun_obj);


/// \method print_fonts()
///
/// Prints the list of installed fonts
///
STATIC mp_obj_t ugfx_print_fonts(void) {
	//TODO: THIS
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ugfx_print_fonts_obj, ugfx_print_fonts);


/// \method send_tab()
///
/// Sends a 'tab' signal to cycle through focus.
///
STATIC mp_obj_t ugfx_send_tab(void) {

	GSourceListener	*psl=0;
	GEventKeyboard	*pe;

	while ((psl = geventGetSourceListener(ginputGetKeyboard(GKEYBOARD_ALL_INSTANCES), psl))){
		pe = (GEventKeyboard *)geventGetEventBuffer(psl);


		pe->type = GEVENT_KEYBOARD;
		pe->bytecount = 1;
		pe->c[0] = GKEY_TAB;
		geventSendEvent(psl);
	}

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ugfx_send_tab_obj, ugfx_send_tab);


/// \method write_command(register, data)
///
/// Sends data to a given register on the screen
/// Example: ugfx.write_command(0x35,0) turns on TEAR output
///
STATIC mp_obj_t ugfx_write_command(mp_uint_t n_args, const mp_obj_t *args) {

	uint16_t regw = mp_obj_get_int(args[0]);
	uint len;
	mp_obj_t * data_array;

	write_index(0, regw);

	if (n_args > 1){
		if (MP_OBJ_IS_INT(args[1]))
			write_data(0, (uint8_t)mp_obj_get_int(args[1]));
		else
		{
			mp_obj_get_array(args[1], &len, &data_array);
			for (int i = 0; i < len; i++){
				write_data(0, (uint8_t)mp_obj_get_int(data_array[i]));
			}
		}
	}

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_write_command_obj, 1, 2, ugfx_write_command);


/// \method enable_tear()
///
/// Enables tear output, connected to pin "TEAR" on the board
///
STATIC mp_obj_t ugfx_enable_tear(void) {

	write_index(0, 0x35);
	write_data(0, 0);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ugfx_enable_tear_obj, ugfx_enable_tear);


/// \method disable_tear()
///
/// Disables tear output
///
STATIC mp_obj_t ugfx_disable_tear(void) {

	write_index(0, 0x34);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ugfx_disable_tear_obj, ugfx_disable_tear);

/// \method set_tear_line()
///
/// Sets the line (0->319) which causes tear output to go high
///
STATIC mp_obj_t ugfx_set_tear_line(mp_obj_t line_in) {

	int l = mp_obj_get_int(line_in);

	write_index(0, 0x44);
	write_data(0, (l&0x100) >> 16);
	write_data(0, l&0xFF);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_set_tear_line_obj, ugfx_set_tear_line);





/// \method stream_start(x1, y1, w, h)
///
/// Starts a display stream
///
STATIC mp_obj_t ugfx_stream_start(mp_uint_t n_args, const mp_obj_t *args) {
    int x = mp_obj_get_int(args[0]);
    int y = mp_obj_get_int(args[1]);
    int w = mp_obj_get_int(args[2]);
    int h = mp_obj_get_int(args[3]);

    gdispStreamStart(x, y, w, h);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_stream_start_obj, 4, 4, ugfx_stream_start);

/// \method stream_color(color)
///
/// Sets a pixel. Needs to be called after stream_start
///
STATIC mp_obj_t ugfx_stream_color(mp_obj_t color) {
    gdispStreamColor(mp_obj_get_int(color));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_stream_color_obj, ugfx_stream_color);

/// \method stream_stop()
///
/// Ends stream
///
STATIC mp_obj_t ugfx_stream_stop(void) {
    gdispStreamStop();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ugfx_stream_stop_obj, ugfx_stream_stop);



/* this doesnt appear to do anything, set background colour via styles
/// \method set_default_back_color()
///
STATIC mp_obj_t ugfx_container_set_default_back_color(mp_obj_t colour) {

	gwinSetDefaultBgColor(mp_obj_get_int(colour));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_container_set_default_back_color_fun_obj, ugfx_container_set_default_back_color);
STATIC MP_DEFINE_CONST_STATICMETHOD_OBJ(ugfx_container_set_default_back_color_obj, (mp_obj_t)&ugfx_container_set_default_back_color_fun_obj);

/// \method get_default_back_color()
///
STATIC mp_obj_t ugfx_container_get_default_back_color(void) {

	return mp_obj_new_int(gwinGetDefaultBgColor());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ugfx_container_get_default_back_color_fun_obj, ugfx_container_get_default_back_color);
STATIC MP_DEFINE_CONST_STATICMETHOD_OBJ(ugfx_container_get_default_back_color_obj, (mp_obj_t)&ugfx_container_get_default_back_color_fun_obj);
*/



/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
////////////////     Primitives   ///////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////


/// \method text(x, y, str, colour)
///
/// Draw the given text to the position `(x, y)` using the given colour.
///
STATIC mp_obj_t ugfx_text(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    mp_uint_t len;
    const char *data = mp_obj_str_get_data(args[2], &len);
    int x0 = mp_obj_get_int(args[0]);
    int y0 = mp_obj_get_int(args[1]);
    int col = mp_obj_get_int(args[3]);

    gdispDrawString(x0, y0, data, gwinGetDefaultFont(), col);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_text_obj, 4, 4, ugfx_text);


/// \method line(x1, y1, x2, y2, colour)
///
/// Draw a line from (x1,y1) to (x2,y2) using the given colour.
///
STATIC mp_obj_t ugfx_line(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[0]);
    int y0 = mp_obj_get_int(args[1]);
	int x1 = mp_obj_get_int(args[2]);
    int y1 = mp_obj_get_int(args[3]);
    int col = mp_obj_get_int(args[4]);

	gdispDrawLine(x0, y0, x1, y1, col);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_line_obj, 5, 5, ugfx_line);


/// \method thickline(x1, y1, x2, y2, colour, width, round)
///
/// Draw a line with a given thickness from (x1,y1) to (x2,y2) using the given colour.
/// Option to round the ends
///
STATIC mp_obj_t ugfx_thickline(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[0]);
    int y0 = mp_obj_get_int(args[1]);
	int x1 = mp_obj_get_int(args[2]);
    int y1 = mp_obj_get_int(args[3]);
    int col = mp_obj_get_int(args[4]);
    int width = mp_obj_get_int(args[5]);
    bool rnd = (mp_obj_get_int(args[6]) != 0);

	gdispDrawThickLine(x0, y0, x1, y1, col, width, rnd);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_thickline_obj, 7, 7, ugfx_thickline);


/// \method arc(x1, y1, r, angle1, angle2, colour)
///
/// Draw an arc having a centre point at (x1,y1), radius r, using the given colour.
///
STATIC mp_obj_t ugfx_arc(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    int x0 = mp_obj_get_int(args[0]);
    int y0 = mp_obj_get_int(args[1]);
	int r = mp_obj_get_int(args[2]);
    int col = mp_obj_get_int(args[5]);
    int a1 = mp_obj_get_int(args[3]);
    int a2 = mp_obj_get_int(args[4]);

	gdispDrawArc(x0, y0, r, a1, a2,col);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_arc_obj, 6, 6, ugfx_arc);

/// \method fill_arc(x1, y1, r, angle1, angle2, colour)
///
/// Fill an arc having a centre point at (x1,y1), radius r, using the given colour.
///
STATIC mp_obj_t ugfx_fill_arc(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    int x0 = mp_obj_get_int(args[0]);
    int y0 = mp_obj_get_int(args[1]);
	int r = mp_obj_get_int(args[2]);
	int col = mp_obj_get_int(args[5]);
    int a1 = mp_obj_get_int(args[3]);
    int a2 = mp_obj_get_int(args[4]);

	gdispFillArc(x0, y0, r, a1, a2,col);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_fill_arc_obj, 6, 6, ugfx_fill_arc);



/// \method circle(x1, y1, r, colour)
///
/// Draw a circle having a centre point at (x1,y1), radius r, using the given colour.
/// Option to round the ends
///
STATIC mp_obj_t ugfx_circle(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[0]);
    int y0 = mp_obj_get_int(args[1]);
	int r = mp_obj_get_int(args[2]);
    int col = mp_obj_get_int(args[3]);

	gdispDrawCircle(x0, y0, r, col);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_circle_obj, 4, 4, ugfx_circle);

/// \method fill_circle(x1, y1, r, colour)
///
/// Fill a circle having a centre point at (x1,y1), radius r, using the given colour.
/// Option to round the ends
///
STATIC mp_obj_t ugfx_fill_circle(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[0]);
    int y0 = mp_obj_get_int(args[1]);
	int r = mp_obj_get_int(args[2]);
    int col = mp_obj_get_int(args[3]);

	gdispFillCircle(x0, y0, r, col);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_fill_circle_obj, 4, 4, ugfx_fill_circle);



/// \method ellipse(x1, y1, a, b, colour)
///
/// Draw a ellipse having a centre point at (x1,y1), lengths a,b, using the given colour.
/// Option to round the ends
///
STATIC mp_obj_t ugfx_ellipse(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[0]);
    int y0 = mp_obj_get_int(args[1]);
	int a = mp_obj_get_int(args[2]);
	int b = mp_obj_get_int(args[3]);
    int col = mp_obj_get_int(args[4]);

	gdispDrawEllipse(x0, y0, a, b, col);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_ellipse_obj, 5, 5, ugfx_ellipse);

/// \method fill_ellipse(x1, y1, a, b, colour)
///
/// Fill a ellipse having a centre point at (x1,y1), lengths a,b, using the given colour.
/// Option to round the ends
///
STATIC mp_obj_t ugfx_fill_ellipse(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[0]);
    int y0 = mp_obj_get_int(args[1]);
	int a = mp_obj_get_int(args[2]);
	int b = mp_obj_get_int(args[3]);
    int col = mp_obj_get_int(args[4]);

	gdispFillEllipse(x0, y0, a, b, col);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_fill_ellipse_obj, 5, 5, ugfx_fill_ellipse);




/// \method polygon(x1, y1, array, colour)
///
/// Draw a polygon starting at (x1,y1), using the aarray of points, using the given colour.
/// Option to round the ends
///
STATIC mp_obj_t ugfx_polygon(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[0]);
    int y0 = mp_obj_get_int(args[1]);
	int col = mp_obj_get_int(args[3]);

	point ar[20];

	mp_obj_t *mp_arr;
	mp_obj_t *mp_arr2;
	uint len;
	uint len2;
	mp_obj_get_array(args[2], &len, &mp_arr);

	if (len <= 20){
		int i,j;
		j = 0;
		for (i = 0; i < len; i++){
			mp_obj_get_array(mp_arr[i], &len2, &mp_arr2);
			if (len2 == 2){
				point p = {mp_obj_get_int(mp_arr2[0]), mp_obj_get_int(mp_arr2[1])};
				ar[j++] = p;
			}
		}
		gdispDrawPoly(x0, y0, ar, j, col);
	}

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_polygon_obj, 4, 4, ugfx_polygon);




/// \method fill_polygon(x1, y1, array, colour)
///
/// Draw a polygon starting at (x1,y1), using the aarray of points, using the given colour.
/// Option to round the ends
///
STATIC mp_obj_t ugfx_fill_polygon(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[0]);
    int y0 = mp_obj_get_int(args[1]);
	int col = mp_obj_get_int(args[3]);

	point ar[20];

	mp_obj_t *mp_arr;
	mp_obj_t *mp_arr2;
	uint len;
	uint len2;
	mp_obj_get_array(args[2], &len, &mp_arr);

	if (len <= 20){
		int i,j;
		j = 0;
		for (i = 0; i < len; i++){
			mp_obj_get_array(mp_arr[i], &len2, &mp_arr2);
			if (len2 == 2){
				point p = {mp_obj_get_int(mp_arr2[0]), mp_obj_get_int(mp_arr2[1])};
				ar[j++] = p;
			}
		}
		gdispFillConvexPoly(x0, y0, ar, i, col);
	}

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_fill_polygon_obj, 4, 4, ugfx_fill_polygon);




/// \method area(x1, y1, a, b, colour)
///
/// Fill area from (x,y), with lengths x1,y1, using the given colour.
///
STATIC mp_obj_t ugfx_area(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[0]);
    int y0 = mp_obj_get_int(args[1]);
	int a = mp_obj_get_int(args[2]);
	int b = mp_obj_get_int(args[3]);
    int col = mp_obj_get_int(args[4]);

	gdispFillArea(x0, y0, a, b, col);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_area_obj, 5, 5, ugfx_area);

/// \method clear(color=ugfx.WHITE)
///
/// Clear screen
///
STATIC mp_obj_t ugfx_clear(mp_uint_t n_args, const mp_obj_t *args) {
    int color = n_args == 0 ? White : mp_obj_get_int(args[0]);
    gdispFillArea(0, 0, gdispGetWidth(), gdispGetHeight(), color);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_clear_obj, 0, 1, ugfx_clear);

/// \method box(x1, y1, a, b, colour)
///
/// Draw a box from (x,y), with lengths x1,y1, using the given colour.
///
STATIC mp_obj_t ugfx_box(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[0]);
    int y0 = mp_obj_get_int(args[1]);
	int a = mp_obj_get_int(args[2]);
	int b = mp_obj_get_int(args[3]);
    int col = mp_obj_get_int(args[4]);

	gdispDrawBox(x0, y0, a, b, col);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_box_obj, 5, 5, ugfx_box);


/*
/// \method next()
///
STATIC mp_obj_t ugfx_image_next(mp_obj_t img_obj) {
	if (img_obj != mp_const_none) {
		if (!MP_OBJ_IS_TYPE(img_obj, &ugfx_image_type)) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "img argument needs to be be a Image type"));
			return mp_const_none;
	   }
		ugfx_image_obj_t *image = img_obj;
		gdispImageNext(&(image->thisImage));
	}
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_image_next_obj, ugfx_image_next);
*/


/// \method display_image(x, y, image_object, {rotation})
///
STATIC mp_obj_t ugfx_display_image(mp_uint_t n_args, const mp_obj_t *args){
    // extract arguments
    //pyb_ugfx_obj_t *self = args[0];
	int x = mp_obj_get_int(args[0]);
	int y = mp_obj_get_int(args[1]);
	mp_obj_t img_obj = args[2];
	gdispImage imo;
	gdispImage *iptr;
    
	if (img_obj != mp_const_none) {
		if (MP_OBJ_IS_STR(img_obj)){
			const char *img_str = mp_obj_str_get_str(img_obj);
			gdispImageError er = gdispImageOpenFile(&imo, img_str);
			if (er != 0){
				nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Error opening file"));
				return mp_const_none;
			}
			iptr = &imo;
		}
		else if (MP_OBJ_IS_TYPE(img_obj, &ugfx_image_type))
		{
			iptr = &(((ugfx_image_obj_t*)img_obj)->thisImage);
		}
		else if (MP_OBJ_IS_TYPE(img_obj, &mp_type_bytearray))
		{
			void *items = ((mp_obj_array_t*)img_obj)->items;
			gdispImageError er = gdispImageOpenMemory(&imo, items);
			if (er != 0){
				nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Error finding image in bytearray"));
				return mp_const_none;
			}
			iptr = &imo;
		}
		else{
			nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "img argument needs to be be a bytearray (for image data) or String type (for file path)"));
			return mp_const_none;
		}


		coord_t	swidth, sheight;

		// Get the display dimensions
		swidth = gdispGetWidth();
		sheight = gdispGetHeight();

		if (n_args > 3)
			set_blit_rotation(get_orientation(mp_obj_get_int(args[3])));

		int err = gdispImageDraw(iptr, x, y, swidth, sheight, 0, 0);

		set_blit_rotation(GDISP_ROTATE_0);

		if (MP_OBJ_IS_STR(img_obj))
			gdispImageClose(&imo);

		print_image_error(err);

	}

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_display_image_obj, 3, 4, ugfx_display_image);


/*
/// \method display_image_file(x,y,file_name)
///
STATIC mp_obj_t ugfx_display_image_file(mp_uint_t n_args, const mp_obj_t *args) {

	mp_uint_t len;
	const char *file = mp_obj_str_get_data(args[2], &len);
	int x = mp_obj_get_int(args[0]);
	int y = mp_obj_get_int(args[1]);

	gdispImage myImage;
	coord_t	swidth, sheight;

	// Get the display dimensions
	swidth = gdispGetWidth();
	sheight = gdispGetHeight();

	// Set up IO for our image
	gdispImageOpenFile(&myImage, file);
	gdispImageDraw(&myImage, x, y, swidth, sheight, 0, 0);
	gdispImageClose(&myImage);


    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_display_image_file_obj, 3, 3, ugfx_display_image_file);
*/


STATIC const mp_map_elem_t ugfx_module_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_ugfx) },
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init), (mp_obj_t)&ugfx_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_backlight), (mp_obj_t)&ugfx_backlight_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit), (mp_obj_t)&ugfx_deinit_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_power_mode), (mp_obj_t)&ugfx_power_mode_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_text), (mp_obj_t)&ugfx_text_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_line), (mp_obj_t)&ugfx_line_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_box), (mp_obj_t)&ugfx_box_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_area), (mp_obj_t)&ugfx_area_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_clear), (mp_obj_t)&ugfx_clear_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_thickline), (mp_obj_t)&ugfx_thickline_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_circle), (mp_obj_t)&ugfx_circle_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_fill_circle), (mp_obj_t)&ugfx_fill_circle_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ellipse), (mp_obj_t)&ugfx_ellipse_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_fill_ellipse), (mp_obj_t)&ugfx_fill_ellipse_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_arc), (mp_obj_t)&ugfx_arc_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_fill_arc), (mp_obj_t)&ugfx_fill_arc_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_polygon), (mp_obj_t)&ugfx_polygon_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_fill_polygon), (mp_obj_t)&ugfx_fill_polygon_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_display_image), (mp_obj_t)&ugfx_display_image_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_orientation), (mp_obj_t)&ugfx_set_orientation_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_spi_clk), (mp_obj_t)&ugfx_spi_clk_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write_command), (mp_obj_t)&ugfx_write_command_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_width), (mp_obj_t)&ugfx_width_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_height), (mp_obj_t)&ugfx_height_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_poll), (mp_obj_t)&ugfx_poll_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disable_tear), (mp_obj_t)&ugfx_disable_tear_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_enable_tear), (mp_obj_t)&ugfx_enable_tear_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_tear_line), (mp_obj_t)&ugfx_set_tear_line_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ball_demo), (mp_obj_t)&ugfx_ball_demo_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_pixel), (mp_obj_t)&ugfx_get_pixel_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_default_font), (mp_obj_t)&ugfx_set_default_font_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_default_style), (mp_obj_t)&ugfx_set_default_style_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_print_fonts), (mp_obj_t)&ugfx_print_fonts_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send_tab), (mp_obj_t)&ugfx_send_tab_obj },

	//static methods
	//{ MP_OBJ_NEW_QSTR(MP_QSTR_set_default_back_color), (mp_obj_t)&ugfx_container_set_default_back_color_obj },
	//{ MP_OBJ_NEW_QSTR(MP_QSTR_get_default_back_color), (mp_obj_t)&ugfx_container_get_default_back_color_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_html_color), (mp_obj_t)&ugfx_html_color_obj },

    // instance methods: stream
    { MP_OBJ_NEW_QSTR(MP_QSTR_stream_start), (mp_obj_t)&ugfx_stream_start_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_stream_color), (mp_obj_t)&ugfx_stream_color_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_stream_stop), (mp_obj_t)&ugfx_stream_stop_obj },

	//class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_RED),        MP_OBJ_NEW_SMALL_INT(Red) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BLUE),       MP_OBJ_NEW_SMALL_INT(Blue) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GREEN),      MP_OBJ_NEW_SMALL_INT(Green) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BLACK),      MP_OBJ_NEW_SMALL_INT(Black) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WHITE),      MP_OBJ_NEW_SMALL_INT(White) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_YELLOW),     MP_OBJ_NEW_SMALL_INT(Yellow) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ORANGE),     MP_OBJ_NEW_SMALL_INT(Orange) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PURPLE),     MP_OBJ_NEW_SMALL_INT(Purple) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GREY),       MP_OBJ_NEW_SMALL_INT(Grey) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GRAY),       MP_OBJ_NEW_SMALL_INT(Gray) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PURPLE),     MP_OBJ_NEW_SMALL_INT(Purple) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_TIME_INFINITE), MP_OBJ_NEW_SMALL_INT(TIME_INFINITE)  },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TIME_IMMEDIATE), MP_OBJ_NEW_SMALL_INT(TIME_IMMEDIATE)  },

    { MP_OBJ_NEW_QSTR(MP_QSTR_JOY_RIGHT),  MP_OBJ_NEW_SMALL_INT(Buttons_JOY_Right) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_JOY_LEFT),   MP_OBJ_NEW_SMALL_INT(Buttons_JOY_Right) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_JOY_UP),     MP_OBJ_NEW_SMALL_INT(Buttons_JOY_Up) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_JOY_DOWN),   MP_OBJ_NEW_SMALL_INT(Buttons_JOY_Down) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_JOY_CENTER), MP_OBJ_NEW_SMALL_INT(Buttons_JOY_Center) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BTN_A),      MP_OBJ_NEW_SMALL_INT(Buttons_BTN_A) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BTN_B),      MP_OBJ_NEW_SMALL_INT(Buttons_BTN_B) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BTN_MENU),   MP_OBJ_NEW_SMALL_INT(Buttons_BTN_Menu) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_POWER_ON),   MP_OBJ_NEW_SMALL_INT(powerOn) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_POWER_OFF),   MP_OBJ_NEW_SMALL_INT(powerOff) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_POWER_SLEEP),   MP_OBJ_NEW_SMALL_INT(powerSleep) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_POWER_DEEP_SLEEP),   MP_OBJ_NEW_SMALL_INT(powerDeepSleep) },
	
	{ MP_OBJ_NEW_QSTR(MP_QSTR_FONT_SMALL),   MP_OBJ_NEW_SMALL_INT(0) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_FONT_TITLE),   MP_OBJ_NEW_SMALL_INT(1) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_FONT_NAME),   MP_OBJ_NEW_SMALL_INT(2) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_FONT_MEDIUM),   MP_OBJ_NEW_SMALL_INT(3) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_FONT_MEDIUM_BOLD),   MP_OBJ_NEW_SMALL_INT(4) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_FONT_FIXED),   MP_OBJ_NEW_SMALL_INT(5) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_FONT_FIXED_LG),   MP_OBJ_NEW_SMALL_INT(6) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Button), (mp_obj_t)&ugfx_button_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Container), (mp_obj_t)&ugfx_container_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Graph), (mp_obj_t)&ugfx_graph_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Font), (mp_obj_t)&ugfx_font_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_List), (mp_obj_t)&ugfx_list_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Textbox), (mp_obj_t)&ugfx_textbox_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Style), (mp_obj_t)&ugfx_style_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Keyboard), (mp_obj_t)&ugfx_keyboard_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Label), (mp_obj_t)&ugfx_label_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Image), (mp_obj_t)&ugfx_image_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Checkbox), (mp_obj_t)&ugfx_checkbox_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Imagebox), (mp_obj_t)&ugfx_imagebox_type },
};

STATIC MP_DEFINE_CONST_DICT (
    ugfx_module_globals,
    ugfx_module_dict_table
);

const mp_obj_module_t mp_module_ugfx = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&ugfx_module_globals,
};

#endif // MICROPY_HW_HAS_UGFX
