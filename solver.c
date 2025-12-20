#include "wordle.h"
#include <math.h>
int count_letter(const char* word, char letter) {
    int count = 0;
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (word[i] == letter) {
            count++;
        }
    }
    return count;
}
void init_constraints(Constraints* c) {
    for (int i = 0; i < WORD_LENGTH; i++) {
        c->green[i] = 0;
        c->yellow_count[i] = 0;
        for (int j = 0; j < 26; j++) {
            c->yellow[i][j] = 0;
        }
    }
    c->gray_count = 0;
    for (int i = 0; i < 26; i++) {
        c->gray[i] = 0;
        c->min_letter_count[i] = 0;
    }
}

void update_constraints(Constraints* c, const char* guess, Feedback* feedback) {
    int letter_in_word[26] = {0};
    
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (feedback[i] == GREEN) {
            c->green[i] = guess[i];
            letter_in_word[guess[i] - 'A'] = 1;
        } else if (feedback[i] == YELLOW) {
            letter_in_word[guess[i] - 'A'] = 1;
        }
    }
    
    for (int i = 0; i < WORD_LENGTH; i++) {
        char letter = guess[i];
        
        if (feedback[i] == YELLOW) {
            c->yellow[i][letter - 'A'] = 1;
            c->yellow_count[i]++;
            c->min_letter_count[letter - 'A']++;
        } else if (feedback[i] == GRAY) {
            if (!letter_in_word[letter - 'A']) {
                int already_gray = 0;
                for (int j = 0; j < c->gray_count; j++) {
                    if (c->gray[j] == letter) {
                        already_gray = 1;
                        break;
                    }
                }
                if (!already_gray) {
                    c->gray[c->gray_count++] = letter;
                }
            }
        }
    }
    
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (feedback[i] == GREEN) {
            int letter_idx = guess[i] - 'A';
            int count = count_letter(guess, guess[i]);
            if (count > c->min_letter_count[letter_idx]) {
                c->min_letter_count[letter_idx] = count;
            }
        }
    }
}

int matches_constraints(const char* word, Constraints* c) {
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (c->green[i] != 0 && word[i] != c->green[i]) {
            return 0;
        }
    }
    
    for (int i = 0; i < WORD_LENGTH; i++) {
        for (int j = 0; j < 26; j++) {
            if (c->yellow[i][j]) {
                char letter = 'A' + j;
                if (count_letter(word, letter) == 0) {
                    return 0;
                }
                if (word[i] == letter) {
                    return 0;
                }
            }
        }
    }
    
    for (int i = 0; i < c->gray_count; i++) {
        if (count_letter(word, c->gray[i]) > 0) {
            return 0;
        }
    }
    
    for (int i = 0; i < 26; i++) {
        if (c->min_letter_count[i] > 0) {
            char letter = 'A' + i;
            if (count_letter(word, letter) < c->min_letter_count[i]) {
                return 0;
            }
        }
    }
    
    return 1;
}

Dictionary* filter_candidates(Dictionary* dict, Constraints* c) {
    Dictionary* filtered = create_dictionary(dict->count);
    if (!filtered) return NULL;
    
    for (int i = 0; i < dict->count; i++) {
        if (matches_constraints(dict->words[i].word, c)) {
            strcpy(filtered->words[filtered->count].word, dict->words[i].word);
            filtered->count++;
        }
    }
    
    return filtered;
}

double calculate_entropy(const char* guess, Dictionary* candidates) {
    if (candidates->count == 0) return 0.0;
    
    int pattern_counts[243] = {0};
    
    for (int i = 0; i < candidates->count; i++) {
        Feedback feedback[WORD_LENGTH];
        generate_feedback(candidates->words[i].word, guess, feedback);
        
        int pattern = 0;
        for (int j = 0; j < WORD_LENGTH; j++) {
            pattern = pattern * 3 + feedback[j];
        }
        pattern_counts[pattern]++;
    }
    
    double entropy = 0.0;
    for (int i = 0; i < 243; i++) {
        if (pattern_counts[i] > 0) {
            double p = (double)pattern_counts[i] / candidates->count;
            entropy -= p * log2(p);
        }
    }
    
    return entropy;
}

char* choose_best_guess(Dictionary* candidates, Dictionary* all_words, int is_first) {
    char* best_guess = (char*)malloc(WORD_LENGTH + 1);
    
    if (is_first) {
        strcpy(best_guess, "SALET");
        return best_guess;
    }
    
    if (candidates->count == 1) {
        strcpy(best_guess, candidates->words[0].word);
        return best_guess;
    }
    
    double best_entropy = -1.0;
    int eval_limit = candidates->count < 50 ? candidates->count : 50;
    
    for (int i = 0; i < eval_limit; i++) {
        double entropy = calculate_entropy(candidates->words[i].word, candidates);
        
        if (entropy > best_entropy) {
            best_entropy = entropy;
            strcpy(best_guess, candidates->words[i].word);
        }
    }
    
    return best_guess;
}

int solve_wordle(Dictionary* dict, const char* target, int verbose) {
    Constraints constraints;
    init_constraints(&constraints);
    
    Dictionary* candidates = create_dictionary(dict->count);
    for (int i = 0; i < dict->count; i++) {
        strcpy(candidates->words[i].word, dict->words[i].word);
    }
    candidates->count = dict->count;
    
    if (verbose) {
        printf("Target word: %s\n", target);
        printf("Starting with %d possible words\n\n", candidates->count);
    }
    
    for (int attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
        char* guess = choose_best_guess(candidates, dict, attempt == 1);
        Feedback feedback[WORD_LENGTH];
        
        generate_feedback(target, guess, feedback);
        
        if (verbose) {
            printf("Attempt %d: %s\n", attempt, guess);
            print_feedback(guess, feedback);
            printf("\n");
        }
        
        if (strcmp(guess, target) == 0) {
            if (verbose) {
                printf("✓ Solved in %d attempts!\n", attempt);
            }
            free(guess);
            free_dictionary(candidates);
            return attempt;
        }
        
        update_constraints(&constraints, guess, feedback);
        free(guess);
        
        Dictionary* new_candidates = filter_candidates(dict, &constraints);
        free_dictionary(candidates);
        candidates = new_candidates;
        
        if (verbose) {
            printf("Remaining candidates: %d\n\n", candidates->count);
        }
        
        if (candidates->count == 0) {
            if (verbose) {
                printf("✗ No valid candidates remaining!\n");
            }
            free_dictionary(candidates);
            return -1;
        }
    }
    
    if (verbose) {
        printf("✗ Failed to solve in %d attempts\n", MAX_ATTEMPTS);
    }
    free_dictionary(candidates);
    return -1;
}