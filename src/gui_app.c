#include "gui_app.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "game_logic.h"

// TODO: Only allow submit if all entries at not NONE
//       active_idx limited to 10

// Screen dimension constants
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;

#define SDL_COLOUR(colour) (((colour) >> 3 * 8) & 0xFF), (((colour) >> 2 * 8) & 0xFF), (((colour) >> 1 * 8) & 0xFF), (((colour) >> 0 * 8) & 0xFF)
#define BACKGROUND_COLOUR 0x1E1E1EFF
#define FOREGROUND_COLOUR 0x8E8E8EFF

#define LARGE_CIRCLE_RADIUS 30
#define LARGE_CIRCLE_DIAMETER (2 * LARGE_CIRCLE_RADIUS)
#define LARGE_CIRCLE_THICKNESS 5
#define SMALL_CIRCLE_RADIUS 15
#define SMALL_CIRCLE_DIAMETER (2 * SMALL_CIRCLE_RADIUS)
#define SMALL_CIRCLE_THICKNESS 2
#define PADDING 20
#define SMALL_PADDING 5
#define MAXIMUM_NUMBER_OF_TRIES 10

typedef enum {
  NONE = 0,
  RED,
  ORANGE,
  YELLOW,
  GREEN,
  BLUE,
  PURPLE,
  PINK,
  COLOUR_COUNT
} colour_label_t;

static const int colour_choices[] = {
  [NONE] = BACKGROUND_COLOUR,
  [RED] =    0xFFADADFF,
  [ORANGE] = 0xFFD6A5FF,
  [YELLOW] = 0xFDFFB6FF,
  [GREEN] =  0xCAFFBFFF,
  [BLUE] =   0xA0C4FFFF,
  [PURPLE] = 0xBDB2FFFF,
  [PINK] =   0xFFC6FFFF,
};
typedef struct {
  int x;
  int y;
  colour_label_t colour;
  bool islarge;
} circle_t;

typedef struct {
  circle_t guess[NUMBER_OF_VALUES_TO_GUESS];
  circle_t result[NUMBER_OF_VALUES_TO_GUESS];
} guess_set_t;

static guess_set_t guess_sets[MAXIMUM_NUMBER_OF_TRIES];

static int active_idx = 0;

static void init_circles(void) {
  int circles_offset_width = SCREEN_WIDTH / 2 - (NUMBER_OF_VALUES_TO_GUESS * (LARGE_CIRCLE_DIAMETER + PADDING) + 2 * (SMALL_CIRCLE_DIAMETER + SMALL_PADDING) + PADDING) / 2;
  int circles_offset_height = SCREEN_HEIGHT / 2 - (MAXIMUM_NUMBER_OF_TRIES * (LARGE_CIRCLE_DIAMETER + PADDING) - PADDING) / 2;
  for (int j = 0; j < MAXIMUM_NUMBER_OF_TRIES; j++) {
    for (int i = 0; i < NUMBER_OF_VALUES_TO_GUESS; i++) {
      circle_t *circle = &guess_sets[j].guess[i];
      circle->x = i * (LARGE_CIRCLE_DIAMETER + PADDING) + circles_offset_width;
      circle->y = j * (LARGE_CIRCLE_DIAMETER + PADDING) + circles_offset_height;
      circle->colour = NONE;
      circle->islarge = true;
    }

    int small_circle_offset_width = NUMBER_OF_VALUES_TO_GUESS * (LARGE_CIRCLE_DIAMETER + PADDING) + PADDING + circles_offset_width;
    int small_circle_offset_height = j * (LARGE_CIRCLE_DIAMETER + PADDING) + circles_offset_height;
    for (int i = 0; i < 2; i++) {
      for (int k = 0; k < 2; k++) {
        circle_t *circle = &guess_sets[j].result[i*2 + k];
        circle->x = i * (SMALL_CIRCLE_DIAMETER + SMALL_PADDING) + small_circle_offset_width;
        circle->y = k * (SMALL_CIRCLE_DIAMETER + SMALL_PADDING) + small_circle_offset_height;
        circle->colour = NONE;
        circle->islarge = false;
      }
    }
  }
}

