#include "rand.h"
#include "std.h"
#include "sheetgame.h"
#include "sheetmenu.h"
#include "sheettext.h"
#include "sheetcongrats.h"
#include "level0.h"
#include "level1.h"
#include "level2.h"
#include "level3.h"
#include "level4.h"
#include "level5.h"
#include "level6.h"
#include "level7.h"
#include "level13.h"
u8 menuind;
u8 difficulty = 1;

void debug_indic(){
  if(indicate) {
    bg[0 * 4] = 0;
    bg[0 * 4 + 1] = 0;
    bg[0 * 4 + 3] = 1;
  } else {
    bg[0 * 4 + 3] = 0;
  }
}
bool game = false;
bool inmenu = true;
#define BGIND(x, y) (((y))*(320/16)+((x)))
#define BGSQUARE(x, y) (((y)/16)*(320/16)+((x)/16))
#define SKYSCREEN   do{for(int i = 0; i < (240 * 320) / 256; i++) {	\
    bg[0x500 + i * 4] = 0; \
    bg[0x500 + i * 4 + 1] = 240; \
    bg[0x500 + i * 4 + 3] = 1; \
    }}while(0)


bool justpressed[12];
bool held[12];

u16 deaths = 0;
bool died;

void copy_level(u8* from, u8 to) {
  for(int x = 0; x < 20; x++) {
    for(int y = 0; y < 15; y++){
      levels[to][x][y] = from[y*20+x];
    }
  }
}
void just_pressed() {
  for(int i = 0; i < 12; i++) {
    if(input[i] && justpressed[i]) {
      held[i] = true;
      justpressed[i] = false;
    }
    if(input[i] && !held[i]) {      
      justpressed[i] = true;     
    }
    if(!input[i]) {
      justpressed[i] = false;
      held[i] = false;
    }
  }
}
bool done = false;
void load_next_level() {
  our_sprites[0].yvel = 0;
  current_level++;
  if(current_level == difficulty*2+2) done=true;
  for(int _x = 0; _x < 20; _x++) {
    for(int _y = 0; _y < 15; _y++) {
      u8 x = 0, y = 240;
      bool coll = true;
      bool kill = false;
      bool next = false;
      switch(levels[current_level][_x][_y]) {
      case 1: //platform left
	x = 64;
	break;
      case 2: //platform mid
	x = 80;
	break;
      case 3: //platform right
	x = 96;
	break;
      case 4: //ground top
	x = 48;
	break;
      case 5: //ground middle
	x = 32;
	break;
      case 6: //spikes
	x = 112; kill = true;
	break;
      case 7: // door top
	x = 144; y = 224; next = true; coll = false;
	break;
      case 8: // door bottom
	x = 144; next = true; coll = false;
	break;
      case 9: // cloud
	x = 16; coll = false;
	break;
      case 0:  //sky
      default: //sky
	coll = false;
	break;
      }
      bg[0x500 + BGIND(_x, _y) * 4] = x;
      bg[0x500 + BGIND(_x, _y) * 4 + 1] = y;
      bg[0x500 + BGIND(_x, _y) * 4 + 3] = 1 | coll * 0x80 | next * 0x40 | kill * 0x20;
    }
  }
}
void show_in_middle(u8 x, u8 y, u16 nx, u8 ny, u8 fillx, u8 filly) {
  u32 nbef = (320-(nx))/32;
  u32 yaf = (240-(ny))/16;
  int i;
  for(i = 0; i < ny/16; i++) {
    int j;
    for(j = 0; j < nbef;  j++) {
      bg[0x500 + BGIND(j, i) * 4] = fillx;
      bg[0x500 + BGIND(j, i) * 4 + 1] = filly;
      bg[0x500 + BGIND(j, i) * 4 + 3] = 1;
    }
    for(int oldj = j; j < nx/16 + oldj; j++) {
      bg[0x500 + BGIND(j, i) * 4] = x + ((j-oldj) * 16);
      bg[0x500 + BGIND(j, i) * 4 + 1] = y + (i * 16);
      bg[0x500 + BGIND(j, i) * 4 + 3] = 1;
    }
    for(int oldj = j; j < nbef + oldj; j++) {
      bg[0x500 + BGIND(j, i) * 4] = fillx;
      bg[0x500 + BGIND(j, i) * 4 + 1] = filly;
      bg[0x500 + BGIND(j, i) * 4 + 3] = 1;
    }
  }
  for(int oldi = i; i < yaf+oldi; i++) {
    for(int j=0; j<320/16; j++) {
      bg[0x500 + BGIND(j, i) * 4] = fillx;
      bg[0x500 + BGIND(j, i) * 4 + 1] = filly;
      bg[0x500 + BGIND(j, i) * 4 + 3] = 1;
    }
  }
}

