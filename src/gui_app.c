#include "gui_app.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

// Screen dimension constants
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;

#define SDL_COLOUR(colour) (((colour) >> 3 * 8) & 0xFF), (((colour) >> 2 * 8) & 0xFF), (((colour) >> 1 * 8) & 0xFF), (((colour) >> 0 * 8) & 0xFF)
#define BACKGROUND_COLOUR 0x1E1E1EFF
#define FOREGROUND_COLOUR 0xFFFFFFFF
#define LARGE_CIRCLE_RADIUS 30
#define LARGE_CIRCLE_THICKNESS 5
#define SMALL_CIRCLE_RADIUS 15
#define SMALL_CIRCLE_THICKNESS 2
#define PADDING 20
#define SMALL_PADDING 5

static void draw_empty_circle(SDL_Renderer *renderer, int x, int y, int radius, int thickness) {
  SDL_SetRenderDrawColor(renderer, SDL_COLOUR(FOREGROUND_COLOUR));
  for (int h = 0; h < 2 * radius; h++) {
    for (int w = 0; w < 2 * radius; w++) {
      int dy = h - radius;
      int dx = w - radius;
      if ((dx*dx + dy*dy < radius*radius) && 
          (dx*dx + dy*dy >= (radius - thickness)*(radius - thickness))) {
        SDL_RenderDrawPoint(renderer, x + dx, y + dy);
      }
    }
  }
}

int gui_main(void) {
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer) < 0) {
    fprintf(stderr, "window could not be created! SDL_Error: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  // Hack to get window to stay up
  SDL_Event e;
  bool quit = false;
  while (quit == false)
  {
    while (SDL_PollEvent(&e))
    {
      if (e.type == SDL_QUIT)
        quit = true;
    }

    if (SDL_SetRenderDrawColor(renderer, SDL_COLOUR(BACKGROUND_COLOUR)) < 0) {
      fprintf(stderr, "Render draw colour error! SDL_Error: %s\n", SDL_GetError());
    }

    if (SDL_RenderClear(renderer) < 0) {
      fprintf(stderr, "Render clear error! SDL_Error: %s\n", SDL_GetError());
    }

  // TODO magic numbers
  int circles_offset_width = SCREEN_WIDTH / 2 - ((5 * PADDING + 4 * (2 * LARGE_CIRCLE_RADIUS)) / 2);
  int circles_offset_height = SCREEN_HEIGHT / 2 - ((9 * PADDING + 10 * (2 * LARGE_CIRCLE_RADIUS)) / 2);
  for (int j = 0; j < 10; j++) {
    for (int i = 0; i < 4; i++) {
      draw_empty_circle(renderer, (i * (2 * LARGE_CIRCLE_RADIUS + PADDING) + LARGE_CIRCLE_RADIUS + circles_offset_width), 
                        (j * (2 * LARGE_CIRCLE_RADIUS + PADDING) + LARGE_CIRCLE_RADIUS + circles_offset_height), LARGE_CIRCLE_RADIUS, LARGE_CIRCLE_THICKNESS);
    }

    int mini_circles_offset_width = 4 * (2 * LARGE_CIRCLE_RADIUS + PADDING) + LARGE_CIRCLE_RADIUS + PADDING + circles_offset_width;
    int mini_circles_offset_height = (j * (2 * LARGE_CIRCLE_RADIUS + PADDING) + SMALL_CIRCLE_RADIUS + circles_offset_height);
    for (int i = 0; i < 2; i++) {
      for (int k = 0; k < 2; k++) {
        draw_empty_circle(renderer, i * (2 * SMALL_CIRCLE_RADIUS + SMALL_PADDING) + mini_circles_offset_width, 
                          k * (2 * SMALL_CIRCLE_RADIUS + SMALL_PADDING) + mini_circles_offset_height, SMALL_CIRCLE_RADIUS, SMALL_CIRCLE_THICKNESS);
      }
    }

  }
    SDL_RenderPresent(renderer);
  }

  // Destroy window
  SDL_DestroyWindow(window);

  // Quit SDL subsystems
  SDL_Quit();

  return EXIT_SUCCESS;
}