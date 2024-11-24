#include "unity.h"
#include "game_logic.h"
#include "random.h"
#include <string.h>

static const game_logic_values_t MOCK_RANDOM_DEFAULTS[NUMBER_OF_VALUES_TO_GUESS] = {0};

static int mock_random_return[NUMBER_OF_VALUES_TO_GUESS];
static size_t mock_random_return_index;

// manually mock out random_value
int random_value(void) {
  return mock_random_return[mock_random_return_index++];
}

static void mock_random_set_values_to_match(const game_logic_values_t *set_answer) {
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

void tearDown(void) {}


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
  TEST_ASSERT_EQUAL_UINT8(0, feedback.number_of_correct_value_only);
}

void test_get_feedback_for_two_correct_placement_and_value(void) {
  game_logic_values_t guess[] = {GAME_VALUE_ONE, GAME_VALUE_ONE, GAME_VALUE_TWO, GAME_VALUE_THREE};

  game_logic_feedback_t feedback = game_logic_get_feedback(guess);

  TEST_ASSERT_EQUAL_UINT8(2, feedback.number_of_correct_value_and_placement);
  TEST_ASSERT_EQUAL_UINT8(0, feedback.number_of_correct_value_only);
}

void test_get_feedback_for_one_correct_value_only(void) {
  game_logic_values_t set_answer[] = {GAME_VALUE_ONE, GAME_VALUE_TWO, GAME_VALUE_THREE, GAME_VALUE_FOUR};
  mock_random_set_values_to_match(set_answer);
  game_logic_generate_random_answer();
  game_logic_values_t guess[] = {GAME_VALUE_FIVE, GAME_VALUE_FIVE, GAME_VALUE_ONE, GAME_VALUE_FIVE};
  
  game_logic_feedback_t feedback = game_logic_get_feedback(guess);
  
  TEST_ASSERT_EQUAL_UINT8(1, feedback.number_of_correct_value_only);
  TEST_ASSERT_EQUAL_UINT8(0, feedback.number_of_correct_value_and_placement);
}

void test_get_feedback_for_two_correct_value_only(void) {
  game_logic_values_t set_answer[] = {GAME_VALUE_ONE, GAME_VALUE_TWO, GAME_VALUE_THREE, GAME_VALUE_FOUR};
  mock_random_set_values_to_match(set_answer);
  game_logic_generate_random_answer();
  game_logic_values_t guess[] = {GAME_VALUE_FIVE, GAME_VALUE_FIVE, GAME_VALUE_ONE, GAME_VALUE_TWO};
  
  game_logic_feedback_t feedback = game_logic_get_feedback(guess);
  
  TEST_ASSERT_EQUAL_UINT8(2, feedback.number_of_correct_value_only);
  TEST_ASSERT_EQUAL_UINT8(0, feedback.number_of_correct_value_and_placement);
}

void test_get_feedback_for_one_correct_value_only_but_guess_contains_same_value_twice(void) {
  game_logic_values_t set_answer[] = {GAME_VALUE_ONE, GAME_VALUE_TWO, GAME_VALUE_THREE, GAME_VALUE_FOUR};
  mock_random_set_values_to_match(set_answer);
  game_logic_generate_random_answer();
  game_logic_values_t guess[] = {GAME_VALUE_FIVE, GAME_VALUE_ONE, GAME_VALUE_FIVE, GAME_VALUE_ONE};
  
  game_logic_feedback_t feedback = game_logic_get_feedback(guess);
  
  TEST_ASSERT_EQUAL_UINT8(1, feedback.number_of_correct_value_only);
  TEST_ASSERT_EQUAL_UINT8(0, feedback.number_of_correct_value_and_placement);
}

void test_get_feedback_for_one_correct_value_only_and_one_correct_placement_and_value(void) {
  game_logic_values_t set_answer[] = {GAME_VALUE_ONE, GAME_VALUE_TWO, GAME_VALUE_THREE, GAME_VALUE_FOUR};
  mock_random_set_values_to_match(set_answer);
  game_logic_generate_random_answer();
  game_logic_values_t guess[] = {GAME_VALUE_ONE, GAME_VALUE_THREE, GAME_VALUE_FIVE, GAME_VALUE_ONE};

  game_logic_feedback_t feedback = game_logic_get_feedback(guess);
  
  TEST_ASSERT_EQUAL_UINT8(1, feedback.number_of_correct_value_only);
  TEST_ASSERT_EQUAL_UINT8(1, feedback.number_of_correct_value_and_placement);
}

void test_get_feedback_all_correct_value_only(void) {
  game_logic_values_t set_answer[] = {GAME_VALUE_ONE, GAME_VALUE_TWO, GAME_VALUE_THREE, GAME_VALUE_FOUR};
  mock_random_set_values_to_match(set_answer);
  game_logic_generate_random_answer();
  game_logic_values_t guess[] = {GAME_VALUE_FOUR, GAME_VALUE_THREE, GAME_VALUE_TWO, GAME_VALUE_ONE};

  game_logic_feedback_t feedback = game_logic_get_feedback(guess);
  
  TEST_ASSERT_EQUAL_UINT8(4, feedback.number_of_correct_value_only);
  TEST_ASSERT_EQUAL_UINT8(0, feedback.number_of_correct_value_and_placement);
}