void show_menu() {
  if(justpressed[2]) {menuind++;}
  if(justpressed[0]) {menuind--;}
  if(menuind==255) menuind = 3;
  if(menuind>3) menuind = 0;
  if(difficulty==255) difficulty = 3;
  if(difficulty>3) difficulty = 0;
  for(int i = 0; i < 4; i++) {
    if(menuind == 0) {
      bg[0x500 + BGIND(8+i, 11) * 4] = 80+i*16;
    } else {
      bg[0x500 + BGIND(8+i, 11) * 4] = 16+i*16;
    }
    bg[0x500 + BGIND(8+i, 11) * 4 + 1] = 240;
    bg[0x500 + BGIND(8+i, 11) * 4 + 3] = 1;
  }
  for(int i = 0; i < 4; i++) {
    if(menuind == 1) {
      bg[0x500 + BGIND(8+i, 12) * 4] = 64+i*16;
    } else {
      bg[0x500 + BGIND(8+i, 12) * 4] = 0+i*16;
    }
    bg[0x500 + BGIND(8+i, 12) * 4 + 1] = 224;
    bg[0x500 + BGIND(8+i, 12) * 4 + 3] = 1;
  }
  for(int i = 0; i < 4; i++) {
    if(menuind == 2) {
      bg[0x500 + BGIND(8+i, 13) * 4] = 64+i*16;
    } else {
      bg[0x500 + BGIND(8+i, 13) * 4] = 0+i*16;
    }
    bg[0x500 + BGIND(8+i, 13) * 4 + 1] = 208;
    bg[0x500 + BGIND(8+i, 13) * 4 + 3] = 1;
  }
  for(int i = 0; i < 4; i++) {
    u8 x, y;
    switch(difficulty) {
    case 0: x = 0; y = 176; break;
    case 1: x = 0; y = 192; break;
    case 2: x = 128; y = 176; break;
    case 3: x = 128; y = 192; break;
    default: die();
    }
    if(menuind == 3) {
      bg[0x500 + BGIND(8+i, 14) * 4] = x+64+i*16;
    } else {
      bg[0x500 + BGIND(8+i, 14) * 4] = x+i*16;
    }
    bg[0x500 + BGIND(8+i, 14) * 4 + 1] = y;
    bg[0x500 + BGIND(8+i, 14) * 4 + 3] = 1;
  }  
}
void animate() {
  sprite it = our_sprites[0];
  if((it.xadd + it.xvel) > 0) {
    if(it.anistage == 1) {
      it.anistage = 0;
      it.sheetx = 64;
    } else {
      it.anistage = 1;
      it.sheetx = 48;
    }
  } else if ((it.xadd + it.xvel) < 0) {
    if(it.anistage == 1) {
      it.anistage = 0;
      it.sheetx = 80;
    } else {
      it.anistage = 1;
      it.sheetx = 96;
    }
  } else {
    if (it.sheetx == 80 || it.sheetx == 96 || it.sheetx == 32
	|| it.sheetx == 128) {
      it.sheetx = 32;
    } else {
      it.sheetx = 16;
    }
  }
  if(input[2]) {
    if (it.sheetx == 80 || it.sheetx == 96 || it.sheetx == 32
	|| it.sheetx == 128) {
      it.sheetx = 128;
    } else {
      it.sheetx = 112;
    }
  }
  our_sprites[0] = it;
}

bool next_level;
bool collides(bool player, sprite it, int xpos, int ypos) {
  u8 xsize = ((it.sizes&0xf0)/2);
  u8 ysize = ((it.sizes&0x0f)*8);
  u16 xlim = 320 - xsize; // xxxx0000 / 2 = 0xxxx000 so xxxx * 8
  u8 ylim = 240 - ysize;
  if (xpos > xlim) { // collides right
    return true;
  }
  if (xpos < 0) { // collides left
    return true;
  }
  if (ypos > ylim) {
    if(player){
      deaths++;
      died = true;
    }
    return true;
  }
  if (ypos < 0) {

    return true;
  }
  for (int i = 0; i <= xsize/16; i++) {
    for (int j = 0; j <= ysize/16; j++) {
      u8 flags = bg[0x500 + BGSQUARE(xpos + i*16, ypos + j*16) * 4 + 3];
      if (flags & 0x20) {
	deaths++;
	died = true;
      }
      if (flags & 0x40) {
	died = true;
	next_level = true;
      }
      if (flags & 0x80) {
	return true;
      }
    }
  }

  return false;
}
  