static void draw_circle(SDL_Renderer *renderer, circle_t *circle) {
  int radius = circle->islarge ? LARGE_CIRCLE_RADIUS : SMALL_CIRCLE_RADIUS;
  int thickness = circle->islarge ? LARGE_CIRCLE_THICKNESS : SMALL_CIRCLE_THICKNESS;
  int colour = colour_choices[circle->colour];
  int x_center = circle->x + radius;
  int y_center = circle->y + radius;
  for (int h = 0; h < 2 * radius; h++) {
    for (int w = 0; w < 2 * radius; w++) {
      int dy = h - radius;
      int dx = w - radius;
      if (dx*dx + dy*dy < radius*radius) {
          if (dx*dx + dy*dy >= (radius - thickness)*(radius - thickness)) {
            SDL_SetRenderDrawColor(renderer, SDL_COLOUR(FOREGROUND_COLOUR));
          } else {
            SDL_SetRenderDrawColor(renderer, SDL_COLOUR(colour));
          }
          SDL_RenderDrawPoint(renderer, x_center + dx, y_center + dy);
      }
    }
  }
}

static circle_t* check_mouse_click(int x, int y) {
  for (int i = 0; i < NUMBER_OF_VALUES_TO_GUESS; i++) {
    circle_t *circle = &guess_sets[active_idx].guess[i];
    if (x >= circle->x &&  x <= (circle->x + LARGE_CIRCLE_DIAMETER) &&
        y >= circle->y  && y <= (circle->y + LARGE_CIRCLE_DIAMETER)) {
      int dy = y - (circle->y + LARGE_CIRCLE_RADIUS);
      int dx = x - (circle->x + LARGE_CIRCLE_RADIUS);
      if (dx*dx + dy*dy < LARGE_CIRCLE_RADIUS*LARGE_CIRCLE_RADIUS) {
        return circle;
      }
    }
  }

  return NULL;
}

int gui_main(void) {
  init_circles();

  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;
  TTF_Font *font = NULL;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  if (TTF_Init() < 0) {
    fprintf(stderr, "TTF could not initialize! TFF_Error: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer) < 0) {
    fprintf(stderr, "window could not be created! SDL_Error: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  font = TTF_OpenFont("./Roboto-Medium.ttf", 36);
  if (font == NULL) {
    fprintf(stderr, "TTF failed to load font! TFF_Error: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  SDL_Surface *submit_text_surface = TTF_RenderText_Solid( font, "Submit", (SDL_Colour) {SDL_COLOUR(FOREGROUND_COLOUR)});
  if (submit_text_surface == NULL) {
    fprintf(stderr, "TTF failed to render text! TFF_Error: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }
  
  SDL_Texture *submit_text_texture = SDL_CreateTextureFromSurface(renderer, submit_text_surface);
  if (submit_text_surface == NULL) {
    fprintf(stderr, "failed to create text texture from surface! SDL_Error: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  int text_x = guess_sets[active_idx].result[1].x + SMALL_CIRCLE_DIAMETER + 4 * PADDING;
  SDL_Rect submit_text_dest = {text_x, (guess_sets[active_idx].result[0].y - SMALL_PADDING), submit_text_surface->w, submit_text_surface->h};

  // Hack to get window to stay up
  SDL_Event e;
  bool quit = false;
  while (quit == false) {
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
      case SDL_QUIT:
        quit = true;
        break;
      case SDL_MOUSEBUTTONUP:
        if (e.button.button == SDL_BUTTON_LEFT) {
          circle_t *circle = check_mouse_click(e.button.x, e.button.y);
          if (circle != NULL) {
            circle->colour = ((int)circle->colour + 1) % COLOUR_COUNT;
          }
          if (SDL_PointInRect(&(SDL_Point) {e.button.x, e.button.y}, &submit_text_dest)) {
            active_idx++;
            submit_text_dest.y = guess_sets[active_idx].result[0].y - SMALL_PADDING;
          }
        }
      default:
        break;
      } 
    }

    if (SDL_SetRenderDrawColor(renderer, SDL_COLOUR(BACKGROUND_COLOUR)) < 0) {
      fprintf(stderr, "Render draw colour error! SDL_Error: %s\n", SDL_GetError());
    }

    if (SDL_RenderClear(renderer) < 0) {
      fprintf(stderr, "Render clear error! SDL_Error: %s\n", SDL_GetError());
    }

    for (int j = 0; j < MAXIMUM_NUMBER_OF_TRIES; j++) {
      for (int i = 0; i < NUMBER_OF_VALUES_TO_GUESS; i++) {
        circle_t *circle = &guess_sets[j].guess[i];
        draw_circle(renderer, circle);
        circle = &guess_sets[j].result[i];
        draw_circle(renderer, circle);
      }
    }

    SDL_RenderCopy(renderer, submit_text_texture, NULL, &submit_text_dest);

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_CloseFont(font);
  TTF_Quit();
  SDL_Quit();

  return EXIT_SUCCESS;
}