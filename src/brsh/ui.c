/*
 
 Brain Rarely Accepts Incoherent Nonsense (BRAIN)
 Copyright 2012-2013 Joshua Hawcroft
 
 This file is part of BRAIN.
 
 BRAIN is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 BRAIN is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with BRAIN.  If not, see <http://www.gnu.org/licenses/>.
 
 */
/* curses front-end for the brsh program */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <locale.h>
#include <ncurses.h>
#include <form.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdbool.h>
#include <signal.h>

#include "event.h"
#include "ui.h"
#include "../fatal.h"
#include "../../includes/client.h"


/* compile-time configuration */
#define BRSH_RUNLOOP_SLEEP_USEC 25000

#define BRSH_NAME               "brsh"
#define BRSH_VERSION            "1.0"


extern brain_client_t *g_client;


static int g_brsh_input_display_lines = 3;
static int g_brsh_input_maximum_lines = 10;
static bool g_brsh_show_instructions = true;

static WINDOW *win_output = NULL;
static FORM *frm_input = NULL;
static FIELD *fld_input = NULL;


static void brsh_init(void);



int brsh_ui_inited()
{
    return (frm_input != NULL);
}


static void resized_(int in_signal)
{
    brsh_teardown();
    brsh_init();
    refresh();
}


static void brsh_init(void)
{
    /* handle window resize */
    signal(SIGWINCH, &resized_);
    
    /* initalize curses */
    initscr();
    cbreak();
    noecho();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE); /* enable cursor keys and others */
    
    /* create the output window */
    win_output = newwin(LINES - g_brsh_input_display_lines - 1, COLS, 0, 0);
    scrollok(win_output, TRUE);
    
    /* create the input field */
    fld_input = new_field(g_brsh_input_display_lines,
                          COLS,
                          LINES - g_brsh_input_display_lines,
                          0,
                          g_brsh_input_maximum_lines - g_brsh_input_display_lines,
                          1);
    static FIELD *fields[2] = { NULL, NULL }; /* not sure if it needs this to hang around or not,
                                               so we'll make it static anyway */
    fields[0] = fld_input;
    frm_input = new_form(fields);
    post_form(frm_input);
    
    /* draw a line */
    move(LINES - g_brsh_input_display_lines - 1, 0);
    for (int i = 0; i < COLS; i++)
        addch(ACS_HLINE);
    if (g_brsh_show_instructions)
        mvprintw(LINES - g_brsh_input_display_lines - 1, COLS - 15, " F1 = Help ");
    refresh();
    move(LINES - g_brsh_input_display_lines, 0);
    
    /* post welcome banner */
    brsh_print(BRSH_NAME " " BRSH_VERSION "\n");
}


void brsh_printf(char const *in_output, ...)
{
    va_list arg_list;
    va_start(arg_list, in_output);
    int y, x;
    getyx(stdscr, y, x);
    vwprintw(win_output, in_output, arg_list);
    va_end(arg_list);
    wrefresh(win_output);
    set_current_field(frm_input, fld_input);
    move(y, x);
}


void brsh_print(char const *in_output)
{
    int y, x;
    getyx(stdscr, y, x);
    waddstr(win_output, in_output);
    wrefresh(win_output);
    set_current_field(frm_input, fld_input);
    move(y, x);
}


static char const* brsh_get_input(void)
{
    /* get the field content */
    form_driver(frm_input, REQ_VALIDATION);
    char *fld_buffer = field_buffer(fld_input, 0);
    
    /* trim the left-hand side */
    while (isspace(*fld_buffer)) fld_buffer++;
    
    /* copy the buffer */
    static char *input_buffer = NULL;
    if (input_buffer) free(input_buffer);
    long size = strlen(fld_buffer);
    input_buffer = malloc(size + 1);
    if (!input_buffer)
        brain_fatal_("Not enough memory\n");
    strcpy(input_buffer, fld_buffer);
    
    /* trim the right-hand side */
    for (long i = size-1; i >= 0; i--)
    {
        if (!isspace(input_buffer[i]))
        {
            input_buffer[i+1] = 0;
            break;
        }
    }
    
    return input_buffer;
}


static void brsh_clear_input(void)
{
    form_driver(frm_input, REQ_VALIDATION);
    set_field_buffer(fld_input, 0, "");
}


void brsh_teardown(void)
{
    if (frm_input)
    {
        unpost_form(frm_input);
        free_form(frm_input);
    }
    if (fld_input) free_field(fld_input);
    frm_input = NULL;
    fld_input = NULL;
    
    endwin();
}


static void brsh_runloop(void)
{
    timeout(0); /* don't wait for input characters;
                 also allows us to do other things while the user isn't typing */
    for (;;)
    {
        int input_chr = getch();
        switch (input_chr)
        {
            case KEY_LEFT:
                form_driver(frm_input, REQ_LEFT_CHAR);
                break;
            case KEY_RIGHT:
                form_driver(frm_input, REQ_RIGHT_CHAR);
                break;
            case KEY_DOWN:
                form_driver(frm_input, REQ_DOWN_CHAR);
                break;
            case KEY_UP:
                form_driver(frm_input, REQ_UP_CHAR);
                break;
            case KEY_ENTER:
            case '\n':
            case '\r':
                brsh_handle_input(brsh_get_input());
                brsh_clear_input();
                break;
            case KEY_BACKSPACE:
            case KEY_DC:
            case 127:
                form_driver(frm_input, REQ_LEFT_CHAR);
                form_driver(frm_input, REQ_DEL_CHAR);
                break;
            case 27: /* ESC */
                brsh_clear_input();
                break;
            case KEY_F(1):
                brsh_handle_input("help");
                break;
            case ERR:
                break;
            case KEY_RESIZE:
                break;
            default:
                form_driver(frm_input, input_chr);
                break;
        }
        
        brsh_handle_io();
        usleep(BRSH_RUNLOOP_SLEEP_USEC);
    }
}


void brsh_shutdown(int in_exit_status)
{
    brsh_teardown();
    exit(in_exit_status);
}


void brsh(void)
{
    setlocale(LC_ALL, "");
    
    brsh_init();
    brsh_runloop();
    brsh_shutdown(EXIT_SUCCESS);
}


