#include "gui_app.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "game_logic.h"

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
#define MAXIMUM_NUMBER_OF_TRIES 8

typedef enum {
  RED = 0,
  ORANGE,
  GREEN,
  BLUE,
  PURPLE,
  PINK,
  NONE,
  COLOUR_COUNT
} colour_label_t;

static const int SCREEN_WIDTH = 900;
static const int SCREEN_HEIGHT = 900;

static const int colour_choices[] = {
  [NONE] =   BACKGROUND_COLOUR,
  [RED] =    0xFFADADFF,
  [ORANGE] = 0xFFD6A5FF,
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

static void error_check(bool status, const char *error_str) {
  if (status) {
    fprintf(stderr, "%s SDL_Error: %s\n", error_str, SDL_GetError());
    exit(EXIT_FAILURE);
  }
}

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
            error_check(SDL_SetRenderDrawColor(renderer, SDL_COLOUR(FOREGROUND_COLOUR)) < 0, "Render draw colour error!");
          } else {
            error_check(SDL_SetRenderDrawColor(renderer, SDL_COLOUR(colour)) < 0, "Render draw colour error!");
          }
          error_check(SDL_RenderDrawPoint(renderer, x_center + dx, y_center + dy) < 0, "Render draw point error!");
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
  srand(time(NULL));
  game_logic_generate_random_answer();
  game_logic_feedback_t feedback = {0};
  game_logic_values_t game_buffer[NUMBER_OF_VALUES_TO_GUESS] = {0};

  init_circles();

  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;
  TTF_Font *font = NULL;

  error_check(SDL_Init(SDL_INIT_VIDEO) < 0, "SDL could not initialize!");
  error_check(TTF_Init() < 0, "TTF could not initialize!");
  error_check(SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer) < 0, "window could not be created!");
  
  font = TTF_OpenFont("./Roboto-Medium.ttf", 36);
  error_check(font == NULL, "TTF failed to load font!");

  SDL_Surface *submit_text_surface = TTF_RenderText_Solid( font, "Submit", (SDL_Colour) {SDL_COLOUR(FOREGROUND_COLOUR)});
  error_check(submit_text_surface == NULL, "TTF failed to render text!");
  SDL_Texture *submit_text_texture = SDL_CreateTextureFromSurface(renderer, submit_text_surface);
  error_check(submit_text_texture == NULL, "failed to create text texture from surface!");
  
  SDL_Surface *win_text_surface = TTF_RenderText_Solid(font, "You Win!", (SDL_Colour) {SDL_COLOUR(FOREGROUND_COLOUR)});
  error_check(win_text_surface == NULL, "TTF failed to render text!");
  SDL_Texture *win_text_texture = SDL_CreateTextureFromSurface(renderer, win_text_surface);
  error_check(win_text_texture == NULL, "failed to create text texture from surface!");

  SDL_Surface *lose_text_surface = TTF_RenderText_Solid(font, "You Lose!", (SDL_Colour) {SDL_COLOUR(FOREGROUND_COLOUR)});
  error_check(lose_text_surface == NULL, "TTF failed to render text!");
  SDL_Texture *lose_text_texture = SDL_CreateTextureFromSurface(renderer, lose_text_surface);
  error_check(lose_text_texture == NULL, "failed to create text texture from surface!");

  SDL_Texture *active_text_texture = submit_text_texture;
  int text_x = guess_sets[active_idx].result[1].x + SMALL_CIRCLE_DIAMETER + 4 * PADDING;
  SDL_Rect text_dest = {text_x, (guess_sets[active_idx].result[0].y - SMALL_PADDING), submit_text_surface->w, submit_text_surface->h};

  SDL_Event e;
  bool game_active = true;
  bool redraw = true;
  bool quit = false;
  bool draw_answer = false;
  while (quit == false) {
    if (redraw) {
      error_check(SDL_SetRenderDrawColor(renderer, SDL_COLOUR(BACKGROUND_COLOUR)) < 0, "Render draw colour error!");
      error_check(SDL_RenderClear(renderer) < 0, "Render clear error!");

      for (int j = 0; j < MAXIMUM_NUMBER_OF_TRIES; j++) {
        for (int i = 0; i < NUMBER_OF_VALUES_TO_GUESS; i++) {
          circle_t *circle = &guess_sets[j].guess[i];
          draw_circle(renderer, circle);
          circle = &guess_sets[j].result[i];
          draw_circle(renderer, circle);
        }
      }

      if (draw_answer) {
        for (int i = 0; i < NUMBER_OF_VALUES_TO_GUESS; i++) {
          circle_t circle = {
            .colour = (colour_label_t) game_logic_get_answer()[i],
            .islarge = true,
            .x = guess_sets[MAXIMUM_NUMBER_OF_TRIES - 1].guess[i].x,
            .y = guess_sets[MAXIMUM_NUMBER_OF_TRIES - 1].guess[i].y + LARGE_CIRCLE_DIAMETER + PADDING
          };
          draw_circle(renderer, &circle);
        }
      }

      error_check(SDL_RenderCopy(renderer, active_text_texture, NULL, &text_dest) < 0, "Render copy error!");

      SDL_RenderPresent(renderer);
      redraw = false;
    }

    if (SDL_WaitEvent(&e)) {
      switch (e.type) {
      case SDL_QUIT:
        quit = true;
        break;
      case SDL_MOUSEBUTTONUP:
        if (game_active) {
          if (e.button.button == SDL_BUTTON_LEFT) {
            circle_t *circle = check_mouse_click(e.button.x, e.button.y);
            if (circle != NULL) {
              redraw = true;
              circle->colour = ((int)circle->colour + 1) % COLOUR_COUNT;
            }
            if (SDL_PointInRect(&(SDL_Point) {e.button.x, e.button.y}, &text_dest)) {
              bool is_selection_valid = true;
              for (int i = 0; i < NUMBER_OF_VALUES_TO_GUESS; i++) {
                if (guess_sets[active_idx].guess[i].colour == NONE) is_selection_valid = false;
              }
              if (!is_selection_valid) break;
              redraw = true;
              for (int i = 0; i < NUMBER_OF_VALUES_TO_GUESS; i++) {
                game_buffer[i] = (game_logic_values_t) guess_sets[active_idx].guess[i].colour;
              }
              feedback = game_logic_get_feedback(game_buffer);
              for (int i = 0; i < NUMBER_OF_VALUES_TO_GUESS; i++) {
                if (feedback.number_of_correct_value_and_placement > 0) {
                  feedback.number_of_correct_value_and_placement--;
                  guess_sets[active_idx].result[i].colour = GREEN;
                } else if (feedback.number_of_correct_value_only > 0) {
                  feedback.number_of_correct_value_only--;
                  guess_sets[active_idx].result[i].colour = ORANGE;
                } else {
                  guess_sets[active_idx].result[i].colour = RED;
                }
              }
              if (feedback.is_guess_correct) { 
                active_text_texture = win_text_texture;
                game_active = false;
              } else if (active_idx + 1 == MAXIMUM_NUMBER_OF_TRIES) {
                active_text_texture = lose_text_texture;
                game_active = false;
                draw_answer = true;
              } else {
                active_idx++;
                text_dest.y = guess_sets[active_idx].result[0].y - SMALL_PADDING;
              }
            }
          }
        }
        break;
      default:
        break;
      } 
    }
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_CloseFont(font);
  TTF_Quit();
  SDL_Quit();

  return EXIT_SUCCESS;
}