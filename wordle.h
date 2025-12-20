#ifndef WORDLE_H
#define WORDLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define WORD_LENGTH 5
#define MAX_ATTEMPTS 6
#define MAX_WORDS 15000
#define MAX_LINE_LENGTH 100

typedef enum {
    GRAY = 0,
    YELLOW = 1,
    GREEN = 2
} Feedback;

typedef struct {
    char word[WORD_LENGTH + 1];
} Word;

typedef struct {
    Word* words;
    int count;
    int capacity;
} Dictionary;

typedef struct {
    char green[WORD_LENGTH];
    char yellow[WORD_LENGTH][26];
    int yellow_count[WORD_LENGTH];
    char gray[26];
    int gray_count;
    int min_letter_count[26];
} Constraints;

Dictionary* create_dictionary(int capacity);
void free_dictionary(Dictionary* dict);
int load_dictionary(Dictionary* dict, const char* filename);
int is_valid_word(Dictionary* dict, const char* word);

void generate_feedback(const char* target, const char* guess, Feedback* feedback);
void print_feedback(const char* guess, Feedback* feedback);
int play_wordle_game(Dictionary* dict, int use_solver);

void init_constraints(Constraints* c);
void update_constraints(Constraints* c, const char* guess, Feedback* feedback);
int matches_constraints(const char* word, Constraints* c);
Dictionary* filter_candidates(Dictionary* dict, Constraints* c);
char* choose_best_guess(Dictionary* candidates, Dictionary* all_words, int is_first);
double calculate_entropy(const char* guess, Dictionary* candidates);
int solve_wordle(Dictionary* dict, const char* target, int verbose);

void to_uppercase(char* str);
int count_letter(const char* word, char letter);

#endif