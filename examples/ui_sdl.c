#include <GL/gl.h>
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ui.h"
#include "zbuffer.h"

static int g_xpos, g_ypos;     // 窗口位置
static int g_width, g_height;  // 窗口大小

static ZBuffer *g_frameBuffer;  // ZBuffer 句柄， TinyGL 的绘制缓冲区

static SDL_Window *g_window;      // 窗口句柄
static SDL_Renderer *g_renderer;  // 渲染器句柄
static SDL_Surface *g_surface;    // 表面句柄
static SDL_Texture *g_texture;    // 纹理句柄

// #define RENEW_TEXTURE 1

#define PACK8888(a, b, c, d) (((a) << 24) | ((b) << 16) | ((c) << 8) | d)

void tkSwapBuffers(void) {
  // 将 g_frameBuffer 的内容拷贝到 g_surface 中
  if (g_frameBuffer->linesize == g_surface->pitch) {
    // 像素宽度一样，直接拷贝
    ZB_copyFrameBuffer(g_frameBuffer, g_surface->pixels, g_surface->pitch);
  } else if (g_frameBuffer->xsize * 3 == g_frameBuffer->linesize) {
    // convert RGB888 to XRGB8888
    for (int y = 0; y < g_frameBuffer->ysize; y++) {
      uint8_t *srcRow = ((uint8_t *)g_frameBuffer->pbuf) + y * g_frameBuffer->linesize;
      uint8_t *dstRow = ((uint8_t *)g_surface->pixels) + y * g_surface->pitch;
      for (int x = 0; x < g_frameBuffer->xsize; x++) {
        uint32_t *srcPixel = (uint32_t *)(srcRow + x * 3);
        uint32_t *dstPixel = (uint32_t *)(dstRow + x * 4);
        uint8_t colors[3] = {0, 0, 0};
        memcpy(colors, srcPixel, sizeof(colors));
        *dstPixel = PACK8888(0, colors[0], colors[1], colors[2]);
      }
    }
  } else if (g_frameBuffer->xsize * 2 == g_frameBuffer->linesize) {
    // convert RGB565 to XRGB8888
    for (int y = 0; y < g_frameBuffer->ysize; y++) {
      uint8_t *srcRow = ((uint8_t *)g_frameBuffer->pbuf) + y * g_frameBuffer->linesize;
      uint8_t *dstRow = ((uint8_t *)g_surface->pixels) + y * g_surface->pitch;
      for (int x = 0; x < g_frameBuffer->xsize; x++) {
        uint16_t *srcPixel = (uint16_t *)(srcRow + x * 2);
        uint32_t *dstPixel = (uint32_t *)(dstRow + x * 4);
        uint16_t colors = 0;  // RGB565
        memcpy(&colors, srcPixel, sizeof(colors));
        uint8_t R5 = (colors >> 11) & 0x1F;
        uint8_t G6 = (colors >> 5) & 0x3F;
        uint8_t B5 = colors & 0x1F;
        uint8_t R8 = (R5 * 527 + 23) >> 6;  // 将5位红色值映射到8位
        uint8_t G8 = (G6 * 259 + 33) >> 6;  // 将6位绿色值映射到8位
        uint8_t B8 = (B5 * 527 + 23) >> 6;  // 将5位蓝色值映射到8位
        *dstPixel = PACK8888(0, R8, G8, B8);
      }
    }
  }

  // g_surface 更新到 g_texture 中
  SDL_UpdateTexture(g_texture, NULL, g_surface->pixels, g_surface->pitch);

  // 渲染纹理到窗口
  SDL_RenderClear(g_renderer);
  SDL_RenderCopy(g_renderer, g_texture, NULL, NULL);
  SDL_RenderPresent(g_renderer);
}

