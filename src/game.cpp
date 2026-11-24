#include "game.hpp"
#include "eadkpp.h"

namespace {
uint16_t g_band[320 * 60];

constexpr uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
  return (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

constexpr uint16_t C_WHITE    = rgb565(255, 255, 255);
constexpr uint16_t C_BG_LIGHT = rgb565(246, 248, 252);
constexpr uint16_t C_GRID     = rgb565(224, 230, 240);
constexpr uint16_t C_BLACK    = rgb565(22, 26, 34);
constexpr uint16_t C_DARK     = rgb565(30, 38, 54);
constexpr uint16_t C_DARK_HI  = rgb565(48, 62, 88);
constexpr uint16_t C_GRAY     = rgb565(145, 155, 172);
constexpr uint16_t C_ROAD     = rgb565(72, 78, 90);
constexpr uint16_t C_BLUE     = rgb565(24, 162, 245);
constexpr uint16_t C_BLUE_DK  = rgb565(16, 108, 175);
constexpr uint16_t C_RED      = rgb565(238, 56, 48);
constexpr uint16_t C_GREEN    = rgb565(46, 204, 86);
constexpr uint16_t C_GREEN_DK = rgb565(24, 145, 48);
constexpr uint16_t C_GOLD     = rgb565(255, 202, 28);
constexpr uint16_t C_PURPLE   = rgb565(162, 68, 242);
constexpr uint16_t C_PINK     = rgb565(238, 48, 142);
constexpr uint16_t C_ORANGE   = rgb565(248, 128, 32);
constexpr uint16_t C_CYAN     = rgb565(28, 208, 224);

constexpr uint16_t PELLET_COLS[8] = {
  C_RED, C_GOLD, C_GREEN, C_BLUE, C_PURPLE, C_ORANGE, C_CYAN, C_PINK
};

constexpr float DX16[16] = {
  1.000f, 0.924f, 0.707f, 0.383f, 0.000f,-0.383f,-0.707f,-0.924f,
 -1.000f,-0.924f,-0.707f,-0.383f, 0.000f, 0.383f, 0.707f, 0.924f
};
constexpr float DY16[16] = {
  0.000f, 0.383f, 0.707f, 0.924f, 1.000f, 0.924f, 0.707f, 0.383f,
  0.000f,-0.383f,-0.707f,-0.924f,-1.000f,-0.924f,-0.707f,-0.383f
};

void getGlyph5x7(char c, uint8_t out[7]) {
  for (int i = 0; i < 7; i++) out[i] = 0;
  if (c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
  switch (c) {
    case 'A': out[0]=0x0E;out[1]=0x11;out[2]=0x11;out[3]=0x1F;out[4]=0x11;out[5]=0x11;out[6]=0x11; break;
    case 'B': out[0]=0x1E;out[1]=0x11;out[2]=0x11;out[3]=0x1E;out[4]=0x11;out[5]=0x11;out[6]=0x1E; break;
    case 'C': out[0]=0x0E;out[1]=0x11;out[2]=0x10;out[3]=0x10;out[4]=0x10;out[5]=0x11;out[6]=0x0E; break;
    case 'D': out[0]=0x1C;out[1]=0x12;out[2]=0x11;out[3]=0x11;out[4]=0x11;out[5]=0x12;out[6]=0x1C; break;
    case 'E': out[0]=0x1F;out[1]=0x10;out[2]=0x10;out[3]=0x1E;out[4]=0x10;out[5]=0x10;out[6]=0x1F; break;
    case 'F': out[0]=0x1F;out[1]=0x10;out[2]=0x10;out[3]=0x1E;out[4]=0x10;out[5]=0x10;out[6]=0x10; break;
    case 'G': out[0]=0x0E;out[1]=0x11;out[2]=0x10;out[3]=0x17;out[4]=0x11;out[5]=0x11;out[6]=0x0F; break;
    case 'H': out[0]=0x11;out[1]=0x11;out[2]=0x11;out[3]=0x1F;out[4]=0x11;out[5]=0x11;out[6]=0x11; break;
    case 'I': out[0]=0x0E;out[1]=0x04;out[2]=0x04;out[3]=0x04;out[4]=0x04;out[5]=0x04;out[6]=0x0E; break;
    case 'J': out[0]=0x07;out[1]=0x02;out[2]=0x02;out[3]=0x02;out[4]=0x12;out[5]=0x12;out[6]=0x0C; break;
    case 'K': out[0]=0x11;out[1]=0x12;out[2]=0x14;out[3]=0x18;out[4]=0x14;out[5]=0x12;out[6]=0x11; break;
    case 'L': out[0]=0x10;out[1]=0x10;out[2]=0x10;out[3]=0x10;out[4]=0x10;out[5]=0x10;out[6]=0x1F; break;
    case 'M': out[0]=0x11;out[1]=0x1B;out[2]=0x15;out[3]=0x15;out[4]=0x11;out[5]=0x11;out[6]=0x11; break;
    case 'N': out[0]=0x11;out[1]=0x19;out[2]=0x15;out[3]=0x13;out[4]=0x11;out[5]=0x11;out[6]=0x11; break;
    case 'O': out[0]=0x0E;out[1]=0x11;out[2]=0x11;out[3]=0x11;out[4]=0x11;out[5]=0x11;out[6]=0x0E; break;
    case 'P': out[0]=0x1E;out[1]=0x11;out[2]=0x11;out[3]=0x1E;out[4]=0x10;out[5]=0x10;out[6]=0x10; break;
    case 'Q': out[0]=0x0E;out[1]=0x11;out[2]=0x11;out[3]=0x11;out[4]=0x15;out[5]=0x12;out[6]=0x0D; break;
    case 'R': out[0]=0x1E;out[1]=0x11;out[2]=0x11;out[3]=0x1E;out[4]=0x14;out[5]=0x12;out[6]=0x11; break;
    case 'S': out[0]=0x0F;out[1]=0x10;out[2]=0x10;out[3]=0x0E;out[4]=0x01;out[5]=0x01;out[6]=0x1E; break;
    case 'T': out[0]=0x1F;out[1]=0x04;out[2]=0x04;out[3]=0x04;out[4]=0x04;out[5]=0x04;out[6]=0x04; break;
    case 'U': out[0]=0x11;out[1]=0x11;out[2]=0x11;out[3]=0x11;out[4]=0x11;out[5]=0x11;out[6]=0x0E; break;
    case 'V': out[0]=0x11;out[1]=0x11;out[2]=0x11;out[3]=0x11;out[4]=0x11;out[5]=0x0A;out[6]=0x04; break;
    case 'W': out[0]=0x11;out[1]=0x11;out[2]=0x11;out[3]=0x15;out[4]=0x15;out[5]=0x1B;out[6]=0x11; break;
    case 'X': out[0]=0x11;out[1]=0x11;out[2]=0x0A;out[3]=0x04;out[4]=0x0A;out[5]=0x11;out[6]=0x11; break;
    case 'Y': out[0]=0x11;out[1]=0x11;out[2]=0x0A;out[3]=0x04;out[4]=0x04;out[5]=0x04;out[6]=0x04; break;
    case 'Z': out[0]=0x1F;out[1]=0x01;out[2]=0x02;out[3]=0x04;out[4]=0x08;out[5]=0x10;out[6]=0x1F; break;
    case '0': out[0]=0x0E;out[1]=0x11;out[2]=0x13;out[3]=0x15;out[4]=0x19;out[5]=0x11;out[6]=0x0E; break;
    case '1': out[0]=0x04;out[1]=0x0C;out[2]=0x04;out[3]=0x04;out[4]=0x04;out[5]=0x04;out[6]=0x0E; break;
    case '2': out[0]=0x0E;out[1]=0x11;out[2]=0x01;out[3]=0x02;out[4]=0x04;out[5]=0x08;out[6]=0x1F; break;
    case '3': out[0]=0x1E;out[1]=0x01;out[2]=0x01;out[3]=0x0E;out[4]=0x01;out[5]=0x01;out[6]=0x1E; break;
    case '4': out[0]=0x02;out[1]=0x06;out[2]=0x0A;out[3]=0x12;out[4]=0x1F;out[5]=0x02;out[6]=0x02; break;
    case '5': out[0]=0x1F;out[1]=0x10;out[2]=0x10;out[3]=0x1E;out[4]=0x01;out[5]=0x01;out[6]=0x1E; break;
    case '6': out[0]=0x0E;out[1]=0x10;out[2]=0x10;out[3]=0x1E;out[4]=0x11;out[5]=0x11;out[6]=0x0E; break;
    case '7': out[0]=0x1F;out[1]=0x01;out[2]=0x02;out[3]=0x04;out[4]=0x08;out[5]=0x08;out[6]=0x08; break;
    case '8': out[0]=0x0E;out[1]=0x11;out[2]=0x11;out[3]=0x0E;out[4]=0x11;out[5]=0x11;out[6]=0x0E; break;
    case '9': out[0]=0x0E;out[1]=0x11;out[2]=0x11;out[3]=0x0F;out[4]=0x01;out[5]=0x01;out[6]=0x0E; break;
    case '.': out[5]=0x0C;out[6]=0x0C; break;
    case ':': out[1]=0x0C;out[2]=0x0C;out[4]=0x0C;out[5]=0x0C; break;
    case '?': out[0]=0x0E;out[1]=0x11;out[2]=0x01;out[3]=0x02;out[4]=0x04;out[6]=0x04; break;
    case '%': out[0]=0x19;out[1]=0x19;out[2]=0x02;out[3]=0x04;out[4]=0x08;out[5]=0x13;out[6]=0x13; break;
    case '/': out[0]=0x01;out[1]=0x02;out[2]=0x04;out[3]=0x04;out[4]=0x08;out[5]=0x10; break;
    case '-': out[3]=0x1F; break;
    default: break;
  }
}

static inline float agarRadius(float m) {
  if (m < 10.0f) m = 10.0f;
  float g = m * 0.1f + 2.0f;
  for (int i = 0; i < 4; i++) g = 0.5f * (g + m / g);
  return 6.5f + g * 1.15f;
}
} // namespace

Game::Game() {
  mode = MODE_MENU;
  menuSel = 0;
  quitOpen = false;
  quitYes = true;
  exitApp = false;
  backWasDown = false;
  keyCd = 0;
  tick = 0;
  rng = 0x9E3779B9u;
}

uint32_t Game::nextRand() {
  uint32_t x = rng;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  rng = x;
  return x;
}

float Game::randRange(float a, float b) {
  return a + (b - a) * ((float)(nextRand() & 0xFFFFu) / 65535.0f);
}

float Game::fastSqrt(float x) const {
  if (x <= 0.0f) return 0.0f;
  float g = x * 0.5f + 1.0f;
  for (int i = 0; i < 5; i++) g = 0.5f * (g + x / g);
  return g;
}

void Game::bandClear(uint16_t col) {
  for (int i = 0; i < W * BAND_H; i++) g_band[i] = col;
}

void Game::bandFillRect(int y0, int x, int y, int w, int h, uint16_t col) {
  if (w <= 0 || h <= 0) return;
  if (x < 0) { w += x; x = 0; }
  if (x + w > W) w = W - x;
  if (w <= 0) return;

  int ly0 = y - y0;
  int ly1 = ly0 + h;
  if (ly0 < 0) ly0 = 0;
  if (ly1 > BAND_H) ly1 = BAND_H;
  if (ly0 >= ly1) return;

  for (int ly = ly0; ly < ly1; ly++) {
    uint16_t* row = &g_band[ly * W + x];
    for (int lx = 0; lx < w; lx++) row[lx] = col;
  }
}

void Game::bandDrawLine(int y0, int x0, int y0p, int x1, int y1, int thick, uint16_t col) {
  int dx = x1 - x0;
  int dy = y1 - y0p;
  int steps = (dx < 0 ? -dx : dx);
  int ady = (dy < 0 ? -dy : dy);
  if (ady > steps) steps = ady;
  if (steps == 0) {
    bandFillRect(y0, x0 - thick / 2, y0p - thick / 2, thick, thick, col);
    return;
  }
  for (int s = 0; s <= steps; s++) {
    int px = x0 + (dx * s) / steps;
    int py = y0p + (dy * s) / steps;
    bandFillRect(y0, px - thick / 2, py - thick / 2, thick, thick, col);
  }
}

void Game::bandDrawDisc(int y0, int cx, int cy, int r, uint16_t col, uint16_t rimCol, int rimThick) {
  if (r <= 0) return;
  if (cx + r < 0 || cx - r >= W) return;
  if (cy + r < y0 || cy - r >= y0 + BAND_H) return;

  int r2 = r * r;
  int inR = (rimThick > 0 && r > rimThick) ? (r - rimThick) : r;
  int in2 = inR * inR;

  int ys = cy - r;
  int ye = cy + r;
  if (ys < y0) ys = y0;
  if (ye >= y0 + BAND_H) ye = y0 + BAND_H - 1;

  for (int y = ys; y <= ye; y++) {
    int dy = y - cy;
    int dy2 = dy * dy;
    int rem = r2 - dy2;
    if (rem < 0) continue;

    int sx = 0;
    while ((sx + 1) * (sx + 1) <= rem) sx++;

    int xs = cx - sx;
    int xe = cx + sx;
    if (xs < 0) xs = 0;
    if (xe >= W) xe = W - 1;
    if (xs > xe) continue;

    uint16_t* row = &g_band[(y - y0) * W];
    if (rimThick <= 0 || r <= rimThick) {
      for (int x = xs; x <= xe; x++) row[x] = col;
    } else {
      for (int x = xs; x <= xe; x++) {
        int dx = x - cx;
        row[x] = ((dx * dx + dy2) >= in2) ? rimCol : col;
      }
    }
  }
}

void Game::bandDrawChar(int y0, int x, int y, char c, int scale, uint16_t col) {
  uint8_t g[7];
  getGlyph5x7(c, g);
  for (int r = 0; r < 7; r++) {
    uint8_t bits = g[r];
    for (int b = 0; b < 5; b++) {
      if (bits & (1 << (4 - b))) {
        bandFillRect(y0, x + b * scale, y + r * scale, scale, scale, col);
      }
    }
  }
}

void Game::bandDrawText(int y0, int x, int y, const char* str, int scale, uint16_t col) {
  int cx = x;
  for (int i = 0; str[i] != '\0'; i++) {
    bandDrawChar(y0, cx, y, str[i], scale, col);
    cx += 6 * scale;
  }
}

void Game::bandDrawInt(int y0, int x, int y, int val, int scale, uint16_t col) {
  if (val < 0) val = 0;
  char buf[12];
  int d = 0;
  if (val == 0) {
    buf[d++] = '0';
  } else {
    while (val > 0 && d < 11) {
      buf[d++] = (char)('0' + (val % 10));
      val /= 10;
    }
  }
  int cx = x;
  while (d > 0) {
    bandDrawChar(y0, cx, y, buf[--d], scale, col);
    cx += 6 * scale;
  }
}

void Game::flushBand(int y0) {
  EADK::Display::pushRect(EADK::Rect(0, y0, W, BAND_H), reinterpret_cast<EADK::Color*>(g_band));
}

void Game::handleGlobalBack() {
  EADK::Keyboard::State k = EADK::Keyboard::scan();
  bool backDown = k.keyDown(EADK::Keyboard::Key::Back);
  bool backPressed = (backDown && !backWasDown);
  backWasDown = backDown;

  if (!backPressed) return;

  if (mode != MODE_MENU) {
    mode = MODE_MENU;
    quitOpen = false;
    keyCd = 10;
  } else {
    quitOpen = !quitOpen;
    if (quitOpen) quitYes = true;
    keyCd = 8;
  }
}

void Game::renderQuitBand(int y0) {
  bandFillRect(y0, 48, 66, 224, 108, C_DARK);
  bandFillRect(y0, 48, 66, 224, 4, C_RED);
  bandDrawText(y0, 74, 82, "QUITTER ?", 2, C_WHITE);

  uint16_t yCol = quitYes ? C_RED : C_DARK_HI;
  uint16_t nCol = !quitYes ? C_GREEN : C_DARK_HI;

  bandFillRect(y0, 72, 120, 76, 34, yCol);
  bandDrawText(y0, 92, 130, "OUI", 2, C_WHITE);

  bandFillRect(y0, 172, 120, 76, 34, nCol);
  bandDrawText(y0, 192, 130, "NON", 2, C_WHITE);
}

void Game::renderMenuBand(int y0) {
  bandClear(C_BG_LIGHT);
  for (int x = 0; x < W; x += 20) bandFillRect(y0, x, y0, 1, BAND_H, C_GRID);
  for (int y = 0; y < H; y += 20) bandFillRect(y0, 0, y, W, 1, C_GRID);

  bandDrawText(y0, 76, 14, "IO ARCADE 4-IN-1", 2, C_DARK);

  const char* titles[4] = { "1. AGAR.IO", "2. DIEP.IO", "3. HOLE.IO", "4. PAPER.IO" };
  const char* subs[4]   = { "SHIFT:FUSION  OK:SPLIT", "TANK + TIR + XP", "TROU NOIR 60S", "ZONE + TRAILEE" };
  const uint16_t cols[4]= { C_BLUE, C_RED, C_PURPLE, C_GREEN };

  for (int i = 0; i < 4; i++) {
    int bx = (i % 2 == 0) ? 16 : 166;
    int by = (i < 2) ? 48 : 132;
    bool sel = (menuSel == i);

    if (sel) {
      bandFillRect(y0, bx - 3, by - 3, 144, 78, C_GOLD);
    }
    bandFillRect(y0, bx, by, 138, 72, sel ? C_DARK : C_DARK_HI);
    bandFillRect(y0, bx, by, 138, 6, cols[i]);

    bandDrawText(y0, bx + 10, by + 16, titles[i], 2, C_WHITE);
    bandDrawText(y0, bx + 8,  by + 44, subs[i],   1, sel ? C_GOLD : C_GRAY);
  }

  bandDrawText(y0, 34, 218, "FLECHES:CHOISIR  OK:JOUER  BACK:QUITTER", 1, C_DARK);
}

void Game::updateMenu() {
  EADK::Keyboard::State k = EADK::Keyboard::scan();

  if (quitOpen) {
    if (keyCd == 0) {
      if (k.keyDown(EADK::Keyboard::Key::Left) || k.keyDown(EADK::Keyboard::Key::Right)) {
        quitYes = !quitYes;
        keyCd = 8;
      } else if (k.keyDown(EADK::Keyboard::Key::OK) || k.keyDown(EADK::Keyboard::Key::EXE)) {
        keyCd = 10;
        if (quitYes) {
          exitApp = true;
        } else {
          quitOpen = false;
        }
      }
    }
    return;
  }

  if (keyCd == 0) {
    int col = menuSel % 2;
    int row = menuSel / 2;
    bool moved = false;
    if (k.keyDown(EADK::Keyboard::Key::Left))  { col = (col + 1) % 2; moved = true; }
    if (k.keyDown(EADK::Keyboard::Key::Right)) { col = (col + 1) % 2; moved = true; }
    if (k.keyDown(EADK::Keyboard::Key::Up))    { row = (row + 1) % 2; moved = true; }
    if (k.keyDown(EADK::Keyboard::Key::Down))  { row = (row + 1) % 2; moved = true; }
    if (moved) {
      menuSel = row * 2 + col;
      keyCd = 7;
    } else if (k.keyDown(EADK::Keyboard::Key::OK) || k.keyDown(EADK::Keyboard::Key::EXE)) {
      keyCd = 10;
      if (menuSel == 0) { initAgar();  mode = MODE_AGAR; }
      else if (menuSel == 1) { initDiep();  mode = MODE_DIEP; }
      else if (menuSel == 2) { initHole();  mode = MODE_HOLE; }
      else { initPaper(); mode = MODE_PAPER; }
    }
  }
}

void Game::initAgar() {
  agarOver = false;
  for (int i = 0; i < AGAR_P_MAX; i++) agarP[i].active = false;
  agarP[0] = { true, 0.0f, 0.0f, 0.0f, 0.0f, 52.0f, 0.0f, C_BLUE, C_PINK, 0, 1.0f, 0.0f };
  agarCamX = 0.0f; agarCamY = 0.0f; agarZoom = 1.0f;
  agarAimX = 1.0f; agarAimY = 0.0f;

  for (int i = 0; i < AGAR_PEL_MAX; i++) {
    agarPel[i] = { true, randRange(-650.0f, 650.0f), randRange(-650.0f, 650.0f), 3.4f, PELLET_COLS[nextRand() & 7] };
  }
  agarVir[0] = { true,  260.0f, -180.0f, 22.0f, 70.0f };
  agarVir[1] = { true, -320.0f,  240.0f, 22.0f, 70.0f };

  const uint16_t bCols[5] = { C_RED, C_GREEN, C_GOLD, C_PURPLE, C_ORANGE };
  const uint16_t bRims[5] = { C_CYAN, C_PINK, C_BLUE, C_GOLD, C_RED };
  for (int i = 0; i < AGAR_B_MAX; i++) {
    int d = nextRand() & 15;
    float dist = randRange(180.0f, 520.0f);
    agarB[i].active = true;
    agarB[i].x = DX16[d] * dist;
    agarB[i].y = DY16[d] * dist;
    agarB[i].vx = agarB[i].vy = 0.0f;
    agarB[i].mass = randRange(18.0f, 62.0f);
    agarB[i].mergeTimer = 0.0f;
    agarB[i].col = bCols[i % 5];
    agarB[i].rim = bRims[i % 5];
    agarB[i].aiCd = 35 + (nextRand() % 45);
    agarB[i].dirX = DX16[nextRand() & 15];
    agarB[i].dirY = DY16[nextRand() & 15];
  }
}

float Game::agarTotalMass() const {
  float s = 0.0f;
  for (int i = 0; i < AGAR_P_MAX; i++) if (agarP[i].active) s += agarP[i].mass;
  return s;
}

void Game::agarSplit() {
  int count = 0;
  for (int i = 0; i < AGAR_P_MAX; i++) if (agarP[i].active) count++;
  if (count >= AGAR_P_MAX) return;

  int best = -1;
  float bm = 0.0f;
  for (int i = 0; i < AGAR_P_MAX; i++) {
    if (agarP[i].active && agarP[i].mass > bm) { bm = agarP[i].mass; best = i; }
  }
  if (best < 0 || bm < 32.0f) return;

  int slot = -1;
  for (int i = 0; i < AGAR_P_MAX; i++) if (!agarP[i].active) { slot = i; break; }
  if (slot < 0) return;

  float half = agarP[best].mass * 0.5f;
  agarP[best].mass = half;
  agarP[best].mergeTimer = 260.0f;
  float r = agarRadius(half);
  agarP[slot] = agarP[best];
  agarP[slot].x += agarAimX * (r * 1.5f);
  agarP[slot].y += agarAimY * (r * 1.5f);
  agarP[slot].vx = agarAimX * 8.2f;
  agarP[slot].vy = agarAimY * 8.2f;
}

void Game::agarMergeWithShift() {
  int target = -1;
  float bestMass = -1.0f;
  for (int i = 0; i < AGAR_P_MAX; i++) {
    if (agarP[i].active && agarP[i].mass > bestMass) {
      bestMass = agarP[i].mass;
      target = i;
    }
  }
  if (target < 0) return;

  for (int i = 0; i < AGAR_P_MAX; i++) {
    if (i == target || !agarP[i].active) continue;
    float dx = agarP[target].x - agarP[i].x;
    float dy = agarP[target].y - agarP[i].y;
    float dist = fastSqrt(dx * dx + dy * dy);
    if (dist > 0.01f) {
      agarP[i].vx = (dx / dist) * 6.5f;
      agarP[i].vy = (dy / dist) * 6.5f;
    }
    if (dist < agarRadius(agarP[target].mass) * 1.15f + agarRadius(agarP[i].mass)) {
      agarP[target].mass += agarP[i].mass;
      agarP[i].active = false;
    }
  }
}

void Game::updateAgar() {
  EADK::Keyboard::State k = EADK::Keyboard::scan();
  if (agarOver) {
    if (keyCd == 0 && (k.keyDown(EADK::Keyboard::Key::OK) || k.keyDown(EADK::Keyboard::Key::EXE))) {
      keyCd = 12;
      initAgar();
    }
    return;
  }

  float ix = 0.0f, iy = 0.0f;
  if (k.keyDown(EADK::Keyboard::Key::Left)) ix -= 1.0f;
  if (k.keyDown(EADK::Keyboard::Key::Right)) ix += 1.0f;
  if (k.keyDown(EADK::Keyboard::Key::Up)) iy -= 1.0f;
  if (k.keyDown(EADK::Keyboard::Key::Down)) iy += 1.0f;
  bool mv = (ix != 0.0f || iy != 0.0f);
  if (mv) {
    if (ix != 0.0f && iy != 0.0f) { ix *= 0.7071f; iy *= 0.7071f; }
    agarAimX = ix; agarAimY = iy;
  }

  if (k.keyDown(EADK::Keyboard::Key::OK) && keyCd == 0) {
    agarSplit();
    keyCd = 8;
  }
  if (k.keyDown(EADK::Keyboard::Key::Shift)) {
    agarMergeWithShift();
  }

  for (int i = 0; i < AGAR_P_MAX; i++) {
    AgarCell& c = agarP[i];
    if (!c.active) continue;
    float r = agarRadius(c.mass);
    float spd = 3.5f / (1.0f + fastSqrt(c.mass) * 0.038f);
    if (spd < 1.35f) spd = 1.35f;

    c.vx += ((mv ? agarAimX * spd : 0.0f) - c.vx) * 0.22f;
    c.vy += ((mv ? agarAimY * spd : 0.0f) - c.vy) * 0.22f;
    c.x += c.vx;
    c.y += c.vy;
    if (c.mergeTimer > 0.0f) c.mergeTimer -= 1.0f;

    for (int j = 0; j < AGAR_P_MAX; j++) {
      if (i == j || !agarP[j].active) continue;
      float dx = agarP[j].x - c.x, dy = agarP[j].y - c.y;
      float dist = fastSqrt(dx * dx + dy * dy);
      float minSep = (r + agarRadius(agarP[j].mass)) * 0.85f;
      if (c.mergeTimer <= 0.0f && agarP[j].mergeTimer <= 0.0f && dist < minSep * 0.8f && c.mass >= agarP[j].mass) {
        c.mass += agarP[j].mass;
        agarP[j].active = false;
      } else if (dist > 0.01f && dist < minSep) {
        float push = (minSep - dist) * 0.12f;
        c.x -= (dx / dist) * push;
        c.y -= (dy / dist) * push;
      }
    }
  }

  for (int i = 0; i < AGAR_B_MAX; i++) {
    AgarCell& b = agarB[i];
    if (!b.active) continue;
    b.aiCd--;
    if (b.aiCd <= 0) {
      b.aiCd = 45 + (nextRand() % 55);
      bool reacted = false;
      if ((nextRand() % 4) == 0) {
        for (int p = 0; p < AGAR_P_MAX; p++) {
          if (!agarP[p].active) continue;
          float dx = agarP[p].x - b.x, dy = agarP[p].y - b.y;
          float d = fastSqrt(dx * dx + dy * dy);
          if (d > 1.0f && d < 105.0f && agarP[p].mass > b.mass * 1.35f) {
            b.dirX = -dx / d;
            b.dirY = -dy / d;
            reacted = true;
            break;
          }
        }
      }
      if (!reacted) {
        int d = nextRand() & 15;
        b.dirX = DX16[d];
        b.dirY = DY16[d];
      }
    }

    float spd = 1.95f / (1.0f + fastSqrt(b.mass) * 0.03f);
    if (spd < 0.9f) spd = 0.9f;
    b.vx += (b.dirX * spd - b.vx) * 0.14f;
    b.vy += (b.dirY * spd - b.vy) * 0.14f;
    b.x += b.vx;
    b.y += b.vy;
  }

  for (int i = 0; i < AGAR_P_MAX; i++) {
    if (!agarP[i].active) continue;
    float r = agarRadius(agarP[i].mass);

    for (int p = 0; p < AGAR_PEL_MAX; p++) {
      if (!agarPel[p].active) continue;
      float dx = agarPel[p].x - agarP[i].x, dy = agarPel[p].y - agarP[i].y;
      if (dx * dx + dy * dy <= (r + 3.5f) * (r + 3.5f)) {
        agarP[i].mass += 2.5f;
        agarPel[p].x = agarCamX + randRange(-650.0f, 650.0f);
        agarPel[p].y = agarCamY + randRange(-650.0f, 650.0f);
      }
    }

    for (int v = 0; v < AGAR_VIR_MAX; v++) {
      if (!agarVir[v].active || agarP[i].mass <= agarVir[v].mass) continue;
      float dx = agarVir[v].x - agarP[i].x, dy = agarVir[v].y - agarP[i].y;
      if (dx * dx + dy * dy <= (r * 0.72f) * (r * 0.72f)) {
        int d = nextRand() & 15;
        agarVir[v].x = agarCamX + DX16[d] * 460.0f;
        agarVir[v].y = agarCamY + DY16[d] * 460.0f;
        agarSplit();
      }
    }

    for (int b = 0; b < AGAR_B_MAX; b++) {
      if (!agarB[b].active) continue;
      float br = agarRadius(agarB[b].mass);
      float dx = agarB[b].x - agarP[i].x, dy = agarB[b].y - agarP[i].y;
      float d2 = dx * dx + dy * dy;
      if (agarP[i].mass > agarB[b].mass * 1.12f && d2 < (r * 0.92f) * (r * 0.92f)) {
        agarP[i].mass += agarB[b].mass * 0.85f;
        agarB[b].active = false;
      } else if (agarB[b].mass > agarP[i].mass * 1.25f && d2 < (br * 0.85f) * (br * 0.85f)) {
        agarB[b].mass += agarP[i].mass * 0.85f;
        agarP[i].active = false;
        break;
      }
    }
  }

  for (int b = 0; b < AGAR_B_MAX; b++) {
    if (!agarB[b].active) continue;
    float br = agarRadius(agarB[b].mass);
    for (int p = 0; p < AGAR_PEL_MAX; p++) {
      float dx = agarPel[p].x - agarB[b].x, dy = agarPel[p].y - agarB[b].y;
      if (dx * dx + dy * dy <= (br + 3.0f) * (br + 3.0f)) {
        agarB[b].mass += 1.5f;
        agarPel[p].x = agarCamX + randRange(-650.0f, 650.0f);
        agarPel[p].y = agarCamY + randRange(-650.0f, 650.0f);
      }
    }
  }

  float cx = 0.0f, cy = 0.0f, sm = 0.0f;
  int alive = 0;
  for (int i = 0; i < AGAR_P_MAX; i++) {
    if (agarP[i].active) { cx += agarP[i].x * agarP[i].mass; cy += agarP[i].y * agarP[i].mass; sm += agarP[i].mass; alive++; }
  }
  if (alive == 0) { agarOver = true; return; }
  cx /= sm; cy /= sm;
  agarCamX += (cx - agarCamX) * 0.14f;
  agarCamY += (cy - agarCamY) * 0.14f;
  float tz = 1.0f / (1.0f + fastSqrt(sm) * 0.032f);
  if (tz < 0.22f) tz = 0.22f;
  agarZoom += (tz - agarZoom) * 0.09f;

  for (int p = 0; p < AGAR_PEL_MAX; p++) {
    float dx = agarPel[p].x - agarCamX, dy = agarPel[p].y - agarCamY;
    if (dx * dx + dy * dy > 820.0f * 820.0f) {
      agarPel[p].x = agarCamX + randRange(-560.0f, 560.0f);
      agarPel[p].y = agarCamY + randRange(-560.0f, 560.0f);
    }
  }
  for (int v = 0; v < AGAR_VIR_MAX; v++) {
    float dx = agarVir[v].x - agarCamX, dy = agarVir[v].y - agarCamY;
    if (dx * dx + dy * dy > 950.0f * 950.0f) {
      int d = nextRand() & 15;
      agarVir[v].x = agarCamX + DX16[d] * 420.0f;
      agarVir[v].y = agarCamY + DY16[d] * 420.0f;
    }
  }
  for (int b = 0; b < AGAR_B_MAX; b++) {
    float dx = agarB[b].x - agarCamX, dy = agarB[b].y - agarCamY;
    if (!agarB[b].active || dx * dx + dy * dy > 920.0f * 920.0f) {
      int d = nextRand() & 15;
      agarB[b].active = true;
      agarB[b].x = agarCamX + DX16[d] * 320.0f;
      agarB[b].y = agarCamY + DY16[d] * 320.0f;
      agarB[b].mass = randRange(18.0f, sm * 0.78f + 16.0f);
      agarB[b].dirX = DX16[(d + 8) & 15];
      agarB[b].dirY = DY16[(d + 8) & 15];
    }
  }
}

void Game::renderAgarBand(int y0) {
  bandClear(C_WHITE);
  int step = (int)(36.0f * agarZoom);
  if (step < 8) step = 8;
  int ox = ((W / 2) - (int)(agarCamX * agarZoom)) % step; if (ox < 0) ox += step;
  int oy = ((H / 2) - (int)(agarCamY * agarZoom)) % step; if (oy < 0) oy += step;
  for (int x = ox; x < W; x += step) bandFillRect(y0, x, y0, 1, BAND_H, C_GRID);
  for (int y = oy; y < H; y += step) bandFillRect(y0, 0, y, W, 1, C_GRID);

  for (int p = 0; p < AGAR_PEL_MAX; p++) {
    int sx = (W / 2) + (int)((agarPel[p].x - agarCamX) * agarZoom);
    int sy = (H / 2) + (int)((agarPel[p].y - agarCamY) * agarZoom);
    int sr = (int)(agarPel[p].r * agarZoom); if (sr < 2) sr = 2;
    bandDrawDisc(y0, sx, sy, sr, agarPel[p].col, C_BLACK, 0);
  }
  for (int v = 0; v < AGAR_VIR_MAX; v++) {
    int sx = (W / 2) + (int)((agarVir[v].x - agarCamX) * agarZoom);
    int sy = (H / 2) + (int)((agarVir[v].y - agarCamY) * agarZoom);
    int sr = (int)(agarVir[v].r * agarZoom); if (sr < 4) sr = 4;
    for (int i = 0; i < 12; i++) {
      int idx = (i + ((tick >> 3) & 1)) & 15;
      bandDrawDisc(y0, sx + (int)(DX16[idx] * sr * 0.9f), sy + (int)(DY16[idx] * sr * 0.9f), (sr / 4) + 1, C_GREEN_DK, C_GREEN, 0);
    }
    bandDrawDisc(y0, sx, sy, sr, C_GREEN, C_GREEN_DK, 2);
  }
  for (int b = 0; b < AGAR_B_MAX; b++) {
    if (!agarB[b].active) continue;
    int sx = (W / 2) + (int)((agarB[b].x - agarCamX) * agarZoom);
    int sy = (H / 2) + (int)((agarB[b].y - agarCamY) * agarZoom);
    int sr = (int)(agarRadius(agarB[b].mass) * agarZoom); if (sr < 2) sr = 2;
    bandDrawDisc(y0, sx, sy, sr, agarB[b].col, agarB[b].rim, sr >= 14 ? 3 : 2);
  }
  for (int i = 0; i < AGAR_P_MAX; i++) {
    if (!agarP[i].active) continue;
    int sx = (W / 2) + (int)((agarP[i].x - agarCamX) * agarZoom);
    int sy = (H / 2) + (int)((agarP[i].y - agarCamY) * agarZoom);
    int sr = (int)(agarRadius(agarP[i].mass) * agarZoom); if (sr < 2) sr = 2;
    bandDrawDisc(y0, sx, sy, sr, agarP[i].col, agarP[i].rim, sr >= 14 ? 3 : 2);
  }

  bandFillRect(y0, 6, 6, 102, 16, C_DARK);
  bandDrawText(y0, 12, 10, "MASS:", 1, C_WHITE);
  bandDrawInt(y0, 46, 10, (int)agarTotalMass(), 1, C_GOLD);

  bandFillRect(y0, 186, 6, 128, 16, C_DARK);
  bandDrawText(y0, 192, 10, "OK:SPLIT SHIFT:FUSION", 1, C_CYAN);

  if (agarOver) {
    bandFillRect(y0, 60, 74, 200, 92, C_DARK);
    bandDrawText(y0, 104, 92, "GAME OVER", 2, C_RED);
    bandDrawText(y0, 98, 130, "OK : REJOUER", 2, C_WHITE);
  }
}

void Game::initDiep() {
  diepOver = false;
  diepX = 0.0f; diepY = 0.0f; diepVx = 0.0f; diepVy = 0.0f;
  diepAimX = 1.0f; diepAimY = 0.0f;
  diepHp = 100.0f; diepMaxHp = 100.0f;
  diepXp = 0; diepLevel = 1; diepShootCd = 0;

  for (int i = 0; i < DIEP_BULLETS; i++) diepBullets[i].active = false;
  for (int i = 0; i < DIEP_SHAPES; i++) {
    int k = nextRand() % 6;
    int kind = (k < 3) ? 0 : ((k < 5) ? 1 : 2);
    diepShapes[i] = {
      true, randRange(-420.0f, 420.0f), randRange(-420.0f, 420.0f),
      randRange(-0.3f, 0.3f), randRange(-0.3f, 0.3f),
      kind == 0 ? 20.0f : (kind == 1 ? 45.0f : 110.0f),
      kind == 0 ? 20.0f : (kind == 1 ? 45.0f : 110.0f),
      kind == 0 ? 9.0f : (kind == 1 ? 11.0f : 15.0f), kind
    };
  }
  for (int i = 0; i < DIEP_ENEMIES; i++) {
    int d = nextRand() & 15;
    diepEnemies[i] = { true, DX16[d] * 300.0f, DY16[d] * 300.0f, 0.0f, 0.0f, 75.0f, 75.0f, -DX16[d], -DY16[d], 25 + i * 5 };
  }
}

void Game::diepSpawnBullet(bool fromPlayer, float x, float y, float dx, float dy, float dmg) {
  for (int i = 0; i < DIEP_BULLETS; i++) {
    if (!diepBullets[i].active) {
      diepBullets[i] = { true, fromPlayer, x + dx * 16.0f, y + dy * 16.0f, dx * 6.2f, dy * 6.2f, dmg, 65 };
      return;
    }
  }
}

void Game::updateDiep() {
  EADK::Keyboard::State k = EADK::Keyboard::scan();
  if (diepOver) {
    if (keyCd == 0 && (k.keyDown(EADK::Keyboard::Key::OK) || k.keyDown(EADK::Keyboard::Key::EXE))) {
      keyCd = 12;
      initDiep();
    }
    return;
  }

  float ix = 0.0f, iy = 0.0f;
  if (k.keyDown(EADK::Keyboard::Key::Left)) ix -= 1.0f;
  if (k.keyDown(EADK::Keyboard::Key::Right)) ix += 1.0f;
  if (k.keyDown(EADK::Keyboard::Key::Up)) iy -= 1.0f;
  if (k.keyDown(EADK::Keyboard::Key::Down)) iy += 1.0f;
  if (ix != 0.0f || iy != 0.0f) {
    if (ix != 0.0f && iy != 0.0f) { ix *= 0.7071f; iy *= 0.7071f; }
    diepAimX = ix; diepAimY = iy;
  }
  diepVx += (ix * 2.8f - diepVx) * 0.22f;
  diepVy += (iy * 2.8f - diepVy) * 0.22f;
  diepX += diepVx; diepY += diepVy;
  if (diepHp < diepMaxHp) diepHp += 0.06f;

  if (diepShootCd > 0) diepShootCd--;
  if ((k.keyDown(EADK::Keyboard::Key::OK) || k.keyDown(EADK::Keyboard::Key::EXE)) && diepShootCd == 0) {
    float dmg = 14.0f + diepLevel * 3.0f;
    diepSpawnBullet(true, diepX, diepY, diepAimX, diepAimY, dmg);
    if (diepLevel >= 4) {
      diepSpawnBullet(true, diepX, diepY, diepAimX * 0.96f - diepAimY * 0.26f, diepAimY * 0.96f + diepAimX * 0.26f, dmg * 0.8f);
      diepSpawnBullet(true, diepX, diepY, diepAimX * 0.96f + diepAimY * 0.26f, diepAimY * 0.96f - diepAimX * 0.26f, dmg * 0.8f);
    }
    diepShootCd = (diepLevel >= 2) ? 7 : 10;
  }

  for (int i = 0; i < DIEP_BULLETS; i++) {
    DiepBullet& b = diepBullets[i];
    if (!b.active) continue;
    b.x += b.vx; b.y += b.vy; b.life--;
    if (b.life <= 0) { b.active = false; continue; }

    if (b.fromPlayer) {
      for (int s = 0; s < DIEP_SHAPES; s++) {
        if (!diepShapes[s].active) continue;
        float dx = diepShapes[s].x - b.x, dy = diepShapes[s].y - b.y;
        float r = diepShapes[s].radius + 4.0f;
        if (dx * dx + dy * dy <= r * r) {
          diepShapes[s].hp -= b.dmg;
          b.active = false;
          if (diepShapes[s].hp <= 0.0f) {
            diepXp += (diepShapes[s].kind == 0) ? 15 : (diepShapes[s].kind == 1 ? 35 : 90);
            diepLevel = 1 + (diepXp / 120);
            diepMaxHp = 100.0f + diepLevel * 12.0f;
            diepShapes[s].x = diepX + randRange(-400.0f, 400.0f);
            diepShapes[s].y = diepY + randRange(-400.0f, 400.0f);
            diepShapes[s].hp = diepShapes[s].maxHp;
          }
          break;
        }
      }
      if (!b.active) continue;
      for (int e = 0; e < DIEP_ENEMIES; e++) {
        if (!diepEnemies[e].active) continue;
        float dx = diepEnemies[e].x - b.x, dy = diepEnemies[e].y - b.y;
        if (dx * dx + dy * dy <= 16.0f * 16.0f) {
          diepEnemies[e].hp -= b.dmg;
          b.active = false;
          if (diepEnemies[e].hp <= 0.0f) {
            diepXp += 120;
            diepLevel = 1 + (diepXp / 120);
            int d = nextRand() & 15;
            diepEnemies[e].x = diepX + DX16[d] * 320.0f;
            diepEnemies[e].y = diepY + DY16[d] * 320.0f;
            diepEnemies[e].hp = 85.0f;
          }
          break;
        }
      }
    } else {
      float dx = diepX - b.x, dy = diepY - b.y;
      if (dx * dx + dy * dy <= 15.0f * 15.0f) {
        diepHp -= b.dmg;
        b.active = false;
        if (diepHp <= 0.0f) diepOver = true;
      }
    }
  }

  for (int s = 0; s < DIEP_SHAPES; s++) {
    diepShapes[s].x += diepShapes[s].vx;
    diepShapes[s].y += diepShapes[s].vy;
    float dx = diepShapes[s].x - diepX, dy = diepShapes[s].y - diepY;
    if (dx * dx + dy * dy > 480.0f * 480.0f) {
      diepShapes[s].x = diepX + randRange(-380.0f, 380.0f);
      diepShapes[s].y = diepY + randRange(-380.0f, 380.0f);
    }
  }

  for (int e = 0; e < DIEP_ENEMIES; e++) {
    DiepEnemy& en = diepEnemies[e];
    float dx = diepX - en.x, dy = diepY - en.y;
    float d = fastSqrt(dx * dx + dy * dy);
    if (d > 1.0f) {
      en.aimX = dx / d; en.aimY = dy / d;
      float moveDir = (d > 120.0f) ? 1.4f : -0.8f;
      en.x += en.aimX * moveDir;
      en.y += en.aimY * moveDir;
    }
    en.shootCd--;
    if (en.shootCd <= 0 && d < 240.0f) {
      diepSpawnBullet(false, en.x, en.y, en.aimX, en.aimY, 10.0f);
      en.shootCd = 38 + (nextRand() % 20);
    }
  }
}

void Game::renderDiepBand(int y0) {
  bandClear(C_BG_LIGHT);
  int ox = ((W / 2) - (int)diepX) % 24; if (ox < 0) ox += 24;
  int oy = ((H / 2) - (int)diepY) % 24; if (oy < 0) oy += 24;
  for (int x = ox; x < W; x += 24) bandFillRect(y0, x, y0, 1, BAND_H, C_GRID);
  for (int y = oy; y < H; y += 24) bandFillRect(y0, 0, y, W, 1, C_GRID);

  for (int s = 0; s < DIEP_SHAPES; s++) {
    int sx = (W / 2) + (int)(diepShapes[s].x - diepX);
    int sy = (H / 2) + (int)(diepShapes[s].y - diepY);
    int r = (int)diepShapes[s].radius;
    if (diepShapes[s].kind == 0) {
      bandFillRect(y0, sx - r, sy - r, r * 2, r * 2, C_BLACK);
      bandFillRect(y0, sx - r + 2, sy - r + 2, r * 2 - 4, r * 2 - 4, C_GOLD);
    } else if (diepShapes[s].kind == 1) {
      bandDrawDisc(y0, sx, sy, r, C_RED, C_BLACK, 2);
    } else {
      bandDrawDisc(y0, sx, sy, r, C_PURPLE, C_BLACK, 2);
    }
  }

  for (int i = 0; i < DIEP_BULLETS; i++) {
    if (!diepBullets[i].active) continue;
    int sx = (W / 2) + (int)(diepBullets[i].x - diepX);
    int sy = (H / 2) + (int)(diepBullets[i].y - diepY);
    bandDrawDisc(y0, sx, sy, 4, diepBullets[i].fromPlayer ? C_BLUE : C_RED, C_BLACK, 1);
  }

  for (int e = 0; e < DIEP_ENEMIES; e++) {
    int sx = (W / 2) + (int)(diepEnemies[e].x - diepX);
    int sy = (H / 2) + (int)(diepEnemies[e].y - diepY);
    bandDrawLine(y0, sx, sy, sx + (int)(diepEnemies[e].aimX * 18.0f), sy + (int)(diepEnemies[e].aimY * 18.0f), 7, C_GRAY);
    bandDrawDisc(y0, sx, sy, 12, C_RED, C_BLACK, 2);
  }

  int px = W / 2, py = H / 2;
  bandDrawLine(y0, px, py, px + (int)(diepAimX * 19.0f), py + (int)(diepAimY * 19.0f), 8, C_GRAY);
  bandDrawDisc(y0, px, py, 13, C_BLUE, C_BLUE_DK, 2);

  bandFillRect(y0, 6, 6, 128, 16, C_DARK);
  bandDrawText(y0, 10, 10, "LVL:", 1, C_WHITE);
  bandDrawInt(y0, 38, 10, diepLevel, 1, C_GOLD);
  bandDrawText(y0, 62, 10, "XP:", 1, C_WHITE);
  bandDrawInt(y0, 84, 10, diepXp, 1, C_CYAN);

  int hpW = (int)((diepHp / diepMaxHp) * 90.0f);
  if (hpW < 0) hpW = 0;
  bandFillRect(y0, 222, 6, 92, 14, C_DARK);
  bandFillRect(y0, 223, 7, hpW, 12, C_GREEN);

  if (diepOver) {
    bandFillRect(y0, 60, 74, 200, 92, C_DARK);
    bandDrawText(y0, 104, 92, "GAME OVER", 2, C_RED);
    bandDrawText(y0, 98, 130, "OK : REJOUER", 2, C_WHITE);
  }
}

void Game::initHole() {
  holeOver = false;
  holeX = 0.0f; holeY = 0.0f; holeR = 16.0f;
  holeCamX = 0.0f; holeCamY = 0.0f; holeZoom = 1.0f;
  holeScore = 0;
  holeTimer = 60 * 28;

  const uint16_t pCols[5] = { C_GREEN, C_GOLD, C_BLUE, C_ORANGE, C_RED };
  for (int i = 0; i < HOLE_PROPS; i++) {
    int kind = (i < 35) ? 0 : ((i < 62) ? 1 : 2);
    float sz = (kind == 0) ? randRange(5.0f, 9.0f) : ((kind == 1) ? randRange(11.0f, 17.0f) : randRange(20.0f, 28.0f));
    holeProps[i] = { true, randRange(-520.0f, 520.0f), randRange(-520.0f, 520.0f), sz, kind, pCols[i % 5] };
  }
  const uint16_t bRims[4] = { C_RED, C_PURPLE, C_GOLD, C_CYAN };
  for (int i = 0; i < HOLE_BOTS; i++) {
    int d = nextRand() & 15;
    holeBots[i] = { true, DX16[d] * 260.0f, DY16[d] * 260.0f, randRange(13.0f, 28.0f), -DX16[d], -DY16[d], 30 + i * 7, bRims[i % 4] };
  }
}

void Game::updateHole() {
  EADK::Keyboard::State k = EADK::Keyboard::scan();
  if (holeOver) {
    if (keyCd == 0 && (k.keyDown(EADK::Keyboard::Key::OK) || k.keyDown(EADK::Keyboard::Key::EXE))) {
      keyCd = 12;
      initHole();
    }
    return;
  }

  if (holeTimer > 0) holeTimer--;
  if (holeTimer <= 0) { holeOver = true; return; }

  float ix = 0.0f, iy = 0.0f;
  if (k.keyDown(EADK::Keyboard::Key::Left)) ix -= 1.0f;
  if (k.keyDown(EADK::Keyboard::Key::Right)) ix += 1.0f;
  if (k.keyDown(EADK::Keyboard::Key::Up)) iy -= 1.0f;
  if (k.keyDown(EADK::Keyboard::Key::Down)) iy += 1.0f;
  if (ix != 0.0f && iy != 0.0f) { ix *= 0.7071f; iy *= 0.7071f; }

  float spd = 2.6f + holeR * 0.02f;
  holeX += ix * spd;
  holeY += iy * spd;
  holeCamX += (holeX - holeCamX) * 0.16f;
  holeCamY += (holeY - holeCamY) * 0.16f;

  float tz = 24.0f / (16.0f + holeR * 0.65f);
  if (tz < 0.45f) tz = 0.45f;
  if (tz > 1.15f) tz = 1.15f;
  holeZoom += (tz - holeZoom) * 0.08f;

  for (int i = 0; i < HOLE_PROPS; i++) {
    HoleProp& pr = holeProps[i];
    float dx = pr.x - holeX, dy = pr.y - holeY;
    if (dx * dx + dy * dy > 680.0f * 680.0f) {
      pr.x = holeX + randRange(-460.0f, 460.0f);
      pr.y = holeY + randRange(-460.0f, 460.0f);
    }
    if (holeR > pr.size * 1.15f && (dx * dx + dy * dy) <= (holeR * 0.85f) * (holeR * 0.85f)) {
      holeScore += (int)(pr.size * 2.0f);
      holeR += pr.size * 0.09f;
      if (holeR > 58.0f) holeR = 58.0f;
      pr.x = holeX + randRange(-460.0f, 460.0f);
      pr.y = holeY + randRange(-460.0f, 460.0f);
    }
  }

  for (int i = 0; i < HOLE_BOTS; i++) {
    HoleBot& b = holeBots[i];
    b.aiCd--;
    if (b.aiCd <= 0) {
      b.aiCd = 35 + (nextRand() % 40);
      int d = nextRand() & 15;
      b.dirX = DX16[d]; b.dirY = DY16[d];
    }
    b.x += b.dirX * 1.6f; b.y += b.dirY * 1.6f;
    float dx = b.x - holeX, dy = b.y - holeY;
    float d2 = dx * dx + dy * dy;
    if (d2 > 680.0f * 680.0f) {
      int d = nextRand() & 15;
      b.x = holeX + DX16[d] * 280.0f;
      b.y = holeY + DY16[d] * 280.0f;
      b.radius = randRange(12.0f, holeR * 0.95f + 8.0f);
    } else if (holeR > b.radius * 1.15f && d2 <= (holeR * 0.85f) * (holeR * 0.85f)) {
      holeScore += 50;
      holeR += 2.2f;
      if (holeR > 58.0f) holeR = 58.0f;
      int d = nextRand() & 15;
      b.x = holeX + DX16[d] * 320.0f;
      b.y = holeY + DY16[d] * 320.0f;
      b.radius = randRange(12.0f, holeR * 0.85f + 6.0f);
    }
  }
}

void Game::renderHoleBand(int y0) {
  bandClear(C_ROAD);
  int step = (int)(64.0f * holeZoom);
  if (step < 14) step = 14;
  int ox = ((W / 2) - (int)(holeCamX * holeZoom)) % step; if (ox < 0) ox += step;
  int oy = ((H / 2) - (int)(holeCamY * holeZoom)) % step; if (oy < 0) oy += step;
  for (int x = ox; x < W; x += step) bandFillRect(y0, x, y0, 2, BAND_H, C_DARK_HI);
  for (int y = oy; y < H; y += step) bandFillRect(y0, 0, y, W, 2, C_DARK_HI);

  int hsx = (W / 2) + (int)((holeX - holeCamX) * holeZoom);
  int hsy = (H / 2) + (int)((holeY - holeCamY) * holeZoom);
  int hsr = (int)(holeR * holeZoom); if (hsr < 6) hsr = 6;
  bandDrawDisc(y0, hsx, hsy, hsr + 3, C_GOLD, C_GOLD, 0);
  bandDrawDisc(y0, hsx, hsy, hsr, C_BLACK, C_BLACK, 0);

  for (int i = 0; i < HOLE_PROPS; i++) {
    HoleProp& pr = holeProps[i];
    int sx = (W / 2) + (int)((pr.x - holeCamX) * holeZoom);
    int sy = (H / 2) + (int)((pr.y - holeCamY) * holeZoom);
    int psz = (int)(pr.size * holeZoom); if (psz < 3) psz = 3;
    float dx = pr.x - holeX, dy = pr.y - holeY;
    if (holeR > pr.size * 1.15f && (dx * dx + dy * dy) < (holeR * holeR)) continue;
    bandFillRect(y0, sx - psz / 2, sy - psz / 2, psz, psz, pr.col);
  }

  for (int i = 0; i < HOLE_BOTS; i++) {
    HoleBot& b = holeBots[i];
    int sx = (W / 2) + (int)((b.x - holeCamX) * holeZoom);
    int sy = (H / 2) + (int)((b.y - holeCamY) * holeZoom);
    int br = (int)(b.radius * holeZoom); if (br < 5) br = 5;
    bandDrawDisc(y0, sx, sy, br + 2, b.rim, b.rim, 0);
    bandDrawDisc(y0, sx, sy, br, C_BLACK, C_BLACK, 0);
  }

  bandFillRect(y0, 6, 6, 112, 16, C_DARK);
  bandDrawText(y0, 12, 10, "SCORE:", 1, C_WHITE);
  bandDrawInt(y0, 52, 10, holeScore, 1, C_GOLD);

  bandFillRect(y0, 244, 6, 70, 16, C_DARK);
  bandDrawText(y0, 250, 10, "T:", 1, C_WHITE);
  bandDrawInt(y0, 266, 10, holeTimer / 28, 1, C_CYAN);

  if (holeOver) {
    bandFillRect(y0, 52, 70, 216, 100, C_DARK);
    bandDrawText(y0, 108, 86, "TEMPS ECOULE", 2, C_GOLD);
    bandDrawText(y0, 88, 114, "SCORE:", 1, C_WHITE);
    bandDrawInt(y0, 130, 114, holeScore, 1, C_CYAN);
    bandDrawText(y0, 96, 138, "OK : REJOUER", 2, C_WHITE);
  }
}

void Game::initPaper() {
  paperOver = false;
  for (int i = 0; i < P_ROWS * P_COLS; i++) {
    paperOwner[i] = 0;
    paperTrail[i] = 0;
  }
  paperGx = 6; paperGy = 6; paperDx = 1; paperDy = 0;
  for (int r = 4; r <= 8; r++) {
    for (int c = 4; c <= 8; c++) {
      paperOwner[r * P_COLS + c] = 1;
    }
  }

  const int bStartX[P_BOTS] = { 32, 32, 18, 20 };
  const int bStartY[P_BOTS] = { 6, 19, 12, 5 };
  const int bDx[P_BOTS]     = { -1, -1, 0, 1 };
  const int bDy[P_BOTS]     = { 0, 0, 1, 0 };
  for (int b = 0; b < P_BOTS; b++) {
    uint8_t id = (uint8_t)(2 + b);
    paperBots[b] = { true, bStartX[b], bStartY[b], bDx[b], bDy[b], 4 + b * 2 };
    for (int r = bStartY[b] - 1; r <= bStartY[b] + 1; r++) {
      for (int c = bStartX[b] - 1; c <= bStartX[b] + 1; c++) {
        if (r >= 0 && r < P_ROWS && c >= 0 && c < P_COLS) {
          paperOwner[r * P_COLS + c] = id;
        }
      }
    }
  }
  paperStepTick = 0;
}

void Game::paperClearPlayer(uint8_t id) {
  for (int i = 0; i < P_ROWS * P_COLS; i++) {
    if (paperOwner[i] == id) paperOwner[i] = 0;
    if (paperTrail[i] == id) paperTrail[i] = 0;
  }
}

void Game::paperCapture(uint8_t id) {
  for (int i = 0; i < P_ROWS * P_COLS; i++) {
    if (paperTrail[i] == id) {
      paperOwner[i] = id;
      paperTrail[i] = 0;
    }
  }
  uint8_t visited[P_ROWS * P_COLS];
  for (int i = 0; i < P_ROWS * P_COLS; i++) visited[i] = 0;

  int qx[P_ROWS * P_COLS];
  int qy[P_ROWS * P_COLS];
  int head = 0, tail = 0;

  for (int c = 0; c < P_COLS; c++) {
    if (paperOwner[c] != id && !visited[c]) { visited[c] = 1; qx[tail] = c; qy[tail] = 0; tail++; }
    int bIdx = (P_ROWS - 1) * P_COLS + c;
    if (paperOwner[bIdx] != id && !visited[bIdx]) { visited[bIdx] = 1; qx[tail] = c; qy[tail] = P_ROWS - 1; tail++; }
  }
  for (int r = 0; r < P_ROWS; r++) {
    int lIdx = r * P_COLS;
    if (paperOwner[lIdx] != id && !visited[lIdx]) { visited[lIdx] = 1; qx[tail] = 0; qy[tail] = r; tail++; }
    int rIdx = r * P_COLS + (P_COLS - 1);
    if (paperOwner[rIdx] != id && !visited[rIdx]) { visited[rIdx] = 1; qx[tail] = P_COLS - 1; qy[tail] = r; tail++; }
  }

  const int ndx[4] = { 1, -1, 0, 0 };
  const int ndy[4] = { 0, 0, 1, -1 };
  while (head < tail) {
    int cx = qx[head], cy = qy[head];
    head++;
    for (int d = 0; d < 4; d++) {
      int nx = cx + ndx[d], ny = cy + ndy[d];
      if (nx < 0 || nx >= P_COLS || ny < 0 || ny >= P_ROWS) continue;
      int idx = ny * P_COLS + nx;
      if (!visited[idx] && paperOwner[idx] != id) {
        visited[idx] = 1;
        qx[tail] = nx; qy[tail] = ny; tail++;
      }
    }
  }

  for (int i = 0; i < P_ROWS * P_COLS; i++) {
    if (!visited[i]) paperOwner[i] = id;
  }
}

int Game::paperCountTiles(uint8_t id) const {
  int n = 0;
  for (int i = 0; i < P_ROWS * P_COLS; i++) if (paperOwner[i] == id) n++;
  return n;
}

void Game::updatePaper() {
  EADK::Keyboard::State k = EADK::Keyboard::scan();
  if (paperOver) {
    if (keyCd == 0 && (k.keyDown(EADK::Keyboard::Key::OK) || k.keyDown(EADK::Keyboard::Key::EXE))) {
      keyCd = 12;
      initPaper();
    }
    return;
  }

  int ndx = paperDx, ndy = paperDy;
  if (k.keyDown(EADK::Keyboard::Key::Left)  && paperDx != 1)  { ndx = -1; ndy = 0; }
  if (k.keyDown(EADK::Keyboard::Key::Right) && paperDx != -1) { ndx = 1;  ndy = 0; }
  if (k.keyDown(EADK::Keyboard::Key::Up)    && paperDy != 1)  { ndx = 0;  ndy = -1; }
  if (k.keyDown(EADK::Keyboard::Key::Down)  && paperDy != -1) { ndx = 0;  ndy = 1; }
  paperDx = ndx; paperDy = ndy;

  paperStepTick++;
  if (paperStepTick < 4) return;
  paperStepTick = 0;

  int nx = paperGx + paperDx;
  int ny = paperGy + paperDy;
  if (nx < 0 || nx >= P_COLS || ny < 0 || ny >= P_ROWS) {
    paperOver = true;
    return;
  }
  paperGx = nx; paperGy = ny;
  int idx = ny * P_COLS + nx;

  if (paperTrail[idx] == 1) {
    paperOver = true;
    return;
  }
  if (paperTrail[idx] >= 2) {
    uint8_t victim = paperTrail[idx];
    paperClearPlayer(victim);
    for (int b = 0; b < P_BOTS; b++) {
      if ((uint8_t)(2 + b) == victim) {
        paperBots[b].gx = 32;
        paperBots[b].gy = (b % 2 == 0) ? 6 : 19;
        paperBots[b].dx = -1;
        paperBots[b].dy = 0;
        paperOwner[paperBots[b].gy * P_COLS + paperBots[b].gx] = victim;
      }
    }
  }

  if (paperOwner[idx] == 1) {
    paperCapture(1);
  } else {
    paperTrail[idx] = 1;
  }

  for (int b = 0; b < P_BOTS; b++) {
    PaperBot& bot = paperBots[b];
    uint8_t id = (uint8_t)(2 + b);
    bot.turnCd--;
    if (bot.turnCd <= 0) {
      bot.turnCd = 3 + (nextRand() % 5);
      int t = nextRand() % 3;
      if (t == 1) { int tmp = bot.dx; bot.dx = -bot.dy; bot.dy = tmp; }
      else if (t == 2) { int tmp = bot.dx; bot.dx = bot.dy; bot.dy = -tmp; }
    }
    int bx = bot.gx + bot.dx;
    int by = bot.gy + bot.dy;
    if (bx < 1 || bx >= P_COLS - 1 || by < 1 || by >= P_ROWS - 1) {
      bot.dx = -bot.dx; bot.dy = -bot.dy;
      bx = bot.gx + bot.dx; by = bot.gy + bot.dy;
    }
    bot.gx = bx; bot.gy = by;
    int bIdx = by * P_COLS + bx;

    if (paperTrail[bIdx] == id) {
      paperClearPlayer(id);
      bot.gx = 32; bot.gy = (b % 2 == 0) ? 6 : 19;
      bot.dx = -1; bot.dy = 0;
      paperOwner[bot.gy * P_COLS + bot.gx] = id;
      continue;
    }
    if (paperTrail[bIdx] == 1) {
      paperOver = true;
      return;
    }
    if (paperOwner[bIdx] == id) {
      paperCapture(id);
    } else {
      paperTrail[bIdx] = id;
    }
  }
}

void Game::renderPaperBand(int y0) {
  bandClear(C_BG_LIGHT);
  const int cellW = 8;
  const int cellH = 8;
  const int topY = 24;

  const uint16_t pCols[6] = { C_BG_LIGHT, C_GREEN, C_RED, C_BLUE, C_PURPLE, C_ORANGE };
  const uint16_t tCols[6] = { C_BG_LIGHT, C_GREEN_DK, C_PINK, C_CYAN, C_GOLD, C_RED };

  for (int r = 0; r < P_ROWS; r++) {
    int py = topY + r * cellH;
    if (py + cellH <= y0 || py >= y0 + BAND_H) continue;
    for (int c = 0; c < P_COLS; c++) {
      int px = c * cellW;
      int idx = r * P_COLS + c;
      uint8_t tr = paperTrail[idx];
      uint8_t ow = paperOwner[idx];
      uint16_t col = tr ? tCols[tr] : pCols[ow];
      bandFillRect(y0, px, py, cellW - 1, cellH - 1, col);
    }
  }

  bandFillRect(y0, paperGx * cellW - 1, topY + paperGy * cellH - 1, cellW + 1, cellH + 1, C_BLACK);
  bandFillRect(y0, paperGx * cellW,     topY + paperGy * cellH,     cellW - 1, cellH - 1, C_WHITE);

  for (int b = 0; b < P_BOTS; b++) {
    PaperBot& bot = paperBots[b];
    bandFillRect(y0, bot.gx * cellW - 1, topY + bot.gy * cellH - 1, cellW + 1, cellH + 1, C_BLACK);
  }

  int pct = (paperCountTiles(1) * 100) / (P_ROWS * P_COLS);
  bandFillRect(y0, 6, 4, 110, 16, C_DARK);
  bandDrawText(y0, 12, 8, "ZONE:", 1, C_WHITE);
  bandDrawInt(y0, 46, 8, pct, 1, C_GREEN);
  bandDrawChar(y0, 70, 8, '%', 1, C_GREEN);

  if (paperOver) {
    bandFillRect(y0, 56, 72, 208, 96, C_DARK);
    bandDrawText(y0, 104, 88, "ELIMINE", 2, C_RED);
    bandDrawText(y0, 96, 126, "OK : REJOUER", 2, C_WHITE);
  }
}

void Game::run() {
  initAgar();
  initDiep();
  initHole();
  initPaper();

  while (!exitApp) {
    if (keyCd > 0) keyCd--;
    handleGlobalBack();

    if (mode == MODE_MENU) {
      updateMenu();
      if (exitApp) break;
    } else if (mode == MODE_AGAR) {
      updateAgar();
    } else if (mode == MODE_DIEP) {
      updateDiep();
    } else if (mode == MODE_HOLE) {
      updateHole();
    } else if (mode == MODE_PAPER) {
      updatePaper();
    }

    for (int y0 = 0; y0 < H; y0 += BAND_H) {
      if (mode == MODE_MENU) {
        renderMenuBand(y0);
        if (quitOpen) renderQuitBand(y0);
      } else if (mode == MODE_AGAR) {
        renderAgarBand(y0);
      } else if (mode == MODE_DIEP) {
        renderDiepBand(y0);
      } else if (mode == MODE_HOLE) {
        renderHoleBand(y0);
      } else if (mode == MODE_PAPER) {
        renderPaperBand(y0);
      }
      flushBand(y0);
    }

    tick++;
    EADK::Timing::msleep(28);
  }
}
