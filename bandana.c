/* BANDANA FIGHTERS GBA - build unique (genere depuis src/ et include/). */


typedef unsigned char  u8;  typedef signed char  s8;
typedef unsigned short u16; typedef signed short s16;
typedef unsigned int   u32; typedef signed int   s32;
typedef s32 fx;                      /* virgule fixe 24.8 */
#define FX(n)       ((fx)(n) * 256)
#define FX_INT(v)   ((v) >> 8)
#define SCREEN_W 240
#define SCREEN_H 160
#define ABS(a)   ((a) < 0 ? -(a) : (a))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define CLAMP(v,lo,hi) ((v) < (lo) ? (lo) : ((v) > (hi) ? (hi) : (v)))
#define ARRAY_LEN(a) ((int)(sizeof(a)/sizeof((a)[0])))
/* Couleur 0xRRGGBB -> RGB555 */
#define C24(h) ((u16)((((h)>>19)&31) | ((((h)>>11)&31)<<5) | ((((h)>>3)&31)<<10)))
/* Couche materielle : seul hw_gba.c (ROM) ou test/hw_host.c (tests PC) l'implemente. */
#define KEY_A      0x001
#define KEY_B      0x002
#define KEY_SELECT 0x004
#define KEY_START  0x008
#define KEY_RIGHT  0x010
#define KEY_LEFT   0x020
#define KEY_UP     0x040
#define KEY_DOWN   0x080
#define KEY_R      0x100
#define KEY_L      0x200

#define SB_BG0 28   /* decor            */
#define SB_BG1 29   /* texte / HUD      */
#define SB_BG2 30   /* panneaux de menu */

void hw_init(void);
void hw_vsync(void);
void hw_flush(void);                       /* OAM shadow + scrolls -> materiel */
u16  hw_keys(void);                        /* 1 = appuye */
u16 *hw_bg_tiles(void);                    /* charblock 0 (0x06000000)  */
u16 *hw_bg_map(int screenblock);
u16 *hw_obj_tiles(void);                   /* 0x06010000                */
u16 *hw_pal_bg(void);
u16 *hw_pal_obj(void);
u16 *hw_oam_shadow(void);                  /* 128 * 4 u16               */
void hw_set_scroll(int bg, int x, int y);
void hw_display(int on);                   /* ecran force noir si 0     */
void hw_sram_read(u32 off, u8 *dst, u32 n);
void hw_sram_write(u32 off, const u8 *src, u32 n);
void hw_snd_tone(u16 freq, u16 env, u16 duty);   /* canal 1 (carre)  */
void hw_snd_noise(u16 env, u16 ctl);             /* canal 4 (bruit)  */
/* ============================ utilitaires ============================ */
void rng_seed(u32 s);
u32  rng_u32(void);
int  rng_range(int n);              /* 0..n-1 */
int  isin(int phase);               /* phase 0..255 -> -256..256 (approx.) */

typedef struct { u16 held, pressed, released; } input_t;
extern input_t g_in;
void input_update(void);

/* ============================ camera ================================= */
typedef struct { int x, y, world_w, world_h, shake_t, shake_amp, sx, sy; } camera_t;
extern camera_t g_cam;
void cam_init(int world_w, int world_h);
void cam_follow(int target_x_px);   /* suit une cible horizontalement (zone morte) */
void cam_shake(int frames, int amp);
void cam_update(void);              /* applique scroll BG + decalage tremblement   */

/* ============================ sprites (OAM) ========================== */
typedef enum { SPR_8, SPR_16, SPR_16x32, SPR_32 } spr_size_t;
#define SPR_HFLIP 1
void spr_begin(void);
void spr_world(int wx, int wy, spr_size_t sz, int tile, int pal, int flags);   /* coordonnees monde */
void spr_end(void);
int  spr_count(void);

/* ============================ art / tuiles =========================== */
#define ART_RYDER 0
#define ART_LUMA 1
#define ART_JACOB 2
#define ART_WALLACE 3
#define ART_KD 4
#define ART_GRUNT 5
#define ART_RUSHER 6
#define ART_THROWER 7
#define TILE_HUMAN(style, frame) ((style) * 32 + (frame) * 8)    /* 16x32 = 8 tuiles */
#define TILE_BAT(frame)   (256 + (frame) * 4)                     /* 16x16 */
#define TILE_BOSS(frame)  (264 + (frame) * 16)                    /* 32x32 */
#define TILE_BULLET 296
#define TILE_EBULLET 297
#define TILE_COIN 298
#define TILE_CROSS 299
#define TILE_SPARK 300
#define TILE_BIGDOT 301
#define TILE_DOOR 304                                             /* 16x32 */
#define PAL_PLAYER 0
#define PAL_GRUNT 1
#define PAL_RUSHER 2
#define PAL_THROWER 3
#define PAL_BAT 4
#define PAL_BOSS 5
#define PAL_MISC 6
#define PAL_FLASH 7
void art_init(void);
void art_set_player_palette(int chr);
void art_update_player_palette(int chr, u32 frame);   /* KD : bandana arc-en-ciel */

/* ============================ UI (texte, barres) ===================== */
#define UI_TXT 0
#define UI_PNL 1
#define TXT_WHITE 1
#define TXT_YELLOW 2
#define TXT_RED 3
#define TXT_GREEN 4
#define TXT_CYAN 5
void ui_init(void);
void ui_clear(int layer);
void ui_clear_rows(int layer, int row, int nrows);
void ui_text(int layer, int col, int row, const char *s, int pal);     /* 2 rangees de tuiles par ligne */
void ui_num(int layer, int col, int row, int v, int width, int pal);   /* nombre cadre a droite */
void ui_bar(int col, int row, int cur, int max, int cells);
void ui_panel(int col, int row, int w, int h);
void ui_text_center(int layer, int row, const char *s, int pal);

/* ============================ monde / collisions ===================== */
typedef struct { fx x, y, vx, vy; u8 w, h; s8 face; u8 ground; } body_t;
typedef struct { int w_tiles, h_tiles; const u8 *col; } level_t;
extern const level_t *g_level;
void level_init_arena(void);
int  level_solid(int tx, int ty);
int  box_solid(int x0, int y0, int x1, int y1);         /* pixels, inclusif */
void body_move(body_t *b);                               /* deplacement + collisions tuiles */
void body_push(body_t *b, int dx_px);
int  rect_hit(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh);
#define GROUND_Y 144          /* y du sol en pixels (haut des tuiles solides) */

/* ============================ donnees ================================ */
typedef struct {
    const char *name; u8 art; u16 speed, jump, dmg, hp;     /* multiplicateurs 8.8 */
    u8 invulnerable, lethal, rainbow; u32 skin, body, bandana, hair;
} char_def_t;
enum { CH_RYDER, CH_LUMA, CH_JACOB, CH_WALLACE, CH_KD, CH_COUNT };
extern const char_def_t g_chars[CH_COUNT];

typedef struct {
    const char *name; u8 melee; s16 dmg; u8 cd; u16 range_px; s16 speed;   /* speed : fx/frame */
    u8 pellets; u8 spread; u8 jitter; u8 pierce; u16 price; u8 pw, ph;
} weapon_def_t;
enum { W_PISTOL, W_SHOTGUN, W_SMG, W_RAILGUN, W_GAUNTLET, W_COUNT };
extern const weapon_def_t g_weapons[W_COUNT];

typedef struct {
    const char *name; s16 hp; s16 speed; u8 dmg; u8 atk_range; u8 atk_cd; u8 points; u8 w, h;
    u8 ranged, flying, art, pal;
} enemy_def_t;
enum { E_GRUNT, E_RUSHER, E_THROWER, E_BAT, E_TYPES };
extern const enemy_def_t g_enemy_defs[E_TYPES];

enum { B_DMG, B_MAXHP, B_HEAL, B_SPEED, B_FIRE, B_DEFENSE, B_COINS, B_COUNT };
typedef struct { const char *name; } bonus_def_t;
extern const bonus_def_t g_bonus_defs[B_COUNT];

typedef struct { u32 dmg, speed, fire, defense, coins; } run_bonus_t;   /* multiplicateurs 8.8 cumulatifs */
extern run_bonus_t g_bonus;
typedef struct { int score, coins; } run_t;
extern run_t g_run;
void bonus_reset(void);
void bonus_apply(int id);                                /* applique + conserve (cumulatif) */

/* ============================ entites ================================ */
typedef struct {
    body_t b; s16 hp, maxhp; u8 chr, weapon, aim, aim_free, invuln, cd, anim, hurt_t, dead, dead_t;
} player_t;
extern player_t g_player;
void player_init(int chr);
void player_update(void);
void player_render(void);
void player_hurt(int dmg, int src_x_px);
void player_heal(int amount);
int  player_center_x(void);
int  player_center_y(void);
void player_set_weapon(int w);

typedef struct {
    body_t b; s16 hp, maxhp; u8 active, type, dead, dead_t, atk_cd, flash, atk_anim, anim;
    u8 bstate, btimer, bphase; u16 phase16; s16 hover_y; s16 dive_x, dive_y; u8 hit_cd;
} enemy_t;
#define ENEMY_MAX 6
#define ENEMY_ALIVE_MAX 4
#define BATS_ALIVE_MAX 2
extern enemy_t g_enemies[ENEMY_MAX];
void enemies_clear(void);
int  enemies_alive(void);
int  enemies_spawn_random(void);                         /* 1 si un ennemi a ete cree */
void enemies_update(void);
void enemies_render(void);
void enemy_damage(enemy_t *e, int dmg, int kdir, int lethal);

typedef struct {
    body_t b; s16 hp, maxhp; u8 active, dead, dead_t, state, timer, melee_cd, slam_cd, flash, anim;
} boss_t;
extern boss_t g_boss;
void boss_spawn(int wave_score);
void boss_update(void);
void boss_render(void);
void boss_damage(int dmg, int lethal);

typedef struct { fx x, y, vx, vy; fx range; u8 active, w, h, friendly, pierce, lethal; u8 hitmask; s16 dmg; } proj_t;
#define PROJ_MAX 24
extern proj_t g_proj[PROJ_MAX];
void proj_clear(void);
void proj_clear_hostile(void);
proj_t *proj_alloc(void);
void proj_update(void);
void proj_render(void);
void enemy_throw(const enemy_t *e, int dir);

typedef struct { fx x, y, vy; u8 active, value, t; } coin_t;
#define COIN_MAX 10
extern coin_t g_coins[COIN_MAX];
void coins_clear(void);
void coin_spawn(int x_px, int y_px, int value);
void coins_update(void);
void coins_render(void);

typedef struct { s16 x, y; u8 life, kind; } fxp_t;
#define FXP_MAX 8
void fxp_clear(void);
void fxp_spawn(int x_px, int y_px, int life);
void fxp_update(void);
void fxp_render(void);

void weapon_fire(player_t *p);                           /* generique : toutes les armes */

/* ============================ audio / save =========================== */
enum { SFX_SHOOT, SFX_HEAVY, SFX_HIT, SFX_DEATH, SFX_COIN, SFX_JUMP, SFX_HURT, SFX_BONUS, SFX_BOSS, SFX_DOOR, SFX_COUNT };
void sfx_play(int id);

typedef struct {
    u32 magic; u16 version; u16 sum; u32 coins; u16 best_wave; u8 sel_char, equipped, owned_mask, kd_unlocked, pad[2];
} save_t;
extern save_t g_save;
void save_load(void);
void save_write(void);

/* ============================ HUD ==================================== */
void hud_reset(void);
void hud_update(int wave, int kills, int needed, int is_boss);

