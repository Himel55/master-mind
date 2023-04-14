#include "unity.h"
#include "game_logic.h"
#include "random.h"
#include <string.h>
// #include <stdio.h>

static const game_logic_values_t MOCK_RANDOM_DEFAULTS[NUMBER_OF_VALUES_TO_GUESS] = {0};

static int mock_random_return[NUMBER_OF_VALUES_TO_GUESS];
static size_t mock_random_return_index;

// manually mock out random_value
int random_value(void) {
  return mock_random_return[mock_random_return_index++];
}

void mock_random_set_values_to_match(const game_logic_values_t *set_answer) {
  mock_random_return_index = 0;
  // make it a bit more complicated then just 0-5, but it will still generate the same value each time
  for (size_t i = 0; i < NUMBER_OF_VALUES_TO_GUESS; i++) {
    mock_random_return[i] = (i*6) + set_answer[i];
  }
}

void setUp(void) {
  mock_random_set_values_to_match(MOCK_RANDOM_DEFAULTS);
  game_logic_generate_random_answer();
}

void test_get_all_ones_answer_value(void) {
  const game_logic_values_t *answer = game_logic_get_answer();

  game_logic_values_t expected_answer[] = {GAME_VALUE_ONE, GAME_VALUE_ONE, GAME_VALUE_ONE, GAME_VALUE_ONE};
  TEST_ASSERT_EQUAL_size_t_ARRAY(expected_answer, answer, NUMBER_OF_VALUES_TO_GUESS);
}

void test_random_answer_value(void) {
  game_logic_values_t set_answer[] = {GAME_VALUE_THREE, GAME_VALUE_FIVE, GAME_VALUE_ONE, GAME_VALUE_FIVE};
  mock_random_set_values_to_match(set_answer);

  game_logic_generate_random_answer();

  const game_logic_values_t *answer = game_logic_get_answer();
  TEST_ASSERT_EQUAL_size_t_ARRAY(set_answer, answer, NUMBER_OF_VALUES_TO_GUESS);
}

// Do we really need to return the answer because once you guess right you already have it.

void test_pass_in_user_guess_which_is_correct(void) {
  game_logic_values_t guess[] = {GAME_VALUE_ONE, GAME_VALUE_ONE, GAME_VALUE_ONE, GAME_VALUE_ONE};

  game_logic_feedback_t feedback = game_logic_get_feedback(guess);

  TEST_ASSERT_TRUE(feedback.is_guess_correct);
}

void test_pass_in_user_guess_which_is_incorrect(void) {
  game_logic_values_t guess[] = {GAME_VALUE_ONE, GAME_VALUE_ONE, GAME_VALUE_ONE, GAME_VALUE_TWO};

  game_logic_feedback_t feedback = game_logic_get_feedback(guess);

  TEST_ASSERT_FALSE(feedback.is_guess_correct);
}

void test_get_feedback_for_one_correct_placement_and_value(void) {
  game_logic_values_t guess[] = {GAME_VALUE_ONE, GAME_VALUE_TWO, GAME_VALUE_TWO, GAME_VALUE_THREE};

  game_logic_feedback_t feedback = game_logic_get_feedback(guess);

  TEST_ASSERT_EQUAL_UINT8(1, feedback.number_of_correct_value_and_placement);
}

void test_get_feedback_for_two_correct_placement_and_value(void) {
  game_logic_values_t guess[] = {GAME_VALUE_ONE, GAME_VALUE_ONE, GAME_VALUE_TWO, GAME_VALUE_THREE};

  game_logic_feedback_t feedback = game_logic_get_feedback(guess);

  TEST_ASSERT_EQUAL_UINT8(2, feedback.number_of_correct_value_and_placement);
}

void test_get_feedback_for_one_correct_value_only(void) {
  game_logic_values_t set_answer[] = {GAME_VALUE_ONE, GAME_VALUE_TWO, GAME_VALUE_THREE, GAME_VALUE_FOUR};
  mock_random_set_values_to_match(set_answer);
  game_logic_generate_random_answer();
  game_logic_values_t guess[] = {GAME_VALUE_FIVE, GAME_VALUE_FIVE, GAME_VALUE_ONE, GAME_VALUE_FIVE};
  
  game_logic_feedback_t feedback = game_logic_get_feedback(guess);
  
  TEST_ASSERT_EQUAL_UINT8(1, feedback.number_of_correct_values_only);
}

void test_get_feedback_for_two_correct_value_only(void) {
  game_logic_values_t set_answer[] = {GAME_VALUE_ONE, GAME_VALUE_TWO, GAME_VALUE_THREE, GAME_VALUE_FOUR};
  mock_random_set_values_to_match(set_answer);
  game_logic_generate_random_answer();
  game_logic_values_t guess[] = {GAME_VALUE_FIVE, GAME_VALUE_FIVE, GAME_VALUE_ONE, GAME_VALUE_TWO};
  
  game_logic_feedback_t feedback = game_logic_get_feedback(guess);
  
  TEST_ASSERT_EQUAL_UINT8(2, feedback.number_of_correct_values_only);
}

void test_get_feedback_for_one_correct_value_only_but_guess_contains_same_value_twice(void) {
  game_logic_values_t set_answer[] = {GAME_VALUE_ONE, GAME_VALUE_TWO, GAME_VALUE_THREE, GAME_VALUE_FOUR};
  mock_random_set_values_to_match(set_answer);
  game_logic_generate_random_answer();
  game_logic_values_t guess[] = {GAME_VALUE_FIVE, GAME_VALUE_ONE, GAME_VALUE_FIVE, GAME_VALUE_ONE};
  
  game_logic_feedback_t feedback = game_logic_get_feedback(guess);
  
  TEST_ASSERT_EQUAL_UINT8(1, feedback.number_of_correct_values_only);
}