int ui_keycode(const SDL_KeyboardEvent *event) {
  if (!event) return 0;
  switch (event->keysym.scancode) {
    case SDL_SCANCODE_UP:
      return KEY_UP;
    case SDL_SCANCODE_DOWN:
      return KEY_DOWN;
    case SDL_SCANCODE_LEFT:
      return KEY_LEFT;
    case SDL_SCANCODE_RIGHT:
      return KEY_RIGHT;
    case SDL_SCANCODE_ESCAPE:
      return KEY_ESCAPE;
    case SDL_SCANCODE_Z:
      // 这两种清空下，返回的是大写的Z：
      // 1. 大写锁定状态下，没有按SHIFT键；
      // 2. 没有大写锁定状态下，按了SHIFT键；
      if (((event->keysym.mod & KMOD_CAPS) && !(event->keysym.mod & KMOD_SHIFT)) ||
          !(event->keysym.mod & KMOD_CAPS) && (event->keysym.mod & KMOD_SHIFT)) {
        return 'Z';
      }
      return 'z';
    default:
      break;
  }
  return 0;
}

int ui_loop(int argc, char **argv, const char *name) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Failed to initialize SDL\n");
    return -1;
  }

  // 创建窗口
  g_width = argc > 1 ? atoi(argv[1]) : 320;
  g_height = argc > 2 ? atoi(argv[2]) : 240;
  g_window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, g_width,
                              g_height, 0);
  if (!g_window) {
    printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  // 创建渲染器
  g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
  if (!g_renderer) {
    printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    SDL_DestroyWindow(g_window);
    SDL_Quit();
    return 1;
  }

  // 创建 ZBuffer
  // g_frameBuffer = ZB_open(g_width, g_height, ZB_MODE_5R6G5B, 0, NULL, NULL, NULL);
  // g_frameBuffer = ZB_open(g_width, g_height, ZB_MODE_RGB24, 0, NULL, NULL, NULL);
  g_frameBuffer = ZB_open(g_width, g_height, ZB_MODE_RGBA, 0, NULL, NULL, NULL);
  if (!g_frameBuffer) {
    printf("ZB_open failed!\n");
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    SDL_Quit();
    return 1;
  }

  // 创建 SDL_Surface， 后续用于渲染到窗口
  g_surface = SDL_CreateRGBSurfaceWithFormat(0, g_width, g_height, 24, SDL_PIXELFORMAT_RGBX8888);
  if (!g_surface) {
    printf("SDL_CreateRGBSurface Error: %s\n", SDL_GetError());
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    SDL_Quit();
    return 1;
  }
  printf("surface color format: %s\n", SDL_GetPixelFormatName(g_surface->format->format));
  printf("    bytes per pixels: %d\n", g_surface->format->BitsPerPixel);

  // surface 填充为黑色
  memset(g_surface->pixels, 0, g_surface->pitch * g_surface->h);

  // 将 surface 转为纹理
  g_texture = SDL_CreateTextureFromSurface(g_renderer, g_surface);
  if (!g_texture) {
    printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
    SDL_FreeSurface(g_surface);
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    SDL_Quit();
    return 1;
  }

  SDL_RenderClear(g_renderer);
  SDL_RenderCopy(g_renderer, g_texture, NULL, NULL);
  SDL_RenderPresent(g_renderer);

  // 初始化 TinyGL 上下文，TinyGL会使用ZBuffer作为绘制缓冲区
  glInit(g_frameBuffer);

  init();
  reshape(g_width, g_height);

  // 主循环
  SDL_Event event;
  SDL_bool quit = SDL_FALSE;
  uint64_t start = SDL_GetTicks64();
  while (!quit) {
    SDL_Scancode code = SDL_SCANCODE_UNKNOWN;
    int hasEvent = SDL_PollEvent(&event);
    if (hasEvent) {
      int k = 0;
      switch (event.type) {
        case SDL_QUIT:
          quit = SDL_TRUE;
          break;

        case SDL_KEYDOWN:
          code = event.key.keysym.scancode;
          k = ui_keycode(&event.key);
          printf("key down: %x %d\n", k, code);
          break;
      }

      if (k) {
        key(k, 0);
      }
      reshape(g_width, g_height);
    } else {
      idle();
    }
    // uint64_t end = SDL_GetTicks64();
    // uint64_t ticks = end - start;
    // double fps = 1000.0 / ticks;
    // printf("cost: %d, FPS: %.3f\n", ticks, fps);
    // start = end;
  }

  // 清理资源
  ZB_close(g_frameBuffer);

  SDL_DestroyTexture(g_texture);
  SDL_FreeSurface(g_surface);
  SDL_DestroyRenderer(g_renderer);
  SDL_DestroyWindow(g_window);
  SDL_Quit();

  return 0;
}
