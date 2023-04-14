#include "game_logic.h"
#include "stdint.h"
#include "random.h"

static game_logic_values_t answer[NUMBER_OF_VALUES_TO_GUESS] = {0};

void game_logic_generate_random_answer(void) {
  for (uint_fast8_t i = 0; i < NUMBER_OF_VALUES_TO_GUESS; i++) {
    answer[i] = (game_logic_values_t) (random_value() % GAME_VALUE_MAX);
  }
}

const game_logic_values_t *game_logic_get_answer(void) {
  return answer;
}

game_logic_feedback_t game_logic_get_feedback(game_logic_values_t *guess)
{
  game_logic_feedback_t feedback = {0};
  feedback.is_guess_correct = true;

  for (uint_fast8_t i = 0; i < NUMBER_OF_VALUES_TO_GUESS; i++) {
    if (guess[i] == answer[i]) {
      feedback.number_of_correct_value_and_placement++;
    } else {
      feedback.is_guess_correct = false;
      for (uint_fast8_t j = 0; j < NUMBER_OF_VALUES_TO_GUESS; j++) {
        if ((i != j) && (guess[i] == answer[j])) {
          feedback.number_of_correct_values_only++;
        }
      }
    }
  }
  return feedback;
}

