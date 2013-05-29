/****************************************************************************
 *  config.c
 *
 *  Genesis Plus GX configuration file support
 *
 *  Eke-Eke (2008)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ***************************************************************************/

#ifndef ___CONFIG_H_
#define ___CONFIG_H_

#define CONFIG_VERSION "GENPLUS-GX 1.4.1"

#define MAX_INPUTS 8

#define MAXPATHLEN 256

#define RAND_MAX 32767

#define strnicmp strncasecmp

/* Genesis controller keys */
#define MAX_KEYS 8

// We can't include full arrays. We'll have to define this in a .c file somewhere.:)
extern char GG_ROM[256];       // "/dev_usb000/genplus/cart/ggenie.bin"
extern char AR_ROM[256];       // "/dev_usb000/genplus/cart/areplay.bin"
extern char SK_ROM[256];       // "/dev_usb000/genplus/cart/sk.bin"
extern char SK_UPMEM[256];     // "/dev_usb000/genplus/cart/sk2chip.bin"

/* Key configuration structure */
typedef struct 
{
  uint8 device;
  uint8 port;
  uint8 padtype;
} t_input_config;

/****************************************************************************
 * Config Option 
 *
 ****************************************************************************/
typedef struct 
{
  char version[16];
  uint8 hq_fm;
  uint8 psgBoostNoise;
  int32 psg_preamp;
  int32 fm_preamp;
  uint8 filter;
  int16 lp_range;
  int16 low_freq;
  int16 high_freq;
  float lg;
  float mg;
  float hg;
  float rolloff;
  uint8 dac_bits;
  uint8 region_detect;
  uint8 force_dtack;
  uint8 addr_error;
  uint8 tmss;
  uint8 bios_enabled;
  uint8 lock_on;
  uint8 hot_swap;
  uint8 romtype;
  int16 xshift;
  int16 yshift;
  int16 xscale;
  int16 yscale;
  uint8 tv_mode;
  uint8 aspect;
  uint8 overscan;
  uint8 render;
  uint8 ntsc;
  uint8 bilinear;
#ifdef HW_RVL
  uint8 trap;
  float gamma;
#endif
  uint8 gun_cursor[2];
  uint8 invert_mouse;
  uint16 pad_keymap[4][MAX_KEYS];
  uint32 wpad_keymap[4*3][MAX_KEYS];
  t_input_config input[MAX_INPUTS];
  uint8 s_default;
  uint8 s_device;
  int8 sram_auto;
  int8 state_auto;
  int8 bg_color;
  int16 screen_w;
  uint8 ask_confirm;
  float bgm_volume;
  float sfx_volume;
} t_config;

/* Global data */
t_config config;


extern void config_save(void);
extern void config_default(void);
extern int ps3_update_input(void);

#define osd_input_Update ps3_update_input


#endif /* _CONFIG_H_ */