/* ============================ modes ================================== */
typedef struct { void (*enter)(void); void (*update)(void); void (*render)(void); void (*leave)(void); } game_mode_t;
extern const game_mode_t MODE_TITLE, MODE_WAVES;
void game_set_mode(const game_mode_t *m);
void game_frame(void);
extern void (*g_hook_enemy_down)(void);
extern void (*g_hook_boss_down)(void);
extern void (*g_hook_player_down)(void);
/* introspection pour les tests PC */
int  waves_phase(void);
int  waves_wave(void);
int  waves_kills(void);
int  waves_needed(void);
int  waves_offer(int i);
int  waves_is_boss(void);

/* ===== hw_gba.c ===== */
/* Implementation GBA reelle. Aucune bibliotheque : registres directs. NON TESTE sur ROM. */
#define R16(a) (*(volatile u16 *)(a))
#define DISPCNT   R16(0x04000000)
#define VCOUNT    R16(0x04000006)
#define BGCNT(n)  R16(0x04000008 + 2*(n))
#define BGOFS_X(n) R16(0x04000010 + 4*(n))
#define BGOFS_Y(n) R16(0x04000012 + 4*(n))
#define KEYINPUT  R16(0x04000130)
#define SND1_L R16(0x04000060)
#define SND1_H R16(0x04000062)
#define SND1_X R16(0x04000064)
#define SND4_L R16(0x04000078)
#define SND4_H R16(0x0400007C)
#define SNDCNT_L R16(0x04000080)
#define SNDCNT_H R16(0x04000082)
#define SNDCNT_X R16(0x04000084)
#define OAM_MEM ((volatile u16 *)0x07000000)

static u16 oam_shadow[128 * 4];
static int scroll[4][2];

void hw_init(void) {
    DISPCNT = 0x0080;                                   /* forced blank pendant l'init */
    BGCNT(0) = 3 | (0 << 2) | (SB_BG0 << 8);            /* prio 3 */
    BGCNT(1) = 1 | (0 << 2) | (SB_BG1 << 8);            /* prio 1 */
    BGCNT(2) = 2 | (0 << 2) | (SB_BG2 << 8);            /* prio 2 */
    SNDCNT_X = 0x80; SNDCNT_L = 0xFF77; SNDCNT_H = 0x0002;  /* PSG on, volume max */
    for (int i = 0; i < 128; i++) { oam_shadow[i*4] = 0x0200; oam_shadow[i*4+1] = 0; oam_shadow[i*4+2] = 0; }
}
void hw_display(int on) {
    /* mode 0 | OBJ 1D | BG0 BG1 BG2 | OBJ */
    DISPCNT = on ? (0x0000 | 0x0040 | 0x0100 | 0x0200 | 0x0400 | 0x1000) : 0x0080;
}
void hw_vsync(void) { while (VCOUNT >= 160) {} while (VCOUNT < 160) {} }
void hw_flush(void) {
    for (int i = 0; i < 128 * 4; i++) OAM_MEM[i] = oam_shadow[i];
    for (int b = 0; b < 3; b++) { BGOFS_X(b) = (u16)scroll[b][0]; BGOFS_Y(b) = (u16)scroll[b][1]; }
}
u16 hw_keys(void) { return (u16)(~KEYINPUT & 0x03FF); }
u16 *hw_bg_tiles(void)  { return (u16 *)0x06000000; }
u16 *hw_bg_map(int sb)  { return (u16 *)(0x06000000 + sb * 0x800); }
u16 *hw_obj_tiles(void) { return (u16 *)0x06010000; }
u16 *hw_pal_bg(void)    { return (u16 *)0x05000000; }
u16 *hw_pal_obj(void)   { return (u16 *)0x05000200; }
u16 *hw_oam_shadow(void){ return oam_shadow; }
void hw_set_scroll(int bg, int x, int y) { scroll[bg][0] = x; scroll[bg][1] = y; }

/* SRAM : bus cartouche partage avec la ROM -> ces fonctions DOIVENT s'executer depuis l'IWRAM (code ARM). */
#define IWRAM_CODE __attribute__((section(".iwram"), long_call, noinline, target("arm")))
IWRAM_CODE void hw_sram_read(u32 off, u8 *dst, u32 n) {
    volatile u8 *s = (volatile u8 *)0x0E000000;
    for (u32 i = 0; i < n; i++) dst[i] = s[off + i];
}
IWRAM_CODE void hw_sram_write(u32 off, const u8 *src, u32 n) {
    volatile u8 *s = (volatile u8 *)0x0E000000;
    for (u32 i = 0; i < n; i++) s[off + i] = src[i];
}
void hw_snd_tone(u16 freq, u16 env, u16 duty) {
    SND1_L = 0; SND1_H = (u16)(env | duty); SND1_X = (u16)(0x8000 | (freq & 0x7FF));
}
void hw_snd_noise(u16 env, u16 ctl) { SND4_L = env; SND4_H = (u16)(0x8000 | ctl); }