void move_sprites() {
  for(int j = 0; j < nsprites; j++) {
    sprite it = our_sprites[j];
    if(it.xvel + it.xadd > 0) { // going right
      for(int i = it.x + it.xvel + it.xadd; i >= it.x; i--) {
	if(!collides(j==0, it, i, it.y)) {
	  it.x = i;
	  break;
	} else {
	  it.xvel = 0;
	}
      }
    } else if (it.xvel + it.xadd <= 0) { // collides left
      for(int i = it.x + it.xvel + it.xadd; i < it.x; i++) {
	if(!collides(j==0, it, i, it.y)) {
	  it.x = i;
	  break;
	} else {
	  it.xvel = 0;
	}
      }
    } 

    bool moved = false;
    if(it.yvel + it.yadd > 0) {
      for(int i = it.y + it.yvel + it.yadd; i >= it.y; i--) {
	if(!collides(j==0, it, it.x, i)) {
	  it.y = i;
	  break;
	} else {
	  it.groundedp = true;
	  moved = true;
	  it.yvel = 0;
	}
      }
    } else if (it.yvel + it.yadd <= 0) {
      for(int i = it.y + it.yvel + it.yadd; i <= it.y; i++) {
	if(!collides(j==0, it, it.x, i)) {
	  it.y = i;
	  break;
	} else {
	  it.yvel = 0;
	}
      }
    }
    if(!moved) it.groundedp = false;
    if(died) {it.x=0;it.y=0;died=false;}
    it.xadd = 0;
    it.yadd = 0;
    
    our_sprites[j] = it;
  }
}

void gravity () {
  for (int i = 0; i < nsprites; i++) {
    if (!our_sprites[i].groundedp) {
      our_sprites[i].yvel += 1;
    }
  }
}

void process_input() {
  /* 	Key(SDL_SCANCODE_UP, 0);
	Key(SDL_SCANCODE_RIGHT, 1);
	Key(SDL_SCANCODE_DOWN, 2);
	Key(SDL_SCANCODE_LEFT, 3);
	Key(SDL_SCANCODE_A, 4);
	Key(SDL_SCANCODE_S, 5);
	Key(SDL_SCANCODE_Z, 6);
	Key(SDL_SCANCODE_X, 7);
	Key(SDL_SCANCODE_Q, 8);
	Key(SDL_SCANCODE_W, 9);
	Key(SDL_SCANCODE_COMMA, 10);
	Key(SDL_SCANCODE_PERIOD, 11); */
  sprite player = our_sprites[0];
  if(input[10]) {
    indicate = !indicate;
  }

  if((input[0] || input[6]) && player.groundedp) {
    indicate = true;
    if(input[2]) {
      player.yvel -= 6;
    } else {
      player.yvel -= 5;
    }
    player.groundedp = false;
  }
  if(input[2]) {
    our_sprites[0] = player;
    return;
  }
  if(input[1]) {
    player.xadd += 1;
  }
  if(input[3]) {
    player.xadd -= 1;
  }
  our_sprites[0] = player;
}
__attribute__((interrupt)) __attribute__((used)) void vblank(void) {
  just_pressed();
  if(game) {
    display_sprites();
  } else if (inmenu) {
    show_menu();
  }
  postvblank = true;
}

void swap_in(u8 data[], u8 cmap[256][3]) {
  for(int i = 0; i < 256; i++) {
    palette[i*3] =   cmap[i][0];
    palette[i*3+1] = cmap[i][1];
    palette[i*3+2] = cmap[i][2];
  }
  for(int i = 0; i < 256 * 256; i++) {
    sheet[i] = data[i];
  }
}

