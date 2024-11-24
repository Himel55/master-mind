#include "game_logic.h"
#include "stdint.h"
#include "random.h"
#include <string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static game_logic_values_t answer[NUMBER_OF_VALUES_TO_GUESS] = {0};
static uint_fast8_t answer_bins[GAME_VALUE_MAX] = {0};

void game_logic_generate_random_answer(void) {
  memset(answer_bins, 0, sizeof(answer_bins));
  for (uint_fast8_t i = 0; i < NUMBER_OF_VALUES_TO_GUESS; i++) {
    game_logic_values_t value = (game_logic_values_t) (random_value() % GAME_VALUE_MAX);
    answer[i] = value;
    answer_bins[value]++;
  }
}

game_logic_feedback_t game_logic_get_feedback(game_logic_values_t guess[]) {
  game_logic_feedback_t feedback = {0};
  uint_fast8_t guess_bins[GAME_VALUE_MAX] = {0};

  for (uint_fast8_t i = 0; i < NUMBER_OF_VALUES_TO_GUESS; i++) {
    guess_bins[guess[i]]++;
    if (guess[i] == answer[i]) {
      feedback.number_of_correct_value_and_placement++;
    }
  }

  for (uint_fast8_t i = 0; i < GAME_VALUE_MAX; i++) {
    feedback.number_of_correct_value_only += MIN(guess_bins[i], answer_bins[i]);
  }

  feedback.number_of_correct_value_only -= feedback.number_of_correct_value_and_placement;
  feedback.is_guess_correct = (bool)(feedback.number_of_correct_value_and_placement == NUMBER_OF_VALUES_TO_GUESS);
  return feedback;
}
