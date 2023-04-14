#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "random.h"

#define NUMBER_OF_VALUES_TO_GUESS 4

typedef enum {
  GAME_VALUE_ONE,
  GAME_VALUE_TWO,
  GAME_VALUE_THREE,
  GAME_VALUE_FOUR,
  GAME_VALUE_FIVE,
  GAME_VALUE_SIX,
  GAME_VALUE_MAX
} game_logic_values_t;

typedef struct {
  uint8_t number_of_correct_values_only;
  uint8_t number_of_correct_value_and_placement;
  bool is_guess_correct;
} game_logic_feedback_t;

void game_logic_generate_random_answer(void);

const game_logic_values_t *game_logic_get_answer(void);

game_logic_feedback_t game_logic_get_feedback(game_logic_values_t *guess);

#endif /* GAME_LOGIC_H */