void test_get_feedback_for_two_correct_value_only_and_two_correct_placement_and_value(void) {
  game_logic_values_t set_answer[] = {GAME_VALUE_ONE, GAME_VALUE_TWO, GAME_VALUE_FIVE, GAME_VALUE_SIX};
  mock_random_set_values_to_match(set_answer);
  game_logic_generate_random_answer();
  game_logic_values_t guess[] = {GAME_VALUE_ONE, GAME_VALUE_TWO, GAME_VALUE_SIX, GAME_VALUE_FIVE};

  game_logic_feedback_t feedback = game_logic_get_feedback(guess);
  
  TEST_ASSERT_EQUAL_UINT8(2, feedback.number_of_correct_value_only);
  TEST_ASSERT_EQUAL_UINT8(2, feedback.number_of_correct_value_and_placement);
}

void test_get_feedback_for_two_correct_value_only_and_two_correct_placement_and_value_repeated_answer(void) {
  game_logic_values_t set_answer[] = {GAME_VALUE_SIX, GAME_VALUE_TWO, GAME_VALUE_FIVE, GAME_VALUE_SIX};
  mock_random_set_values_to_match(set_answer);
  game_logic_generate_random_answer();
  game_logic_values_t guess[] = {GAME_VALUE_SIX, GAME_VALUE_TWO, GAME_VALUE_SIX, GAME_VALUE_FIVE};

  game_logic_feedback_t feedback = game_logic_get_feedback(guess);
  
  TEST_ASSERT_EQUAL_UINT8(2, feedback.number_of_correct_value_only);
  TEST_ASSERT_EQUAL_UINT8(2, feedback.number_of_correct_value_and_placement);
}

void test_get_feedback_all_incorrect(void) {
  game_logic_values_t set_answer[] = {GAME_VALUE_SIX, GAME_VALUE_FIVE, GAME_VALUE_FIVE, GAME_VALUE_SIX};
  mock_random_set_values_to_match(set_answer);
  game_logic_generate_random_answer();
  game_logic_values_t guess[] = {GAME_VALUE_ONE, GAME_VALUE_TWO, GAME_VALUE_TWO, GAME_VALUE_ONE};

  game_logic_feedback_t feedback = game_logic_get_feedback(guess);
  
  TEST_ASSERT_EQUAL_UINT8(0, feedback.number_of_correct_value_only);
  TEST_ASSERT_EQUAL_UINT8(0, feedback.number_of_correct_value_and_placement);
  TEST_ASSERT_FALSE(feedback.is_guess_correct);
}

void test_get_feedback_all_correct(void) {
  game_logic_values_t set_answer[] = {GAME_VALUE_SIX, GAME_VALUE_FIVE, GAME_VALUE_FIVE, GAME_VALUE_SIX};
  mock_random_set_values_to_match(set_answer);
  game_logic_generate_random_answer();
  game_logic_values_t guess[] = {GAME_VALUE_SIX, GAME_VALUE_FIVE, GAME_VALUE_FIVE, GAME_VALUE_SIX};

  game_logic_feedback_t feedback = game_logic_get_feedback(guess);
  
  TEST_ASSERT_EQUAL_UINT8(0, feedback.number_of_correct_value_only);
  TEST_ASSERT_EQUAL_UINT8(4, feedback.number_of_correct_value_and_placement);
  TEST_ASSERT_TRUE(feedback.is_guess_correct);
}

int main(void)
{
  UNITY_BEGIN();
    RUN_TEST(test_pass_in_user_guess_which_is_correct);
    RUN_TEST(test_pass_in_user_guess_which_is_incorrect);
    RUN_TEST(test_get_feedback_for_one_correct_placement_and_value);
    RUN_TEST(test_get_feedback_for_two_correct_placement_and_value);
    RUN_TEST(test_get_feedback_for_one_correct_value_only);
    RUN_TEST(test_get_feedback_for_two_correct_value_only);
    RUN_TEST(test_get_feedback_for_one_correct_value_only_but_guess_contains_same_value_twice);
    RUN_TEST(test_get_feedback_for_one_correct_value_only_and_one_correct_placement_and_value);
    RUN_TEST(test_get_feedback_all_correct_value_only);
    RUN_TEST(test_get_feedback_for_two_correct_value_only_and_two_correct_placement_and_value);
    RUN_TEST(test_get_feedback_for_two_correct_value_only_and_two_correct_placement_and_value_repeated_answer);
    RUN_TEST(test_get_feedback_all_incorrect);
    RUN_TEST(test_get_feedback_all_correct);
  return UNITY_END();
}