#include "wordle.h"

void generate_feedback(const char* target, const char* guess, Feedback* feedback) {
    int target_used[WORD_LENGTH] = {0};
    int guess_used[WORD_LENGTH] = {0};

    /* Pass 1: GREEN */
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (guess[i] == target[i]) {
            feedback[i] = GREEN;
            target_used[i] = 1;
            guess_used[i] = 1;
        } else {
            feedback[i] = GRAY;
        }
    }

    /* Pass 2: YELLOW */
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (guess_used[i]) continue;

        for (int j = 0; j < WORD_LENGTH; j++) {
            if (!target_used[j] && guess[i] == target[j]) {
                feedback[i] = YELLOW;
                target_used[j] = 1;
                break;
            }
        }
    }
}

void print_feedback(const char* guess, Feedback* feedback) {
    for (int i = 0; i < WORD_LENGTH; i++) {
        printf(" %c ", guess[i]);
    }
    printf("\n");

    for (int i = 0; i < WORD_LENGTH; i++) {
        if (feedback[i] == GREEN) {
            printf("[G]");
        } else if (feedback[i] == YELLOW) {
            printf("[Y]");
        } else {
            printf("[_]");
        }
    }
    printf("\n");
}

int play_wordle_game(Dictionary* dict, int use_solver) {
    if (dict->count == 0) {
        printf("Error: Dictionary is empty\n");
        return -1;
    }

    srand(time(NULL));
    int target_index = rand() % dict->count;
    char target[WORD_LENGTH + 1];
    strcpy(target, dict->words[target_index].word);

    printf("\n=== WORDLE GAME ===\n");
    if (!use_solver) {
        printf("I've selected a 5-letter word. You have %d attempts to guess it.\n", MAX_ATTEMPTS);
        printf("Feedback: [G]=Green (correct), [Y]=Yellow (wrong position), [_]=Gray (not in word)\n\n");
    }

    if (use_solver) {
        return solve_wordle(dict, target, 1);
    }

    char guess[MAX_LINE_LENGTH];
    Feedback feedback[WORD_LENGTH];

    for (int attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
        printf("Attempt %d/%d: ", attempt, MAX_ATTEMPTS);

        /* Read guess */
        if (fgets(guess, sizeof(guess), stdin) == NULL) {
            printf("\nError reading input\n");
            return -1;
        }

        guess[strcspn(guess, "\n")] = 0;

        if (strlen(guess) != WORD_LENGTH) {
            printf("Word must be exactly %d letters.\n\n", WORD_LENGTH);
            attempt--;
            continue;
        }

        to_uppercase(guess);

        /* NEW BEHAVIOR: warn but still show feedback */
        int in_dict = is_valid_word(dict, guess);
        if (!in_dict) {
            printf("⚠️  Word not in dictionary, feedback shown anyway.\n");
        }

        generate_feedback(target, guess, feedback);
        print_feedback(guess, feedback);

        if (strcmp(guess, target) == 0) {
            printf("\n🎉 Congratulations! You guessed the word in %d attempts!\n", attempt);
            return attempt;
        }

        printf("\n");
    }

    printf("😞 Game over! The word was: %s\n", target);
    return -1;
}
