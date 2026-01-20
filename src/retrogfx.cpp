 /*
  * UAE - The Un*x Amiga Emulator
  *
  * uae to libretro graphic interface
  *
  * Copyright 2021 Chips
  *
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include <unistd.h>
#include <signal.h>

#include "uae.h"
#include "options.h"

#include "xwin.h"

#include "SDL.h"

SDL_Surface *prSDLScreen = NULL;

// Dummy menu part

int mainMenu_vpos=1;
int mainMenu_sound=-1;
int mainMenu_frameskip=0;
int mainMenu_throttle=0;
int mainMenu_autosave=-1;
int saveMenu_n_savestate=0;

void init_text(int splash)
{
}

void quit_text(void)
{
}

int run_mainMenu()
{
    return 0;
}

void _text_draw_window_bar(SDL_Surface *sf, int x, int y, int w, int h, int per, int max, char *title)
{
}

// Display

int uae4all_keystate[256];
int need_frameskip = 0;
unsigned int uae4all_numframes=0;
int prefs_gfx_framerate, changed_gfx_framerate;
char *gfx_mem=NULL;
unsigned gfx_rowbytes=0;

/* v099: Direct rendering overscan - ZERO overhead Y-offset */
/* TESTOWO: Y-offset = 24 żeby było widać efekt (jak v098) */
overscan_settings_t overscan_config = {
    OVERSCAN_ONLY_Y,  /* mode - POPRAWIONE: musi być ONLY_Y dla 288 linii! */
    24,               /* y_offset - TESTOWO 24 (przesuwa obraz w dół) */
    0,                /* y_stretch - not implemented yet */
    0,                /* x_offset - not implemented yet */
    0,                /* x_stretch - not implemented yet */
    0,                /* pending_mode_change */
    OVERSCAN_ONLY_Y   /* pending_mode - POPRAWIONE */
};

/* v098: Deklaracje extern dla zmiennych z drawing.cpp */
extern int VISIBLE_LEFT_BORDER_VAR;
extern int VISIBLE_RIGHT_BORDER_VAR;
extern int LINETOSCR_X_ADJUST_BYTES_VAR;
extern int GFXVIDINFO_WIDTH_VAR;
extern int GFXVIDINFO_HEIGHT_VAR;

static int red_bits,  green_bits,  blue_bits;
static int red_shift, green_shift, blue_shift;

int libretro_frame_end=0;

void flush_screen (void)
{
    libretro_frame_end = 1;
}

int lockscr (void)
{
    return 1;
}

int needmousehack (void)
{
    return 1;
}


void handle_events (void)
{
}

#if 0
int graphics_init (void)
{
    if (!init_colors ())
        return 0;

    return(1);
}
#endif

static __inline__ int bitsInMask (unsigned long mask)
{
    /* count bits in mask */
    int n = 0;
    while (mask)
    {
        n += mask & 1;
        mask >>= 1;
    }
    return n;
}

static __inline__ int maskShift (unsigned long mask)
{
    /* determine how far mask is shifted */
    int n = 0;
    while (!(mask & 1))
    {
        n++;
        mask >>= 1;
    }
    return n;
}


static int init_colors (void)
{
    int i;

#ifdef DEBUG_GFX
    dbg("Function: init_colors");
#endif

    /* Truecolor: */
    //red_bits = bitsInMask(prSDLScreen->format->Rmask);
    //green_bits = bitsInMask(prSDLScreen->format->Gmask);
    //blue_bits = bitsInMask(prSDLScreen->format->Bmask);
    //red_shift = maskShift(prSDLScreen->format->Rmask);
    //green_shift = maskShift(prSDLScreen->format->Gmask);
    //blue_shift = maskShift(prSDLScreen->format->Bmask);
    red_bits = 5;
    green_bits = 6;
    blue_bits = 5;
    red_shift = 5 + 6;
    green_shift = 5;
    blue_shift = 0;
    alloc_colors64k (red_bits, green_bits, blue_bits, red_shift, green_shift, blue_shift);
    for (i = 0; i < 4096; i++)
        xcolors[i] = xcolors[i] * 0x00010001;

    return 1;
}

/* v099: Pojedynczy bufor 320×288 - render to większy, wysyłaj środek */
static int allocate_internal_buffer(void)
{
    /* Zwolnij stary bufor jeśli istnieje */
    if (gfx_mem != NULL) {
        free(gfx_mem);
        gfx_mem = NULL;
    }

    /* JEDEN bufor 320×288 - tak jak v098 ale BEZ gfx_mem_output! */
    gfx_mem = (char*)calloc(1, 320 * 288 * 2);
    if (!gfx_mem) return 0;

    gfx_rowbytes = 320 * 2;  /* 640 bytes (RGB565) */

    /* Ustaw borders dla 288 linii - jak v098 ONLY_Y */
    VISIBLE_LEFT_BORDER_VAR = 72;
    VISIBLE_RIGHT_BORDER_VAR = 392;
    LINETOSCR_X_ADJUST_BYTES_VAR = 144;
    GFXVIDINFO_WIDTH_VAR = 320;
    GFXVIDINFO_HEIGHT_VAR = 288;  /* KLUCZOWE! Jak v098! */

    return 1;
}


int graphics_setup (void)
{
    /* v099: Pojedynczy bufor - jak v097 */
    if (!allocate_internal_buffer())
        return 0;

    if (!init_colors ())
        return 0;

    return(1);
}

void graphics_leave (void)
{
}

int check_prefs_changed_gfx (void)
{
    extern int mainMenu_vpos;
    static int last_vpos=0;
    int ret=(last_vpos!=mainMenu_vpos);
    last_vpos=mainMenu_vpos;
    return ret;
}

void gui_purge_events(void)
{
}