/* ===== util.c ===== */
static u32 rs = 0x1234567u;
void rng_seed(u32 s) { rs = s ? s : 0x1234567u; }
u32 rng_u32(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
int rng_range(int n) { return n <= 1 ? 0 : (int)((rng_u32() >> 8) % (u32)n); }
int isin(int phase) {                       /* approximation parabolique, sans table */
    int p = phase & 255, neg = p >= 128;
    if (neg) p -= 128;
    int v = (p * (128 - p)) >> 4;           /* 0..256 */
    return neg ? -v : v;
}
input_t g_in;
void input_update(void) {
    u16 k = hw_keys();
    g_in.pressed = (u16)(k & ~g_in.held);
    g_in.released = (u16)(~k & g_in.held);
    g_in.held = k;
}

/* ===== camera.c ===== */
camera_t g_cam;
void cam_init(int ww, int wh) { g_cam.x = g_cam.y = 0; g_cam.world_w = ww; g_cam.world_h = wh; g_cam.shake_t = g_cam.shake_amp = 0; }
void cam_follow(int tx) {                   /* zone morte de 40 px au centre */
    int want = tx - SCREEN_W / 2;
    if (want > g_cam.x + 20) g_cam.x = want - 20;
    else if (want < g_cam.x - 20) g_cam.x = want + 20;
    g_cam.x = CLAMP(g_cam.x, 0, MAX(0, g_cam.world_w - SCREEN_W));
}
void cam_shake(int frames, int amp) { g_cam.shake_t = frames; g_cam.shake_amp = amp; }
void cam_update(void) {
    g_cam.sx = g_cam.sy = 0;
    if (g_cam.shake_t > 0) {                /* meme decalage pour BG ET sprites : une seule camera */
        g_cam.shake_t--;
        g_cam.sx = (int)(g_cam.shake_t & 1) ? g_cam.shake_amp : -g_cam.shake_amp;
    }
    hw_set_scroll(0, g_cam.x + g_cam.sx, g_cam.y + g_cam.sy);
}

/* ===== level.c ===== */
static u8 arena_col[30 * 20];
static level_t arena = { 30, 20, arena_col };
const level_t *g_level = &arena;
void level_init_arena(void) {
    for (int i = 0; i < 30 * 20; i++) arena_col[i] = 0;
    for (int y = 18; y < 20; y++) for (int x = 0; x < 30; x++) arena_col[y * 30 + x] = 1;   /* sol */
    g_level = &arena;
}
int level_solid(int tx, int ty) {
    if (tx < 0 || tx >= g_level->w_tiles) return 1;      /* murs invisibles gauche/droite */
    if (ty < 0) return 0;                                /* ciel ouvert */
    if (ty >= g_level->h_tiles) return 1;
    return g_level->col[ty * g_level->w_tiles + tx];
}
int box_solid(int x0, int y0, int x1, int y1) {
    for (int ty = y0 >> 3; ty <= (y1 >> 3); ty++)
        for (int tx = x0 >> 3; tx <= (x1 >> 3); tx++)
            if (level_solid(tx, ty)) return 1;
    return 0;
}
void body_move(body_t *b) {
    int w = b->w, h = b->h;
    fx nx = b->x + b->vx;
    int l = FX_INT(nx), r = l + w - 1, t = FX_INT(b->y), bt = t + h - 1;
    if (box_solid(l, t, r, bt)) {
        if (b->vx > 0) nx = FX((((r >> 3) << 3) - w));
        else if (b->vx < 0) nx = FX((((l >> 3) + 1) << 3));
        b->vx = 0;
    }
    b->x = nx;
    fx ny = b->y + b->vy;
    l = FX_INT(b->x); r = l + w - 1; t = FX_INT(ny); bt = t + h - 1;
    b->ground = 0;
    if (box_solid(l, t, r, bt)) {
        if (b->vy > 0) { ny = FX((((bt >> 3) << 3) - h)); b->ground = 1; }
        else if (b->vy < 0) ny = FX((((t >> 3) + 1) << 3));
        b->vy = 0;
    }
    b->y = ny;
}
void body_push(body_t *b, int dx_px) {
    fx ovx = b->vx, ovy = b->vy;
    b->vx = FX(dx_px); b->vy = 0;
    body_move(b);
    b->vx = ovx; b->vy = ovy;
}
int rect_hit(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh) {
    return ax < bx + bw && bx < ax + aw && ay < by + bh && by < ay + ah;
}

/* ===== sprites.c ===== */
static int n_used;
static const u8 shape_of[4] = { 0, 0, 2, 0 };
static const u8 size_of[4]  = { 0, 1, 2, 2 };
static const u8 dim_w[4] = { 8, 16, 16, 32 };
static const u8 dim_h[4] = { 8, 16, 32, 32 };
void spr_begin(void) { n_used = 0; }
void spr_world(int wx, int wy, spr_size_t sz, int tile, int pal, int flags) {
    int sx = wx - g_cam.x - g_cam.sx, sy = wy - g_cam.y - g_cam.sy;
    if (sx <= -dim_w[sz] || sx >= SCREEN_W || sy <= -dim_h[sz] || sy >= SCREEN_H) return;   /* culling */
    if (n_used >= 128) return;
    u16 *o = hw_oam_shadow() + n_used * 4;
    o[0] = (u16)((sy & 0xFF) | (shape_of[sz] << 14));
    o[1] = (u16)((sx & 0x1FF) | ((flags & SPR_HFLIP) ? 0x1000 : 0) | (size_of[sz] << 14));
    o[2] = (u16)(tile | (pal << 12));          /* priorite 0 : au-dessus des fonds */
    n_used++;
}
void spr_end(void) {
    u16 *o = hw_oam_shadow();
    for (int i = n_used; i < 128; i++) { o[i*4] = 0x0200; o[i*4+1] = 0; o[i*4+2] = 0; }
}
int spr_count(void) { return n_used; }

/* ===== data.c ===== */
/* Tables de donnees portees du jeu HTML (valeurs x0.6 pour l'echelle 240x160, ms -> frames a 60 Hz).
   speed (fx/frame) = px/s * 0.6 / 60 * 256 = px/s * 2.56 */
const char_def_t g_chars[CH_COUNT] = {
 { "RYDER",  ART_RYDER,   320, 256, 256, 256, 0,0,0, 0x6b4327, 0x2b3a4a, 0x3ecf6b, 0x151515 },
 { "LUMA",   ART_LUMA,    256, 333, 256, 256, 0,0,0, 0xcf9866, 0x3a3550, 0xf4d33c, 0x3b2418 },
 { "JACOB",  ART_JACOB,   256, 256, 320, 256, 0,0,0, 0xf2c9a0, 0x3a4a3c, 0x3b82f6, 0x4a3520 },
 { "WALLACE",ART_WALLACE, 256, 256, 256, 333, 0,0,0, 0xa5703f, 0x4a3434, 0xe5302f, 0x201510 },
 { "KD",     ART_KD,      256, 256, 256, 256, 1,1,1, 0xffffff, 0xffffff, 0xff00ff, 0xdddddd },
};
const weapon_def_t g_weapons[W_COUNT] = {
 /* name       melee dmg cd  range speed pel spr jit pierce price pw ph */
 { "PISTOLET",   0,  7, 25, 204, 1638, 1, 0,  0, 0,   0, 4, 3 },
 { "FUSIL POMPE",0,  4, 38, 126, 1536, 4, 38, 0, 0,  60, 3, 3 },
 { "MITRAILL.",  0,  5,  9, 192, 1741, 1, 0, 15, 0, 140, 4, 3 },
 { "PERFORANT",  0, 20, 42, 336, 2509, 1, 0,  0, 1, 220, 6, 4 },
 { "GANT ELEC.", 1, 15, 29,  31,    0, 1, 0,  0, 0, 380, 0, 0 },
};
const enemy_def_t g_enemy_defs[E_TYPES] = {
 /* name      hp  speed dmg rng cd  pts  w  h ranged fly art           pal */
 { "VOYOU",   28, 217,  7, 23, 60, 10, 12, 28, 0, 0, ART_GRUNT,   PAL_GRUNT },
 { "FONCEUR", 18, 384,  6, 20, 45, 14, 10, 26, 0, 0, ART_RUSHER,  PAL_RUSHER },
 { "LANCEUR", 16, 141,  5, 138,102, 18, 10, 26, 1, 0, ART_THROWER,PAL_THROWER },
 { "CHAUVE-S",12, 320,  6, 16, 90, 12, 14, 10, 0, 1, 0,           PAL_BAT },
};
const bonus_def_t g_bonus_defs[B_COUNT] = {
 { "DEGATS +20%" }, { "VIE MAX +20" }, { "SOIN 50%" }, { "VITESSE +12%" },
 { "CADENCE +15%" }, { "DEFENSE +15%" }, { "PIECES +20%" },
};
run_bonus_t g_bonus;
run_t g_run;
void bonus_reset(void) { g_bonus.dmg = g_bonus.speed = g_bonus.fire = g_bonus.defense = g_bonus.coins = 256; }
void bonus_apply(int id) {                       /* multiplicatif pour les taux, additif pour les PV */
    switch (id) {
    case B_DMG:     g_bonus.dmg     = MIN(g_bonus.dmg * 307 >> 8, 256u * 12); break;      /* x1.20 */
    case B_MAXHP:   g_player.maxhp += 20; g_player.hp += 20; break;
    case B_HEAL:    player_heal(g_player.maxhp / 2); break;
    case B_SPEED:   g_bonus.speed   = MIN(g_bonus.speed * 287 >> 8, 256u * 3); break;      /* x1.12 */
    case B_FIRE:    g_bonus.fire    = MAX(g_bonus.fire * 218 >> 8, 64u); break;            /* x0.85 */
    case B_DEFENSE: g_bonus.defense = MAX(g_bonus.defense * 218 >> 8, 64u); break;         /* x0.85 */
    case B_COINS:   g_bonus.coins   = MIN(g_bonus.coins * 307 >> 8, 256u * 12); break;
    }
}

/* ===== save.c ===== */
save_t g_save;
#define SAVE_MAGIC 0x42464741u   /* 'BFGA' */
static u16 checksum(const save_t *s) {
    const u8 *p = (const u8 *)s; u32 a = 0;
    for (u32 i = 8; i < sizeof(save_t); i++) a = a * 31 + p[i];       /* apres magic+version+sum */
    return (u16)(a ^ (a >> 16));
}
static void defaults(void) {
    u8 *p = (u8 *)&g_save; for (u32 i = 0; i < sizeof(save_t); i++) p[i] = 0;
    g_save.magic = SAVE_MAGIC; g_save.version = 1; g_save.sel_char = CH_RYDER; g_save.owned_mask = 1; g_save.equipped = W_PISTOL;
}
void save_load(void) {
    hw_sram_read(0, (u8 *)&g_save, sizeof(save_t));
    if (g_save.magic != SAVE_MAGIC || g_save.version != 1 || g_save.sum != checksum(&g_save)) defaults();
}
void save_write(void) {
    g_save.sum = checksum(&g_save);
    hw_sram_write(0, (const u8 *)&g_save, sizeof(save_t));
}

/* ===== sfx.c ===== */
/* Effets PSG (canal 1 carre + canal 4 bruit). Env : vol<<12 | dir<<11 | pas<<8 ; duty<<6 */
void sfx_play(int id) {
    switch (id) {
    case SFX_SHOOT: hw_snd_noise(0xA100, 0x0044); break;
    case SFX_HEAVY: hw_snd_noise(0xF200, 0x0053); break;
    case SFX_HIT:   hw_snd_tone(1700, 0x8100, 0x80); break;
    case SFX_DEATH: hw_snd_noise(0xC300, 0x0066); break;
    case SFX_COIN:  hw_snd_tone(1900, 0x7100, 0x40); break;
    case SFX_JUMP:  hw_snd_tone(1500, 0x6100, 0x80); break;
    case SFX_HURT:  hw_snd_tone(900,  0xD200, 0x80); break;
    case SFX_BONUS: hw_snd_tone(1950, 0xA200, 0x40); break;
    case SFX_BOSS:  hw_snd_tone(500,  0xF300, 0x80); break;
    case SFX_DOOR:  hw_snd_tone(1750, 0x9200, 0x40); break;
    }
}

/* ===== art.c ===== */
/* Art PLACEHOLDER genere en code : vrais formats GBA (sprites 4bpp 1D, tuiles BG 4bpp, palettes 15 bits).
   Les vrais PNG (grit) remplaceront ces fonctions sans toucher au moteur : memes index de tuiles/palettes. */
static u8 cv[32][32];
static int cw, ch;
static void cv_clear(int w, int h) { cw = w; ch = h; for (int y = 0; y < 32; y++) for (int x = 0; x < 32; x++) cv[y][x] = 0; }
static void cv_px(int x, int y, int c) { if (x >= 0 && y >= 0 && x < cw && y < ch) cv[y][x] = (u8)c; }
static void cv_rect(int x0, int y0, int x1, int y1, int c) { for (int y = y0; y <= y1; y++) for (int x = x0; x <= x1; x++) cv_px(x, y, c); }
static void cv_ell(int cx, int cy, int rx, int ry, int c) {
    for (int y = cy - ry; y <= cy + ry; y++) for (int x = cx - rx; x <= cx + rx; x++) {
        int dx = x - cx, dy = y - cy;
        if (dx * dx * ry * ry + dy * dy * rx * rx <= rx * rx * ry * ry) cv_px(x, y, c);
    }
}
static void cv_outline(void) {                       /* contour sombre (index 1) autour des pixels peints */
    u8 t[32][32];
    for (int y = 0; y < ch; y++) for (int x = 0; x < cw; x++) t[y][x] = cv[y][x];
    for (int y = 0; y < ch; y++) for (int x = 0; x < cw; x++) if (!t[y][x]) {
        if ((x > 0 && t[y][x-1]) || (x < cw-1 && t[y][x+1]) || (y > 0 && t[y-1][x]) || (y < ch-1 && t[y+1][x])) cv[y][x] = 1;
    }
}
/* canvas -> tuiles 4bpp, layout 1D (rangee par rangee) */
static void cv_blit(u16 *vram, int tile0, int w, int h) {
    int tw = w / 8;
    for (int ty = 0; ty < h / 8; ty++) for (int tx = 0; tx < tw; tx++) {
        u16 *t = vram + (tile0 + ty * tw + tx) * 16;
        for (int y = 0; y < 8; y++) for (int k = 0; k < 2; k++) {
            u16 v = 0;
            for (int i = 0; i < 4; i++) v |= (u16)((cv[ty*8+y][tx*8+k*4+i] & 15) << (i * 4));
            t[y * 2 + k] = v;
        }
    }
}
/* ---------- humanoides 16x32 : index 1 contour, 2 peau, 3 haut, 4 bandana, 5 cheveux, 6 blanc, 7 pantalon ---------- */
typedef struct { u8 hair, wide, item; } style_t;
static const style_t styles[8] = {
    {0,0,1}, /* RYDER afro   */ {1,0,1}, /* LUMA cheveux longs */ {2,0,1}, /* JACOB court */ {3,1,1}, /* WALLACE massif */
    {4,0,1}, /* KD pointes   */ {5,0,0}, /* VOYOU bonnet */ {6,0,0}, /* FONCEUR crete */ {7,0,2}, /* LANCEUR capuche + bouteille */
};
static void draw_human(const style_t *s, int frame) {
    cv_clear(16, 32);
    int c0 = s->wide ? 2 : 4, c1 = s->wide ? 13 : 11;
    int lh = 30, rh = 30, lx = s->wide ? 4 : 5, rx = s->wide ? 8 : 8;
    if (frame == 1) lh = 27; else if (frame == 2) rh = 27;
    if (frame == 3) { lh = 26; rh = 29; }
    cv_rect(lx, 22, lx + 2, lh, 7); cv_rect(rx, 22, rx + 2 + (s->wide ? 1 : 0), rh, 7);
    cv_rect(lx, lh, lx + 3, lh + 1, 6); cv_rect(rx, rh, rx + 3 + (s->wide ? 1 : 0), rh + 1, 6);   /* chaussures */
    cv_rect(c0, 13, c1, 21, 3);                                                                  /* torse */
    cv_rect(c1, 14, MIN(c1 + 2, 15), 16, 2);                                                     /* bras */
    if (s->item == 1) cv_rect(MIN(c1 + 2, 14), 15, 15, 16, 5);                                   /* arme */
    if (s->item == 2) cv_rect(MIN(c1 + 2, 14), 12, MIN(c1 + 3, 15), 16, 6);                      /* bouteille */
    int h = s->hair;                                                                             /* cheveux (derriere la tete) */
    if (h == 0) cv_ell(8, 5, 7, 5, 5);
    if (h == 1) { cv_rect(3, 3, 12, 6, 5); cv_rect(2, 5, 4, 15, 5); }
    if (h == 2 || h == 3) cv_rect(4, 3, 11, 5, 5);
    if (h == 4) { cv_rect(4, 3, 11, 4, 5); cv_rect(5, 1, 6, 2, 5); cv_rect(8, 0, 9, 2, 5); cv_rect(11, 1, 12, 2, 5); }
    if (h == 5) cv_rect(3, 3, 12, 5, 5);
    if (h == 6) { cv_rect(6, 1, 9, 4, 5); cv_rect(5, 3, 11, 4, 5); }
    if (h == 7) { cv_ell(8, 7, 7, 6, 3); }
    cv_rect(4, 5, 11, 12, 2);                                                                    /* tete */
    cv_rect(4, 5, 11, 6, 4);                                                    /* bandana */
    cv_rect(1, 6 + (frame == 1), 3, 7 + (frame == 1), 4);                                        /* pan du bandana */
    cv_px(9, 9, 6); cv_px(10, 9, 1);                                                             /* oeil */
    cv_outline();
}
static void draw_bat(int frame) {
    cv_clear(16, 16);
    cv_ell(8, 8, 2, 3, 3); cv_px(7, 4, 3); cv_px(9, 4, 3); cv_px(7, 7, 6); cv_px(9, 7, 6);
    if (frame == 0) { cv_rect(2, 2, 5, 7, 4); cv_rect(10, 2, 13, 7, 4); cv_px(1, 4, 4); cv_px(14, 4, 4); }
    else            { cv_rect(1, 8, 5, 11, 4); cv_rect(10, 8, 14, 11, 4); cv_px(0, 10, 4); cv_px(15, 10, 4); }
    cv_outline();
}
static void draw_boss(int frame) {                    /* LE CAID 32x32 */
    cv_clear(32, 32);
    cv_rect(9, 22, 14, 30, 7); cv_rect(17, 22, 22, 30, 7); cv_rect(8, 30, 14, 31, 6); cv_rect(17, 30, 23, 31, 6);
    cv_rect(6, 12, 25, 22, 3);
    if (frame == 0) { cv_rect(2, 13, 5, 24, 2); cv_rect(26, 13, 29, 24, 2); }
    else            { cv_rect(2, 3, 5, 14, 2);  cv_rect(26, 3, 29, 14, 2); }
    cv_rect(10, 2, 21, 12, 2); cv_rect(9, 1, 22, 4, 5); cv_rect(10, 6, 21, 7, 4); cv_rect(5, 6, 9, 8, 4);
    cv_px(17, 9, 6); cv_px(18, 9, 1); cv_px(13, 9, 6); cv_px(14, 9, 1); cv_rect(13, 11, 18, 11, 1);
    cv_outline();
}
static void small_sprites(u16 *v) {
    cv_clear(8, 8); cv_ell(4, 4, 2, 2, 1); cv_blit(v, TILE_BULLET, 8, 8);                 /* tir joueur (jaune) */
    cv_clear(8, 8); cv_ell(4, 4, 2, 2, 2); cv_px(4, 4, 5); cv_blit(v, TILE_EBULLET, 8, 8);  /* tir ennemi (rouge) */
    cv_clear(8, 8); cv_ell(4, 4, 3, 3, 3); cv_ell(4, 4, 1, 2, 4); cv_blit(v, TILE_COIN, 8, 8);
    cv_clear(8, 8); cv_px(4,0,8); cv_px(4,1,8); cv_px(4,6,8); cv_px(4,7,8); cv_px(0,4,8); cv_px(1,4,8); cv_px(6,4,8); cv_px(7,4,8); cv_blit(v, TILE_CROSS, 8, 8);
    cv_clear(8, 8); cv_px(4,1,9); cv_px(3,3,9); cv_px(4,3,9); cv_px(5,3,9); cv_px(2,4,9); cv_px(3,4,5); cv_px(4,4,5); cv_px(5,4,5); cv_px(6,4,9);
    cv_px(3,5,9); cv_px(4,5,9); cv_px(5,5,9); cv_px(4,7,9); cv_blit(v, TILE_SPARK, 8, 8);
    cv_clear(8, 8); cv_ell(4, 4, 3, 2, 1); cv_blit(v, TILE_BIGDOT, 8, 8);
}
static void draw_door(void) {
    cv_clear(16, 32);
    cv_rect(0, 0, 15, 31, 6); cv_rect(2, 3, 13, 31, 7); cv_rect(7, 12, 8, 24, 5); cv_rect(5, 20, 10, 21, 5); cv_rect(6, 22, 9, 23, 5);
    cv_outline();
}
#define PAL(bank, i, col) (po[(bank) * 16 + (i)] = (col))
static void set_actor_pal(int bank, u32 skin, u32 body, u32 band, u32 hair, u32 pants) {
    u16 *po = hw_pal_obj();
    PAL(bank,0,0); PAL(bank,1,C24(0x0a0612)); PAL(bank,2,C24(skin)); PAL(bank,3,C24(body)); PAL(bank,4,C24(band));
    PAL(bank,5,C24(hair)); PAL(bank,6,C24(0xf4f4f4)); PAL(bank,7,C24(pants));
}
void art_set_player_palette(int c) {
    const char_def_t *d = &g_chars[c];
    set_actor_pal(PAL_PLAYER, d->skin, d->body, d->bandana, d->hair, 0x22252e);
}
void art_update_player_palette(int c, u32 frame) {
    if (!g_chars[c].rainbow) return;
    static const u32 rb[6] = { 0xff3b3b, 0xff9f1c, 0xffe94a, 0x3ecf6b, 0x3b82f6, 0xb15cff };
    hw_pal_obj()[PAL_PLAYER * 16 + 4] = C24(rb[(frame >> 3) % 6]);
}
/* ---------- fond : tuiles BG ---------- */
enum { T_SKYA = 1, T_SKYB, T_BLD, T_WIN_OFF, T_WIN_ON, T_NEON, T_STREET_TOP, T_STREET, T_PANEL_FILL, T_PANEL_TOP, T_PANEL_BOT, T_PANEL_L, T_PANEL_R };
static void bg_solid(u16 *t, int tile, int c) { cv_clear(8, 8); cv_rect(0, 0, 7, 7, c); cv_blit(t, tile, 8, 8); }
static void build_bg_tiles(void) {
    u16 *t = hw_bg_tiles();
    cv_clear(8, 8); cv_blit(t, 0, 8, 8);
    bg_solid(t, T_SKYA, 1); bg_solid(t, T_SKYB, 2); bg_solid(t, T_BLD, 3);
    cv_clear(8, 8); cv_rect(0,0,7,7,3); cv_rect(2,2,5,5,5); cv_blit(t, T_WIN_OFF, 8, 8);
    cv_clear(8, 8); cv_rect(0,0,7,7,3); cv_rect(2,2,5,5,6); cv_blit(t, T_WIN_ON, 8, 8);
    cv_clear(8, 8); cv_rect(0,0,7,7,3); cv_rect(0,0,7,1,7); cv_blit(t, T_NEON, 8, 8);
    cv_clear(8, 8); cv_rect(0,0,7,7,8); cv_rect(0,0,7,1,9); cv_blit(t, T_STREET_TOP, 8, 8);
    cv_clear(8, 8); cv_rect(0,0,7,7,8); cv_rect(1,4,4,4,9); cv_blit(t, T_STREET, 8, 8);
    bg_solid(t, T_PANEL_FILL, 10);
    cv_clear(8, 8); cv_rect(0,0,7,7,10); cv_rect(0,0,7,1,11); cv_blit(t, T_PANEL_TOP, 8, 8);
    cv_clear(8, 8); cv_rect(0,0,7,7,10); cv_rect(0,6,7,7,11); cv_blit(t, T_PANEL_BOT, 8, 8);
    cv_clear(8, 8); cv_rect(0,0,7,7,10); cv_rect(0,0,1,7,11); cv_blit(t, T_PANEL_L, 8, 8);
    cv_clear(8, 8); cv_rect(0,0,7,7,10); cv_rect(6,0,7,7,11); cv_blit(t, T_PANEL_R, 8, 8);
}
static void build_arena_map(void) {                  /* ville de nuit : skyline pseudo-aleatoire fixe */
    u16 *m = hw_bg_map(SB_BG0);
    for (int y = 0; y < 32; y++) for (int x = 0; x < 32; x++) m[y * 32 + x] = (u16)(y < 9 ? T_SKYA : T_SKYB);
    int x = 0, seed = 7;
    while (x < 30) {
        seed = (seed * 75 + 74) & 0xFFFF;
        int bw = 2 + (seed & 1), bh = 4 + ((seed >> 3) % 6);
        for (int bx = x; bx < x + bw && bx < 30; bx++) for (int by = 18 - bh; by < 18; by++) {
            int t = T_BLD;
            if (by == 18 - bh) t = ((seed >> 5) & 1) ? T_NEON : T_BLD;
            else if (by > 18 - bh + 1 && ((bx + by + seed) & 1)) t = (((bx * 7 + by * 3 + seed) & 3) == 0) ? T_WIN_ON : T_WIN_OFF;
            m[by * 32 + bx] = (u16)t;
        }
        x += bw;
    }
    for (int i = 0; i < 30; i++) { m[18 * 32 + i] = T_STREET_TOP; m[19 * 32 + i] = T_STREET; }
}
void art_init(void) {
    u16 *pb = hw_pal_bg();
    static const u32 bgc[16] = { 0x14082b, 0x14082b, 0x2a0f4a, 0x1a1030, 0x261845, 0x3a2a5c, 0xffd23e, 0xff3ea5,
                                 0x2b2b3a, 0x5c5c78, 0x0c0818, 0xffd23e, 0xe5302f, 0x3a1020, 0xffffff, 0x36e0ff };
    for (int i = 0; i < 16; i++) pb[i] = C24(bgc[i]);
    static const u32 txt[5] = { 0xffffff, 0xffd23e, 0xff5a54, 0x3ecf6b, 0x36e0ff };
    for (int b = 0; b < 5; b++) { pb[(b + 1) * 16] = 0; pb[(b + 1) * 16 + 1] = C24(txt[b]); }
    build_bg_tiles();
    build_arena_map();
    u16 *v = hw_obj_tiles();
    for (int s = 0; s < 8; s++) for (int f = 0; f < 4; f++) { draw_human(&styles[s], f); cv_blit(v, TILE_HUMAN(s, f), 16, 32); }
    for (int f = 0; f < 2; f++) { draw_bat(f); cv_blit(v, TILE_BAT(f), 16, 16); draw_boss(f); cv_blit(v, TILE_BOSS(f), 32, 32); }
    small_sprites(v); draw_door(); cv_blit(v, TILE_DOOR, 16, 32);
    art_set_player_palette(CH_RYDER);
    set_actor_pal(PAL_GRUNT,   0xc8a07a, 0x4b4f58, 0x8b3fd8, 0x1a1a22, 0x2d2f38);
    set_actor_pal(PAL_RUSHER,  0xb98a66, 0x7a3a64, 0x8b3fd8, 0x241028, 0x3a2a40);
    set_actor_pal(PAL_THROWER, 0xd2b48c, 0x3f4658, 0x8b3fd8, 0x1a1a22, 0x262b38);
    u16 *po = hw_pal_obj();
    PAL(PAL_BAT,0,0); PAL(PAL_BAT,1,C24(0x0a0612)); PAL(PAL_BAT,3,C24(0x4a1f8a)); PAL(PAL_BAT,4,C24(0x6b2fb8)); PAL(PAL_BAT,6,C24(0xff4d6d));
    set_actor_pal(PAL_BOSS, 0xc89870, 0x5b2a9a, 0x8b3fd8, 0x1a1020, 0x2a1448);
    PAL(PAL_MISC,0,0); PAL(PAL_MISC,1,C24(0xffe94a)); PAL(PAL_MISC,2,C24(0xff3b3b)); PAL(PAL_MISC,3,C24(0xffc21a)); PAL(PAL_MISC,4,C24(0xb8860b));
    PAL(PAL_MISC,5,C24(0xffffff)); PAL(PAL_MISC,6,C24(0x6b3f1d)); PAL(PAL_MISC,7,C24(0x3ecf6b)); PAL(PAL_MISC,8,C24(0x36e0ff)); PAL(PAL_MISC,9,C24(0xff9f1c));
    for (int i = 1; i < 16; i++) PAL(PAL_FLASH, i, C24(0xffffff));
    PAL(PAL_FLASH, 1, C24(0x2a2a2a));
}

/* ===== ui.c ===== */
/* Police 3x5 agrandie x2 (6x10 dans une cellule de 8 px) : un caractere = 2 tuiles superposees.
   Chaque rangee du glyphe = 1 chiffre octal (3 bits). */
static const char GLYPHS[] = " 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ+-%/:!>.=<";
static const u16 GDATA[] = {
    00000, 075557, 026227, 071747, 071717, 055711, 074717, 074757, 071122, 075757, 075717,
    025755, 065656, 034443, 065556, 074647, 074644, 034553, 055755, 072227, 011152, 055655, 044447,
    057755, 065555, 025552, 065644, 025563, 065655, 034216, 072222, 055557, 055552, 055777, 055255, 055222, 071247,
    002720, 000700, 051245, 011244, 002020, 022202, 042124, 000002, 007070, 012421 };
#define FONT_BASE 16
#define BAR_BASE 112
#define TILE_PNL_FILL 9
static int glyph_of(char c) {
    if (c >= 'a' && c <= 'z') c = (char)(c - 32);
    for (int i = 0; GLYPHS[i]; i++) if (GLYPHS[i] == c) return i;
    return 0;
}
static void put_tile(u16 *t, int tile, const u8 px[8][8]) {
    for (int y = 0; y < 8; y++) for (int k = 0; k < 2; k++) {
        u16 v = 0; for (int i = 0; i < 4; i++) v |= (u16)((px[y][k*4+i] & 15) << (i*4));
        t[tile * 16 + y * 2 + k] = v;
    }
}
void ui_init(void) {
    u16 *t = hw_bg_tiles();
    for (int g = 0; g < ARRAY_LEN(GDATA); g++) {
        u8 top[8][8] = {{0}}, bot[8][8] = {{0}};
        for (int gy = 0; gy < 5; gy++) for (int gx = 0; gx < 3; gx++) {
            if (!((GDATA[g] >> ((4 - gy) * 3)) & 7 & (4 >> gx))) continue;
            for (int dy = 0; dy < 2; dy++) for (int dx = 0; dx < 2; dx++) {
                int y = gy * 2 + dy, x = 1 + gx * 2 + dx;
                if (y < 8) top[y][x] = 1; else bot[y - 8][x] = 1;
            }
        }
        put_tile(t, FONT_BASE + g * 2, top); put_tile(t, FONT_BASE + g * 2 + 1, bot);
    }
    for (int n = 0; n <= 8; n++) {                        /* barre de PV : n pixels remplis sur 8 */
        u8 px[8][8] = {{0}};
        for (int y = 1; y < 7; y++) for (int x = 0; x < 8; x++) px[y][x] = (u8)(x < n ? 12 : 13);
        put_tile(t, BAR_BASE + n, px);
    }
    ui_clear(UI_TXT); ui_clear(UI_PNL);
}
static u16 *layer_map(int l) { return hw_bg_map(l == UI_TXT ? SB_BG1 : SB_BG2); }
void ui_clear(int l) { u16 *m = layer_map(l); for (int i = 0; i < 1024; i++) m[i] = 0; }
void ui_clear_rows(int l, int row, int n) { u16 *m = layer_map(l); for (int y = row; y < row + n && y < 32; y++) for (int x = 0; x < 32; x++) m[y * 32 + x] = 0; }
void ui_text(int l, int col, int row, const char *s, int pal) {
    u16 *m = layer_map(l);
    for (; *s && col < 30; s++, col++) {
        int g = glyph_of(*s);
        if (g == 0) { m[row * 32 + col] = 0; m[(row + 1) * 32 + col] = 0; continue; }
        m[row * 32 + col]       = (u16)((FONT_BASE + g * 2)     | (pal << 12));
        m[(row + 1) * 32 + col] = (u16)((FONT_BASE + g * 2 + 1) | (pal << 12));
    }
}
void ui_text_center(int l, int row, const char *s, int pal) {
    int n = 0; while (s[n]) n++;
    ui_text(l, MAX(0, (30 - n) / 2), row, s, pal);
}
void ui_num(int l, int col, int row, int v, int width, int pal) {
    char b[12]; int n = 0;
    if (v < 0) v = 0;
    do { b[n++] = (char)('0' + v % 10); v /= 10; } while (v && n < 10);
    char o[12]; int k = 0;
    for (int i = n; i < width; i++) o[k++] = ' ';
    while (n) o[k++] = b[--n];
    o[k] = 0;
    ui_text(l, col, row, o, pal);
}
void ui_bar(int col, int row, int cur, int max, int cells) {
    u16 *m = hw_bg_map(SB_BG1);
    int total = cells * 8, fill = max > 0 ? (cur * total) / max : 0;
    if (cur > 0 && fill == 0) fill = 1;
    fill = CLAMP(fill, 0, total);
    for (int i = 0; i < cells; i++) m[row * 32 + col + i] = (u16)(BAR_BASE + CLAMP(fill - i * 8, 0, 8));
}
void ui_panel(int col, int row, int w, int h) {
    u16 *m = hw_bg_map(SB_BG2);
    for (int y = row; y < row + h; y++) for (int x = col; x < col + w; x++) {
        int t = TILE_PNL_FILL;
        if (y == row) t = TILE_PNL_FILL + 1; else if (y == row + h - 1) t = TILE_PNL_FILL + 2;
        else if (x == col) t = TILE_PNL_FILL + 3; else if (x == col + w - 1) t = TILE_PNL_FILL + 4;
        m[y * 32 + x] = (u16)t;
    }
}

/* ===== player.c ===== */
player_t g_player;
void (*g_hook_player_down)(void);
const s16 g_dirv[8][2] = { {256,0},{181,181},{0,256},{-181,181},{-256,0},{-181,-181},{0,-256},{181,-181} };   /* E SE S SO O NO N NE */
static const s8 AIM_TAB[3][3] = { {5,6,7}, {4,-1,0}, {3,2,1} };
#define P_W 10
#define P_H 28
#undef GRAVITY
#define GRAVITY 75
#define JUMP_V (-1638)
#define BASE_SPEED 384
void player_init(int chr) {
    player_t *p = &g_player; u8 *z = (u8 *)p;
    for (u32 i = 0; i < sizeof(*p); i++) z[i] = 0;
    p->chr = (u8)chr;
    p->b.w = P_W; p->b.h = P_H; p->b.face = 1;
    p->b.x = FX(SCREEN_W / 2 - P_W / 2); p->b.y = FX(GROUND_Y - P_H);
    p->maxhp = p->hp = (s16)((100 * g_chars[chr].hp) >> 8);
    p->weapon = g_save.equipped < W_COUNT ? g_save.equipped : W_PISTOL;
    art_set_player_palette(chr);
}
int player_center_x(void) { return FX_INT(g_player.b.x) + P_W / 2; }
int player_center_y(void) { return FX_INT(g_player.b.y) + P_H / 2; }
void player_set_weapon(int w) { if (w >= 0 && w < W_COUNT) g_player.weapon = (u8)w; }
void player_heal(int a) { g_player.hp = (s16)MIN(g_player.maxhp, g_player.hp + a); }
void player_hurt(int dmg, int src_x) {
    player_t *p = &g_player;
    if (p->dead || g_chars[p->chr].invulnerable || p->invuln) return;
    dmg = MAX(1, (int)(((u32)dmg * g_bonus.defense + 128) >> 8));
    p->hp = (s16)(p->hp - dmg);
    p->invuln = 33; p->hurt_t = 18;
    sfx_play(SFX_HURT); cam_shake(6, 1);
    body_push(&p->b, player_center_x() < src_x ? -6 : 6);
    if (p->hp <= 0) { p->hp = 0; p->dead = 1; p->dead_t = 0; if (g_hook_player_down) g_hook_player_down(); }
}
void player_update(void) {
    player_t *p = &g_player; const char_def_t *c = &g_chars[p->chr];
    p->anim++;
    if (p->invuln) p->invuln--;
    if (p->hurt_t) p->hurt_t--;
    if (p->cd) p->cd--;
    art_update_player_palette(p->chr, p->anim);
    if (p->dead) { p->dead_t++; p->b.vx = 0; p->b.vy = MIN(p->b.vy + GRAVITY, 1792); body_move(&p->b); return; }
    u16 h = g_in.held;
    int dx = ((h & KEY_RIGHT) ? 1 : 0) - ((h & KEY_LEFT) ? 1 : 0);
    int dy = ((h & KEY_DOWN) ? 1 : 0) - ((h & KEY_UP) ? 1 : 0);
    int aiming = (h & KEY_R) != 0;
    if (g_in.pressed & KEY_R) p->aim = p->b.face > 0 ? 0 : 4;
    if (aiming) {                                   /* R + D-Pad = visee 8 directions, independante du deplacement */
        p->aim_free = 1;
        if (dx || dy) p->aim = (u8)AIM_TAB[dy + 1][dx + 1];
        if (g_dirv[p->aim][0] > 0) p->b.face = 1; else if (g_dirv[p->aim][0] < 0) p->b.face = -1;
    } else {
        p->aim_free = 0;
        if (dx) p->b.face = (s8)dx;
        p->aim = p->b.face > 0 ? 0 : 4;
    }
    int spd = (int)((((u32)BASE_SPEED * c->speed >> 8) * g_bonus.speed) >> 8);
    if (!aiming) p->b.vx = dx * spd;
    else if (p->b.ground) p->b.vx = 0;              /* au sol : on s'arrete pour viser ; en l'air : l'elan est conserve */
    if ((g_in.pressed & KEY_B) && p->b.ground) { p->b.vy = (JUMP_V * (int)c->jump) >> 8; sfx_play(SFX_JUMP); }
    p->b.vy = MIN(p->b.vy + GRAVITY, 1792);
    body_move(&p->b);
    if ((h & KEY_A) && p->cd == 0) weapon_fire(p);
}
void player_render(void) {
    player_t *p = &g_player; int x = FX_INT(p->b.x), y = FX_INT(p->b.y);
    if (p->invuln && !p->dead && (p->invuln & 3) < 2) return;      /* clignotement d'invincibilite */
    int frame = !p->b.ground ? 3 : (p->b.vx != 0 ? 1 + ((p->anim >> 3) & 1) : 0);
    int pal = p->hurt_t > 14 ? PAL_FLASH : PAL_PLAYER;
    spr_world(x - 3, y - 4, SPR_16x32, TILE_HUMAN(g_chars[p->chr].art, frame), pal, p->b.face < 0 ? SPR_HFLIP : 0);
    if (p->aim_free && !p->dead) {
        int mx = player_center_x() + (g_dirv[p->aim][0] * 20 >> 8), my = y + 13 + (g_dirv[p->aim][1] * 20 >> 8);
        spr_world(mx - 4, my - 4, SPR_8, TILE_CROSS, PAL_MISC, 0);
    }
}

/* ===== weapons.c ===== */
extern const s16 g_dirv[8][2];
void weapon_fire(player_t *p) {
    const weapon_def_t *w = &g_weapons[p->weapon]; const char_def_t *c = &g_chars[p->chr];
    int dmg = (int)(((((u32)w->dmg * 256 * c->dmg) >> 8) * g_bonus.dmg >> 8) + 128) >> 8;
    if (dmg < 1) dmg = 1;
    p->cd = (u8)MAX(4, (int)(((u32)w->cd * g_bonus.fire) >> 8));
    const s16 *d = g_dirv[p->aim];
    if (p->aim_free) { if (d[0] > 51) p->b.face = 1; else if (d[0] < -51) p->b.face = -1; }
    int px = FX_INT(p->b.x), py = FX_INT(p->b.y);
    if (w->melee) {                                         /* gant : zone devant le joueur, la visee haute etend vers le haut */
        int up = (d[1] < 0) ? 14 : 0, reach = w->range_px;
        int rx = p->b.face > 0 ? px + p->b.w : px - reach, ry = py - up, rh = p->b.h + up;
        for (int i = 0; i < ENEMY_MAX; i++) {
            enemy_t *e = &g_enemies[i];
            if (e->active && !e->dead && rect_hit(rx, ry, reach, rh, FX_INT(e->b.x), FX_INT(e->b.y), e->b.w, e->b.h))
                enemy_damage(e, dmg, p->b.face, c->lethal);
        }
        if (g_boss.active && !g_boss.dead && rect_hit(rx, ry, reach, rh, FX_INT(g_boss.b.x), FX_INT(g_boss.b.y), g_boss.b.w, g_boss.b.h))
            boss_damage(dmg, c->lethal);
        fxp_spawn(rx + reach / 2, py + 12, 6); sfx_play(SFX_HEAVY);
        return;
    }
    int mx = px + p->b.w / 2 + (d[0] * 18 >> 8), my = py + 13 + (d[1] * 18 >> 8);
    for (int i = 0; i < w->pellets; i++) {
        proj_t *q = proj_alloc(); if (!q) break;
        int vperp = 0;
        if (w->pellets > 1) vperp = ((w->speed * w->spread >> 8) * (2 * i - (w->pellets - 1))) / 2;
        else if (w->jitter) { int j = w->speed * w->jitter >> 8; vperp = rng_range(2 * j + 1) - j; }
        q->vx = ((d[0] * w->speed) >> 8) + ((-d[1] * vperp) >> 8);
        q->vy = ((d[1] * w->speed) >> 8) + ((d[0] * vperp) >> 8);
        q->w = w->pw; q->h = w->ph;
        q->x = FX(mx - q->w / 2); q->y = FX(my - q->h / 2);
        q->range = FX(w->range_px); q->friendly = 1; q->pierce = w->pierce; q->lethal = c->lethal; q->dmg = (s16)dmg; q->hitmask = 0;
    }
    fxp_spawn(mx, my, 3);
    sfx_play((p->weapon == W_SHOTGUN || p->weapon == W_RAILGUN) ? SFX_HEAVY : SFX_SHOOT);
}

/* ===== enemy.c ===== */
enemy_t g_enemies[ENEMY_MAX];
void (*g_hook_enemy_down)(void);
#undef GRAVITY
#define GRAVITY 75
void enemies_clear(void) { u8 *z = (u8 *)g_enemies; for (u32 i = 0; i < sizeof(g_enemies); i++) z[i] = 0; }
int enemies_alive(void) { int n = 0; for (int i = 0; i < ENEMY_MAX; i++) if (g_enemies[i].active && !g_enemies[i].dead) n++; return n; }
static int bats_alive(void) { int n = 0; for (int i = 0; i < ENEMY_MAX; i++) if (g_enemies[i].active && !g_enemies[i].dead && g_enemies[i].type == E_BAT) n++; return n; }
static int pick_type(void) {                                     /* 42 % voyou / 26 % fonceur / 16 % lanceur / 16 % chauve-souris */
    int r = rng_range(100);
    if (r < 42) return E_GRUNT;
    if (r < 68) return E_RUSHER;
    if (r < 84) return E_THROWER;
    if (g_run.score >= 10 && bats_alive() < BATS_ALIVE_MAX) return E_BAT;
    return E_GRUNT;
}
int enemies_spawn_random(void) {
    if (enemies_alive() >= ENEMY_ALIVE_MAX) return 0;
    enemy_t *e = 0;
    for (int i = 0; i < ENEMY_MAX; i++) if (!g_enemies[i].active) { e = &g_enemies[i]; break; }
    if (!e) return 0;
    int t = pick_type(); const enemy_def_t *d = &g_enemy_defs[t];
    u8 *z = (u8 *)e; for (u32 i = 0; i < sizeof(*e); i++) z[i] = 0;
    e->active = 1; e->type = (u8)t; e->b.w = d->w; e->b.h = d->h;
    e->hp = e->maxhp = (s16)(d->hp + (g_run.score / 120) * 2);        /* progression de PV du jeu d'origine */
    int left = rng_range(2);
    e->b.x = FX(left ? 6 : SCREEN_W - 6 - d->w); e->b.face = left ? 1 : -1;
    if (d->flying) {
        e->hover_y = (s16)(30 + rng_range(40)); e->b.y = FX(MAX(8, e->hover_y - 20));
        e->bphase = (u8)rng_range(256); e->atk_cd = 90;               /* rode avant sa premiere attaque */
    } else e->b.y = FX(GROUND_Y - d->h);
    return 1;
}
void enemy_damage(enemy_t *e, int dmg, int kdir, int lethal) {
    if (!e->active || e->dead) return;
    if (lethal && dmg < e->hp) dmg = e->hp;
    e->hp = (s16)(e->hp - dmg); e->flash = 5;
    int cx = FX_INT(e->b.x) + e->b.w / 2, cy = FX_INT(e->b.y) + e->b.h / 2;
    fxp_spawn(cx, cy, 6);
    if (kdir) body_push(&e->b, kdir * 16);
    if (e->hp > 0) { sfx_play(SFX_HIT); return; }
    e->dead = 1; e->dead_t = 24; e->b.vx = 0; e->b.vy = g_enemy_defs[e->type].flying ? 100 : 0;
    g_run.score += g_enemy_defs[e->type].points;
    coin_spawn(cx, cy, 1);
    sfx_play(SFX_DEATH);
    if (g_hook_enemy_down) g_hook_enemy_down();
}
static void steer(enemy_t *e, int tx, int ty, int sp, int shift) {
    int cx = FX_INT(e->b.x) + e->b.w / 2, cy = FX_INT(e->b.y) + e->b.h / 2;
    int dx = tx - cx, dy = ty - cy, ax = ABS(dx), ay = ABS(dy);
    int d = MAX(ax, ay) + MIN(ax, ay) / 2; if (d == 0) d = 1;       /* norme approchee, sans racine */
    e->b.vx += (dx * sp / d - e->b.vx) >> shift;
    e->b.vy += (dy * sp / d - e->b.vy) >> shift;
}
static void bat_update(enemy_t *e) {
    const enemy_def_t *d = &g_enemy_defs[E_BAT];
    int pcx = player_center_x(), pcy = FX_INT(g_player.b.y) + 11;
    int cx = FX_INT(e->b.x) + e->b.w / 2, cy = FX_INT(e->b.y) + e->b.h / 2;
    e->phase16 = (u16)(e->phase16 + 190);
    if (e->btimer) e->btimer--;
    switch (e->bstate) {
    case 0: {                                                        /* rode au-dessus du joueur */
        int tx = pcx + (isin((e->phase16 >> 8) + e->bphase) * 72 >> 8);
        int ty = e->hover_y + (isin(((e->phase16 >> 8) * 5 >> 1)) * 10 >> 8);
        steer(e, tx, ty, d->speed, 4);
        if (e->atk_cd == 0 && ABS(pcx - cx) < 114 && !g_player.dead) { e->bstate = 1; e->btimer = 17; e->atk_anim = 17; }
    } break;
    case 1:                                                          /* se fige, ailes battantes */
        e->b.vx -= e->b.vx >> 3; e->b.vy -= e->b.vy >> 3;
        if (e->btimer == 0) { e->bstate = 2; e->btimer = 48; e->dive_x = (s16)pcx; e->dive_y = (s16)pcy; }
        break;
    case 2: {                                                        /* pique sur la position visee : esquivable */
        steer(e, e->dive_x, e->dive_y, d->speed * 26 / 10, 3);
        int ax = ABS(e->dive_x - cx), ay = ABS(e->dive_y - cy);
        if (MAX(ax, ay) < 10 || e->btimer == 0) { e->bstate = 3; e->btimer = 42; e->atk_cd = d->atk_cd; }
    } break;
    default: {                                                       /* remonte et s'ecarte */
        int tx = CLAMP(cx + (cx < pcx ? -72 : 72), 14, SCREEN_W - 14);
        steer(e, tx, e->hover_y - 24, d->speed * 125 / 100, 3);
        if (e->btimer == 0) e->bstate = 0;
    } break;
    }
    e->b.x += e->b.vx; e->b.y += e->b.vy;
    e->b.x = CLAMP(e->b.x, 0, FX(SCREEN_W - e->b.w));
    e->b.y = CLAMP(e->b.y, FX(6), FX(GROUND_Y - e->b.h - 4));
    if (e->b.vx > 4096 / 256 * 4) e->b.face = 1; else if (e->b.vx < -4096 / 256 * 4) e->b.face = -1;
    if (e->hit_cd) e->hit_cd--;
    if (!g_player.dead && e->hit_cd == 0 &&
        rect_hit(FX_INT(e->b.x), FX_INT(e->b.y), e->b.w, e->b.h, FX_INT(g_player.b.x), FX_INT(g_player.b.y), g_player.b.w, g_player.b.h)) {
        e->hit_cd = 42; player_hurt(d->dmg, cx);
        if (e->bstate == 2) { e->bstate = 3; e->btimer = 42; e->atk_cd = d->atk_cd; }
    }
}
void enemies_update(void) {
    for (int i = 0; i < ENEMY_MAX; i++) {
        enemy_t *e = &g_enemies[i]; if (!e->active) continue;
        const enemy_def_t *d = &g_enemy_defs[e->type];
        e->anim++; if (e->flash) e->flash--; if (e->atk_anim) e->atk_anim--; if (e->atk_cd) e->atk_cd--;
        if (e->dead) {
            if (d->flying) {                                          /* cadavre de chauve-souris : chute */
                e->b.vy = MIN(e->b.vy + GRAVITY, 1792); e->b.y += e->b.vy;
                if (e->b.y > FX(GROUND_Y - e->b.h)) e->b.y = FX(GROUND_Y - e->b.h);
            }
            if (--e->dead_t == 0) e->active = 0;
            continue;
        }
        if (d->flying) { bat_update(e); continue; }
        int ecx = FX_INT(e->b.x) + e->b.w / 2, cdx = player_center_x() - ecx, dist = ABS(cdx);
        int dir = cdx >= 0 ? 1 : -1;
        e->b.face = (s8)dir; e->b.vx = 0;
        if (d->ranged) {
            if (dist > d->atk_range * 6 / 10) e->b.vx = dir * d->speed;
            if (dist <= d->atk_range && e->atk_cd == 0 && !g_player.dead) { e->atk_cd = d->atk_cd; e->atk_anim = 20; enemy_throw(e, dir); }
        } else if (dist > d->atk_range) {
            int spd = d->speed;
            if (e->type == E_RUSHER && dist < 114) spd = spd * 155 / 100;   /* le fonceur accelere a l'approche */
            e->b.vx = dir * spd;
        } else if (e->atk_cd == 0) {
            e->atk_cd = d->atk_cd; e->atk_anim = 20;
            if (FX_INT(g_player.b.y) + g_player.b.h > FX_INT(e->b.y) + 4) player_hurt(d->dmg, ecx);   /* sauter par-dessus esquive */
        }
        e->b.vy = MIN(e->b.vy + GRAVITY, 1792);
        body_move(&e->b);
        for (int j = 0; j < ENEMY_MAX; j++) {                          /* legere separation entre ennemis au sol */
            enemy_t *o = &g_enemies[j];
            if (j == i || !o->active || o->dead || g_enemy_defs[o->type].flying) continue;
            int dx = FX_INT(e->b.x) - FX_INT(o->b.x);
            if (ABS(dx) < e->b.w * 7 / 10) e->b.x += (dx > 0 || (dx == 0 && i > j)) ? 26 : -26;
        }
        e->b.x = CLAMP(e->b.x, 0, FX(SCREEN_W - e->b.w));
    }
}
void enemies_render(void) {
    for (int i = 0; i < ENEMY_MAX; i++) {
        enemy_t *e = &g_enemies[i]; if (!e->active) continue;
        if (e->dead && (e->dead_t & 2)) continue;                     /* clignote puis disparait */
        const enemy_def_t *d = &g_enemy_defs[e->type];
        int x = FX_INT(e->b.x), y = FX_INT(e->b.y), pal = e->flash ? PAL_FLASH : d->pal, fl = e->b.face < 0 ? SPR_HFLIP : 0;
        if (d->flying) {
            spr_world(x - 1, y - 3, SPR_16, TILE_BAT(e->atk_anim ? 0 : ((e->anim >> 2) & 1)), pal, fl);
        } else {
            int frame = (e->dead || e->atk_anim) ? 3 : (e->b.vx ? 1 + ((e->anim >> 3) & 1) : 0);
            spr_world(x - 3, y + e->b.h - 32, SPR_16x32, TILE_HUMAN(d->art, frame), pal, fl);
        }
    }
}

/* ===== boss.c ===== */
boss_t g_boss;
void (*g_hook_boss_down)(void);
#undef GRAVITY
#define GRAVITY 75
#define BOSS_W 24
#define BOSS_H 30
enum { BS_CHASE, BS_TELEGRAPH, BS_SLAM };
void boss_spawn(int score) {
    boss_t *b = &g_boss; u8 *z = (u8 *)b; for (u32 i = 0; i < sizeof(*b); i++) z[i] = 0;
    int tier = MAX(1, score / 200);
    b->hp = b->maxhp = (s16)(260 + tier * 40);                       /* PV du jeu d'origine */
    b->active = 1; b->b.w = BOSS_W; b->b.h = BOSS_H;
    b->b.x = FX(g_player.b.face == 1 ? SCREEN_W - 46 : 22); b->b.y = FX(GROUND_Y - BOSS_H); b->b.face = -1;
    b->slam_cd = 255;
    sfx_play(SFX_BOSS);
}
void boss_damage(int dmg, int lethal) {
    boss_t *b = &g_boss; if (!b->active || b->dead) return;
    if (lethal && dmg < b->hp) dmg = b->hp;
    b->hp = (s16)(b->hp - dmg); b->flash = 5;
    fxp_spawn(FX_INT(b->b.x) + BOSS_W / 2, FX_INT(b->b.y) + 10, 6);
    if (b->hp > 0) { sfx_play(SFX_HIT); return; }
    b->dead = 1; b->dead_t = 45; b->hp = 0;
    g_run.score += 150;
    coin_spawn(FX_INT(b->b.x) + BOSS_W / 2, FX_INT(b->b.y) + 8, 100);
    cam_shake(14, 3); sfx_play(SFX_DEATH);
    if (g_hook_boss_down) g_hook_boss_down();
}
void boss_update(void) {
    boss_t *b = &g_boss; if (!b->active) return;
    b->anim++; if (b->flash) b->flash--; if (b->melee_cd) b->melee_cd--;
    if (b->dead) { if (--b->dead_t == 0) b->active = 0; return; }
    int bx = FX_INT(b->b.x), bcx = bx + BOSS_W / 2, cdx = player_center_x() - bcx, dist = ABS(cdx), dir = cdx >= 0 ? 1 : -1;
    b->b.face = (s8)dir; b->b.vx = 0;
    switch (b->state) {
    case BS_CHASE:
        if (dist > 35) b->b.vx = dir * 169;
        else if (b->melee_cd == 0) {
            b->melee_cd = 54; b->timer = 0;
            if (FX_INT(g_player.b.y) + g_player.b.h > FX_INT(b->b.y) + 4) player_hurt(16, bcx);
        }
        if (b->slam_cd) b->slam_cd--;
        if (b->slam_cd == 0) { b->state = BS_TELEGRAPH; b->timer = 42; }
        break;
    case BS_TELEGRAPH:                                                   /* signal visuel avant le coup */
        if ((b->timer & 7) == 0) fxp_spawn(bcx + rng_range(30) - 15, GROUND_Y - 6, 8);
        if (--b->timer == 0) {
            b->state = BS_SLAM; b->timer = 9;
            if (g_player.b.ground && dist < 90) player_hurt(22, bcx);
            cam_shake(10, 2); sfx_play(SFX_BOSS);
            for (int k = -2; k <= 2; k++) fxp_spawn(bcx + k * 30, GROUND_Y - 6, 12);
        }
        break;
    default:
        if (--b->timer == 0) { b->state = BS_CHASE; b->slam_cd = 255; }
        break;
    }
    b->b.vy = MIN(b->b.vy + GRAVITY, 1792);
    body_move(&b->b);
    b->b.x = CLAMP(b->b.x, 0, FX(SCREEN_W - BOSS_W));
}
void boss_render(void) {
    boss_t *b = &g_boss; if (!b->active) return;
    if (b->dead && (b->dead_t & 2)) return;
    int pal = (b->flash || (b->state == BS_TELEGRAPH && (b->anim & 4))) ? PAL_FLASH : PAL_BOSS;
    int frame = (b->state != BS_CHASE) ? 1 : 0;
    spr_world(FX_INT(b->b.x) - 4, FX_INT(b->b.y) - 2, SPR_32, TILE_BOSS(frame), pal, b->b.face < 0 ? SPR_HFLIP : 0);
}

/* ===== proj.c ===== */
proj_t g_proj[PROJ_MAX];
coin_t g_coins[COIN_MAX];
static fxp_t g_fxp[FXP_MAX];
void proj_clear(void) { u8 *z = (u8 *)g_proj; for (u32 i = 0; i < sizeof(g_proj); i++) z[i] = 0; }
void proj_clear_hostile(void) { for (int i = 0; i < PROJ_MAX; i++) if (!g_proj[i].friendly) g_proj[i].active = 0; }
proj_t *proj_alloc(void) {
    for (int i = 0; i < PROJ_MAX; i++) if (!g_proj[i].active) { u8 *z = (u8 *)&g_proj[i]; for (u32 k = 0; k < sizeof(proj_t); k++) z[k] = 0; g_proj[i].active = 1; return &g_proj[i]; }
    return 0;
}
void enemy_throw(const enemy_t *e, int dir) {
    proj_t *q = proj_alloc(); if (!q) return;
    q->w = 6; q->h = 4; q->vx = dir * 563; q->range = FX(400); q->friendly = 0;
    q->x = FX(dir > 0 ? FX_INT(e->b.x) + e->b.w : FX_INT(e->b.x) - 6); q->y = FX(FX_INT(e->b.y) + e->b.h * 4 / 10);
    q->dmg = g_enemy_defs[e->type].dmg;
}
static int target_dist(int ox, int tx, int tw) { return ABS(ox - (tx + tw / 2)); }
void proj_update(void) {
    for (int i = 0; i < PROJ_MAX; i++) {
        proj_t *p = &g_proj[i]; if (!p->active) continue;
        fx ox = p->x, oy = p->y;
        p->x += p->vx; p->y += p->vy;
        p->range -= MAX(ABS(p->vx), ABS(p->vy)) + MIN(ABS(p->vx), ABS(p->vy)) / 2;
        int cx = FX_INT(p->x) + p->w / 2, cy = FX_INT(p->y) + p->h / 2;
        if (p->range <= 0 || cx < -8 || cx > SCREEN_W + 8 || cy < -8 || cy > SCREEN_H || level_solid(cx >> 3, cy >> 3)) { p->active = 0; continue; }
        if (!p->friendly) {
            if (!g_player.invuln && !g_player.dead &&
                rect_hit(FX_INT(p->x), FX_INT(p->y), p->w, p->h, FX_INT(g_player.b.x), FX_INT(g_player.b.y), g_player.b.w, g_player.b.h)) {
                player_hurt(p->dmg, FX_INT(p->x)); p->active = 0;
            }
            continue;
        }
        int sx = FX_INT(MIN(ox, p->x)), sy = FX_INT(MIN(oy, p->y));                    /* test balaye : pas de traversee a haute vitesse */
        int sw = ABS(FX_INT(p->x) - FX_INT(ox)) + p->w, sh = ABS(FX_INT(p->y) - FX_INT(oy)) + p->h;
        int ox_px = FX_INT(ox);
        for (;;) {
            int best = -1, bd = 1 << 30;
            for (int k = 0; k < ENEMY_MAX; k++) {
                enemy_t *e = &g_enemies[k];
                if (!e->active || e->dead || (p->hitmask & (1 << k))) continue;
                if (!rect_hit(sx, sy, sw, sh, FX_INT(e->b.x), FX_INT(e->b.y), e->b.w, e->b.h)) continue;
                int d = target_dist(ox_px, FX_INT(e->b.x), e->b.w); if (d < bd) { bd = d; best = k; }
            }
            if (g_boss.active && !g_boss.dead && !(p->hitmask & 0x80) &&
                rect_hit(sx, sy, sw, sh, FX_INT(g_boss.b.x), FX_INT(g_boss.b.y), g_boss.b.w, g_boss.b.h)) {
                int d = target_dist(ox_px, FX_INT(g_boss.b.x), g_boss.b.w); if (d < bd) { bd = d; best = 7; }
            }
            if (best < 0) break;
            p->hitmask |= (u8)(1 << best);
            if (best == 7) boss_damage(p->dmg, p->lethal); else enemy_damage(&g_enemies[best], p->dmg, 0, p->lethal);
            if (!p->pierce) { p->active = 0; break; }
        }
    }
}
void proj_render(void) {
    for (int i = 0; i < PROJ_MAX; i++) {
        proj_t *p = &g_proj[i]; if (!p->active) continue;
        int cx = FX_INT(p->x) + p->w / 2, cy = FX_INT(p->y) + p->h / 2;
        spr_world(cx - 4, cy - 4, SPR_8, !p->friendly ? TILE_EBULLET : (p->pierce ? TILE_BIGDOT : TILE_BULLET), PAL_MISC, 0);
    }
}
/* ------------------------------ pieces ------------------------------ */
void coins_clear(void) { u8 *z = (u8 *)g_coins; for (u32 i = 0; i < sizeof(g_coins); i++) z[i] = 0; }
static void coin_collect(int value) {
    int v = (int)(((u32)value * g_bonus.coins + 128) >> 8); if (v < 1) v = 1;
    g_run.coins += v; g_save.coins += (u32)v; sfx_play(SFX_COIN);
}
void coin_spawn(int x, int y, int value) {
    for (int i = 0; i < COIN_MAX; i++) if (!g_coins[i].active) {
        g_coins[i].active = 1; g_coins[i].value = (u8)value; g_coins[i].x = FX(x - 4); g_coins[i].y = FX(y - 8); g_coins[i].vy = -666; g_coins[i].t = 0;
        return;
    }
    coin_collect(value);                                     /* pool plein : credite directement */
}
void coins_update(void) {
    for (int i = 0; i < COIN_MAX; i++) {
        coin_t *c = &g_coins[i]; if (!c->active) continue;
        c->t++; c->vy += 38; c->y += c->vy;
        if (c->y > FX(GROUND_Y - 8)) { c->y = FX(GROUND_Y - 8); c->vy = 0; }
        if (!g_player.dead && rect_hit(FX_INT(c->x), FX_INT(c->y), 8, 8, FX_INT(g_player.b.x), FX_INT(g_player.b.y), g_player.b.w, g_player.b.h)) {
            c->active = 0; coin_collect(c->value);
        }
    }
}
void coins_render(void) {
    for (int i = 0; i < COIN_MAX; i++) if (g_coins[i].active) spr_world(FX_INT(g_coins[i].x), FX_INT(g_coins[i].y), SPR_8, TILE_COIN, PAL_MISC, 0);
}
/* ------------------------------ etincelles ------------------------------ */
void fxp_clear(void) { for (int i = 0; i < FXP_MAX; i++) g_fxp[i].life = 0; }
void fxp_spawn(int x, int y, int life) {
    int slot = 0, minl = 255;
    for (int i = 0; i < FXP_MAX; i++) { if (g_fxp[i].life == 0) { slot = i; break; } if (g_fxp[i].life < minl) { minl = g_fxp[i].life; slot = i; } }
    g_fxp[slot].x = (s16)x; g_fxp[slot].y = (s16)y; g_fxp[slot].life = (u8)life;
}
void fxp_update(void) { for (int i = 0; i < FXP_MAX; i++) if (g_fxp[i].life) g_fxp[i].life--; }
void fxp_render(void) { for (int i = 0; i < FXP_MAX; i++) if (g_fxp[i].life) spr_world(g_fxp[i].x - 4, g_fxp[i].y - 4, SPR_8, TILE_SPARK, PAL_MISC, 0); }

/* ===== hud.c ===== */
/* HUD sur BG1 (materiel) : mise a jour uniquement quand une valeur change -> cout quasi nul par frame. */
static int c_hp, c_max, c_wave, c_kills, c_need, c_boss, c_coins, c_bhp, c_bmax;
void hud_reset(void) {
    c_hp = c_max = c_wave = c_kills = c_need = c_boss = c_coins = c_bhp = c_bmax = -1;
    ui_clear_rows(UI_TXT, 0, 4);
    ui_text(UI_TXT, 0, 0, "HP", TXT_WHITE);
}
void hud_update(int wave, int kills, int need, int is_boss) {
    int hp = g_player.hp, mx = g_player.maxhp;
    if (hp != c_hp || mx != c_max) {
        c_hp = hp; c_max = mx;
        ui_bar(2, 0, hp, mx, 8);
        ui_num(UI_TXT, 10, 0, hp, 3, hp * 4 < mx ? TXT_RED : TXT_WHITE);
    }
    if (wave != c_wave) { c_wave = wave; ui_text(UI_TXT, 14, 0, "V", TXT_YELLOW); ui_num(UI_TXT, 15, 0, wave, 2, TXT_YELLOW); }
    if (kills != c_kills || need != c_need || is_boss != c_boss) {
        c_kills = kills; c_need = need; c_boss = is_boss;
        if (is_boss) ui_text(UI_TXT, 18, 0, "BOSS ", TXT_RED);
        else { ui_num(UI_TXT, 18, 0, kills, 2, TXT_CYAN); ui_text(UI_TXT, 20, 0, "/", TXT_CYAN); ui_num(UI_TXT, 21, 0, need, 2, TXT_CYAN); }
    }
    if (g_run.coins != c_coins) { c_coins = g_run.coins; ui_text(UI_TXT, 25, 0, "C", TXT_YELLOW); ui_num(UI_TXT, 26, 0, g_run.coins, 4, TXT_YELLOW); }
    int bh = (g_boss.active && !g_boss.dead) ? g_boss.hp : 0, bm = g_boss.active ? g_boss.maxhp : 0;
    if (bh != c_bhp || bm != c_bmax) {
        c_bhp = bh; c_bmax = bm;
        ui_clear_rows(UI_TXT, 2, 2);
        if (bm) { ui_text(UI_TXT, 0, 2, "BOSS", TXT_RED); ui_bar(5, 2, bh, bm, 12); }
    }
}

/* ===== game.c ===== */
static const game_mode_t *cur, *pending;
void game_set_mode(const game_mode_t *m) { pending = m; }
void game_frame(void) {
    if (pending) {
        if (cur && cur->leave) cur->leave();
        ui_clear(UI_TXT); ui_clear(UI_PNL);
        cur = pending; pending = 0;
        if (cur->enter) cur->enter();
    }
    if (cur) {
        if (cur->update) cur->update();
        spr_begin();
        if (cur->render) cur->render();
        spr_end();
        cam_update();
    }
}

/* ===== mode_title.c ===== */
static u32 frames;
static void mode_title_enter(void) {
    cam_init(SCREEN_W, SCREEN_H);
    ui_text_center(UI_TXT, 2, "BANDANA", TXT_YELLOW);
    ui_text_center(UI_TXT, 5, "FIGHTERS", TXT_YELLOW);
    ui_text_center(UI_TXT, 8, "GBA EDITION", TXT_CYAN);
    ui_text(UI_TXT, 8, 11, "> VAGUES", TXT_WHITE);
    ui_text(UI_TXT, 8, 13, "  HISTOIRE", TXT_RED);
    ui_text(UI_TXT, 8, 15, "  BIENTOT", TXT_RED);
    ui_text(UI_TXT, 2, 17, "RECORD V", TXT_WHITE); ui_num(UI_TXT, 10, 17, g_save.best_wave, 2, TXT_WHITE);
    ui_text(UI_TXT, 16, 17, "PIECES", TXT_YELLOW); ui_num(UI_TXT, 22, 17, (int)MIN(g_save.coins, 99999u), 5, TXT_YELLOW);
}
static void mode_title_update(void) {
    frames++;
    if (g_in.pressed & (KEY_START | KEY_A)) { rng_seed(frames * 2654435761u + 1); game_set_mode(&MODE_WAVES); }
}
static void mode_title_render(void) { }
const game_mode_t MODE_TITLE = { mode_title_enter, mode_title_update, mode_title_render, 0 };

/* ===== mode_waves.c ===== */
/* MODE VAGUES : COMBAT -> PORTE -> CHOIX DE BONUS -> VAGUE SUIVANTE. Bonus cumulatifs pendant toute la run. */
enum { WP_COMBAT, WP_DOOR, WP_BONUS, WP_OVER };
#define DOOR_X 216
static struct { int wave, boss, phase, kills, needed, spawned, timer, interval, toast, cursor, over_t, paused; u8 offer[3]; } W;
int waves_phase(void) { return W.phase; }
int waves_wave(void) { return W.wave; }
int waves_kills(void) { return W.kills; }
int waves_needed(void) { return W.needed; }
int waves_offer(int i) { return W.offer[i]; }
int waves_is_boss(void) { return W.boss; }

static void toast(const char *a, const char *b, int frames) {
    ui_clear_rows(UI_TXT, 6, 4);
    ui_text_center(UI_TXT, 6, a, TXT_YELLOW);
    if (b) ui_text_center(UI_TXT, 8, b, TXT_WHITE);
    W.toast = frames;
}
static void wave_start(int n) {
    W.wave = n; W.boss = (n % 5 == 0);
    W.needed = W.boss ? 1 : MIN(6 + (n - 1) * 2, 26);                      /* min(6 + 2(n-1), 26) */
    W.kills = W.spawned = W.timer = 0;
    W.interval = MAX(39, 90 - (36 * n) / 10);                              /* max(650, 1500 - 60n) ms en frames */
    W.phase = WP_COMBAT;
    enemies_clear(); proj_clear(); fxp_clear();
    g_player.b.x = FX(SCREEN_W / 2 - g_player.b.w / 2); g_player.b.vx = 0;
    g_boss.active = 0;
    if (W.boss) { boss_spawn(g_run.score); toast("VAGUE BOSS", W.wave < 10 ? "LE CAID" : "LE CAID !", 90); }
    else { char b[8] = "VAGUE "; (void)b; toast("VAGUE", 0, 90); ui_num(UI_TXT, 17, 6, n, 2, TXT_YELLOW); }
}
static void on_enemy_down(void) {
    if (W.phase != WP_COMBAT || W.boss) return;
    if (++W.kills >= W.needed) { W.phase = WP_DOOR; proj_clear_hostile(); toast("VAGUE FINIE", "VA A LA PORTE >", 150); sfx_play(SFX_DOOR); }
}
static void on_boss_down(void) {
    if (W.phase != WP_COMBAT || !W.boss) return;
    W.kills = 1; W.phase = WP_DOOR; proj_clear_hostile(); toast("BOSS VAINCU", "VA A LA PORTE >", 150); sfx_play(SFX_DOOR);
}
static void on_player_down(void) { W.phase = WP_OVER; W.over_t = 0; }
static void draw_offer(void) {
    ui_clear_rows(UI_TXT, 4, 28);
    ui_panel(2, 4, 26, 14);
    ui_text_center(UI_TXT, 5, "CHOISIS UN BONUS", TXT_YELLOW);
    for (int i = 0; i < 3; i++) {
        ui_text(UI_TXT, 4, 8 + i * 3, i == W.cursor ? ">" : " ", TXT_YELLOW);
        ui_text(UI_TXT, 6, 8 + i * 3, g_bonus_defs[W.offer[i]].name, i == W.cursor ? TXT_WHITE : TXT_CYAN);
    }
}
static void reached_door(void) {
    W.phase = WP_BONUS; W.cursor = 0;
    u8 pool[B_COUNT]; for (int i = 0; i < B_COUNT; i++) pool[i] = (u8)i;
    int n = B_COUNT;
    for (int k = 0; k < 3; k++) { int r = rng_range(n); W.offer[k] = pool[r]; pool[r] = pool[--n]; }   /* 3 bonus distincts */
    sfx_play(SFX_DOOR);
    save_write();
    hud_reset();
    draw_offer();
}
static void mode_waves_enter(void) {
    cam_init(SCREEN_W, SCREEN_H); level_init_arena();
    g_run.score = 0; g_run.coins = 0;
    bonus_reset(); coins_clear(); fxp_clear(); proj_clear(); enemies_clear(); g_boss.active = 0;
    g_hook_enemy_down = on_enemy_down; g_hook_boss_down = on_boss_down; g_hook_player_down = on_player_down;
    player_init(g_save.sel_char);
    W.paused = 0;
    hud_reset();
    wave_start(1);
}
static void mode_waves_leave(void) { g_hook_enemy_down = 0; g_hook_boss_down = 0; g_hook_player_down = 0; }
static void mode_waves_update(void) {
    if (W.phase == WP_BONUS) {
        if (g_in.pressed & KEY_UP)   { W.cursor = (W.cursor + 2) % 3; draw_offer(); }
        if (g_in.pressed & KEY_DOWN) { W.cursor = (W.cursor + 1) % 3; draw_offer(); }
        if (g_in.pressed & KEY_A) {
            bonus_apply(W.offer[W.cursor]);                                  /* applique immediatement, conserve ensuite */
            sfx_play(SFX_BONUS);
            ui_clear(UI_PNL); ui_clear_rows(UI_TXT, 4, 28);
            hud_reset();
            wave_start(W.wave + 1);
        }
        return;
    }
    if (W.phase == WP_OVER) {
        if (W.over_t == 0) {
            if (W.wave > g_save.best_wave) g_save.best_wave = (u16)MIN(W.wave, 99);
            save_write();
            ui_clear_rows(UI_TXT, 4, 28); ui_panel(4, 5, 22, 10);
            ui_text_center(UI_TXT, 6, "GAME OVER", TXT_RED);
            ui_text(UI_TXT, 7, 9, "VAGUE", TXT_WHITE); ui_num(UI_TXT, 14, 9, W.wave, 3, TXT_WHITE);
            ui_text(UI_TXT, 7, 11, "PIECES", TXT_YELLOW); ui_num(UI_TXT, 14, 11, g_run.coins, 3, TXT_YELLOW);
        }
        if (W.over_t < 255) W.over_t++;
        player_update(); fxp_update(); enemies_update(); proj_update(); coins_update();
        if (W.over_t > 60 && (g_in.pressed & KEY_START)) { ui_clear(UI_PNL); game_set_mode(&MODE_TITLE); }
        return;
    }
    if (g_in.pressed & KEY_START) {                                          /* pause */
        W.paused = !W.paused;
        if (W.paused) { toast("PAUSE", "SELECT = MENU", 0); }
        else { ui_clear_rows(UI_TXT, 6, 4); W.toast = 0; }
    }
    if (W.paused) {
        if (g_in.pressed & KEY_SELECT) { save_write(); game_set_mode(&MODE_TITLE); }
        return;
    }
    player_update(); enemies_update(); boss_update(); proj_update(); coins_update(); fxp_update();
    if (W.toast > 0 && --W.toast == 0) ui_clear_rows(UI_TXT, 6, 4);
    if (W.phase == WP_COMBAT && !W.boss) {
        W.timer++;
        if (W.spawned < W.needed && W.timer >= W.interval && enemies_alive() < ENEMY_ALIVE_MAX) {
            if (enemies_spawn_random()) { W.timer = 0; W.spawned++; }
        }
    } else if (W.phase == WP_DOOR && !g_player.dead && player_center_x() >= DOOR_X - 4) reached_door();
    cam_follow(player_center_x());
}
static void mode_waves_render(void) {
    if (W.phase != WP_BONUS) {
        if (W.phase == WP_DOOR) spr_world(DOOR_X, GROUND_Y - 32, SPR_16x32, TILE_DOOR, PAL_MISC, 0);
        coins_render(); enemies_render(); boss_render(); player_render(); proj_render(); fxp_render();
    }
    hud_update(W.wave, W.kills, W.needed, W.boss);
}
const game_mode_t MODE_WAVES = { mode_waves_enter, mode_waves_update, mode_waves_render, mode_waves_leave };

/* ===== main.c ===== */
/* BANDANA FIGHTERS GBA - point d'entree. Pipeline : vsync -> flush OAM/scroll -> input -> logique -> rendu (shadow OAM). */
int main(void) {
    hw_init();
    art_init(); ui_init();
    save_load();
    rng_seed(0xBA2DA2A);
    game_set_mode(&MODE_TITLE);
    game_frame();                 /* construit la 1re image avant d'allumer l'ecran */
    hw_flush();
    hw_display(1);
    for (;;) {
        hw_vsync();
        hw_flush();
        input_update();
        game_frame();
    }
}
