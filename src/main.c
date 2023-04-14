#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "game_logic.h"

#define MAXIMUM_NUMBER_OF_TRIES 12

static const struct {
  char character;
  game_logic_values_t game_input_value;
} conversion_table[] = {
  {'A', GAME_VALUE_ONE},
  {'B', GAME_VALUE_TWO},
  {'C', GAME_VALUE_THREE},
  {'D', GAME_VALUE_FOUR},
  {'E', GAME_VALUE_FIVE},
  {'F', GAME_VALUE_SIX},
};

static void convert_string_input_to_game_input(const char *char_buffer, game_logic_values_t *game_buffer) {
  for (uint_fast8_t i = 0; i < NUMBER_OF_VALUES_TO_GUESS; i++) {
    for (uint_fast8_t j = 0; j < GAME_VALUE_MAX; j++) {
      if (conversion_table[j].character == char_buffer[i]) {
        game_buffer[i] = conversion_table[j].game_input_value;
        break;
      }
    }
  }
}

static void print_game_buffer(const game_logic_values_t *game_buffer) {
  for (uint_fast8_t i = 0; i < NUMBER_OF_VALUES_TO_GUESS; i++) {
    printf("%d ", game_buffer[i]+1);
  }
  printf("\n");
}

int main(void) {
  srand(time(NULL));
  game_logic_generate_random_answer();
  game_logic_feedback_t feedback = {0};
  uint8_t tries = 0;

  printf("Start guessing?\n");
  printf("Enter 4 values ranging from A-F?\n");
  printf("NOTE: The value can contain duplicates E.g. AAFB\n");
  printf("For ever correct value a - will appear\n");
  printf("For ever correct value and correct placement in the order a + will appear\n");
  printf("\n\n");
  
  while (!feedback.is_guess_correct) {
    if (tries == MAXIMUM_NUMBER_OF_TRIES) {
      printf("Oh No! You Failed To Guess The Correct Answer In 12 Goes!\n");
      printf("Still Have A Nice Day!\n");
      return EXIT_SUCCESS;
    }

    char char_buffer[NUMBER_OF_VALUES_TO_GUESS + 2];  // +2 for null terminator and newline
    fgets(char_buffer, sizeof(char_buffer), stdin);
    tries++;

    game_logic_values_t game_buffer[NUMBER_OF_VALUES_TO_GUESS];
    convert_string_input_to_game_input(char_buffer, game_buffer);
    // print_game_buffer(game_buffer);

    feedback = game_logic_get_feedback(game_buffer);
    for (uint_fast8_t i = 0; i < feedback.number_of_correct_value_only; i++) {
      printf("-");
    }

    for (uint_fast8_t i = 0; i < feedback.number_of_correct_value_and_placement; i++) {
      printf("+");
    }

    printf("\n");
  }

  printf("Well Done! You Guessed it %d tries!\n", tries);
  printf("Have A Nice Day!\n");
  return EXIT_SUCCESS;
}