bool ji6 = false;
int main(void) {
  register_interrupt(&vblank, 16);
  u32 timer = 0;
  copy_level(level0, 0);
  copy_level(level1, 1);
  copy_level(level2, 2);
  copy_level(level3, 3);
  copy_level(level4, 4);
  copy_level(level5, 5);  
  copy_level(level6, 6);
  copy_level(level7, 7);  

  copy_level(level13, 13);

 menu:
  swap_in(sm_header_data, sm_header_data_cmap);
  show_in_middle(0, 0, 224, 160, 0, 240);

  while(1) {
    asm volatile ( "wfi" );
    if(postvblank){
      timer++;

      postvblank = false;
      if(input[6] && inmenu) {
	if(menuind == 0) {
	  goto play;
	}
	if(menuind == 1) {
	  swap_in(st_header_data, st_header_data_cmap);
	  inmenu = false;
	  SKYSCREEN;
	  show_in_middle(0, 0, 256, 128, 0, 240);
	}
	if(menuind == 2) {
	  swap_in(st_header_data, st_header_data_cmap);
	  inmenu = false;
	  SKYSCREEN;
	  show_in_middle(0, 128, 256, 128, 0, 240);
	}
	if(menuind == 3 && !ji6) {
	  difficulty++;
	  ji6 = true;
	}
      }
      if(!input[6]) {ji6 = false;}
      if(input[7] && !inmenu) {
	swap_in(sm_header_data, sm_header_data_cmap);
	inmenu = true;
	goto menu;
      }
    }
  }
  
 play:
  game = true;
  current_level = difficulty * 2 - 1;
  swap_in(sg_header_data, sg_header_data_cmap);
  load_next_level();
  our_sprites[nsprites].sheetx = 16;
  our_sprites[nsprites].sheety = 0;
  our_sprites[nsprites].x = 0;
  our_sprites[nsprites].y = 0;
  our_sprites[nsprites].visiblep = 1;
  our_sprites[nsprites].sizes = 0x24;
  our_sprites[nsprites].yvel = 10;
  nsprites++;
  while(1) {
    asm volatile ( "wfi" );
    if (postvblank) {
      timer++;
      move_sprites();
      if(0 == (timer % 4)) {
	gravity();
      }
      process_input();
      if(0 == (timer % 10)) {
	animate();
      }
      if(next_level) {
	load_next_level();
	next_level = false;
      }
      if(done) {
	goto congrats;
      }
      postvblank = false;
    }
  }
 congrats:
  swap_in(sc_header_data, sc_header_data_cmap);
  SKYSCREEN;
  show_in_middle(0, 0, 256, 192, 0, 240);
  switch(difficulty) {
  default:
  case 0:
    for (int i = 0; i < 3; i++) {
      bg[0x500 + BGIND(8+i, 5)*4] = 0+i*16;
      bg[0x500 + BGIND(8+i, 5)*4 + 1] = 221;
      bg[0x500 + BGIND(8+i, 5)*4 + 3] = 1;
    }
    break;
  case 1:
    for (int i = 0; i < 5; i++) {
      bg[0x500 + BGIND(7+i, 5)*4] = 43+i*16;
      bg[0x500 + BGIND(7+i, 5)*4 + 1] = 221;
      bg[0x500 + BGIND(7+i, 5)*4 + 3] = 1;
    }
    break;
  case 2:
    for (int i = 0; i < 3; i++) {
      bg[0x500 + BGIND(8+i, 5)*4] = 123+i*16;
      bg[0x500 + BGIND(8+i, 5)*4 + 1] = 221;
      bg[0x500 + BGIND(8+i, 5)*4 + 3] = 1;
    }
    break;
  case 3:
    for (int i = 0; i < 7; i++) {
      bg[0x500 + BGIND(6+i, 5)*4] = 0+i*16;
      bg[0x500 + BGIND(6+i, 5)*4 + 1] = 240;
      bg[0x500 + BGIND(6+i, 5)*4 + 3] = 1;
    }
    break;
  }
  u8 d1 = deaths/100;
  u8 d2 = (deaths/10)%10;
  u8 d3 = deaths%10;
  bg[0x500 + BGIND(9, 8)*4] = d1*16;
  bg[0x500 + BGIND(9, 8)*4 + 1] = 200;
  bg[0x500 + BGIND(9, 8)*4 + 3] = 1;
  
  bg[0x500 + BGIND(10, 8)*4] = d2*16;
  bg[0x500 + BGIND(10, 8)*4 + 1] = 200;
  bg[0x500 + BGIND(10, 8)*4 + 3] = 1;
  
  bg[0x500 + BGIND(11, 8)*4] = d3*16;
  bg[0x500 + BGIND(11, 8)*4 + 1] = 200;
  bg[0x500 + BGIND(11, 8)*4 + 3] = 1;
  while(1){asm volatile ( "wfi" );}
}
