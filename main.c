#include "wordle.h"

void run_benchmark(Dictionary* dict, int num_tests) {
    printf("\n=== BENCHMARK MODE ===\n");
    printf("Testing solver on %d random words...\n\n", num_tests);
    
    int total_attempts = 0;
    int solved = 0;
    int failed = 0;
    int attempt_distribution[MAX_ATTEMPTS + 1] = {0};
    
    srand(time(NULL));
    
    for (int i = 0; i < num_tests; i++) {
        int target_idx = rand() % dict->count;
        char target[WORD_LENGTH + 1];
        strcpy(target, dict->words[target_idx].word);
        
        int attempts = solve_wordle(dict, target, 0);
        
        if (attempts > 0) {
            solved++;
            total_attempts += attempts;
            attempt_distribution[attempts]++;
            printf("Test %3d: %s - Solved in %d attempts\n", i + 1, target, attempts);
        } else {
            failed++;
            printf("Test %3d: %s - FAILED\n", i + 1, target);
        }
    }
    
    printf("\n=== BENCHMARK RESULTS ===\n");
    printf("Total tests: %d\n", num_tests);
    printf("Solved: %d (%.1f%%)\n", solved, 100.0 * solved / num_tests);
    printf("Failed: %d (%.1f%%)\n", failed, 100.0 * failed / num_tests);
    
    if (solved > 0) {
        printf("Average attempts: %.2f\n\n", (double)total_attempts / solved);
        
        printf("Distribution:\n");
        for (int i = 1; i <= MAX_ATTEMPTS; i++) {
            if (attempt_distribution[i] > 0) {
                printf("  %d attempts: %d words (%.1f%%)\n", 
                       i, attempt_distribution[i], 
                       100.0 * attempt_distribution[i] / solved);
            }
        }
    }
}

int display_menu() {
    printf("\n=== WORDLE GAME & SOLVER ===\n");
    printf("1. Play Wordle (human player)\n");
    printf("2. Watch solver play\n");
    printf("3. Run benchmark (test solver on multiple words)\n");
    printf("4. Exit\n");
    printf("\nChoice: ");
    
    int choice;
    if (scanf("%d", &choice) != 1) {
        while (getchar() != '\n');
        return -1;
    }
    while (getchar() != '\n');
    
    return choice;
}

int main(int argc, char* argv[]) {
    const char* dict_file = "words.txt";
    
    if (argc > 1) {
        dict_file = argv[1];
    }
    
    Dictionary* dict = create_dictionary(MAX_WORDS);
    if (!dict) {
        printf("Error: Could not create dictionary\n");
        return 1;
    }
    
    if (load_dictionary(dict, dict_file) <= 0) {
        printf("Error: Could not load dictionary from '%s'\n", dict_file);
        printf("Make sure the file exists and contains valid 5-letter words.\n");
        free_dictionary(dict);
        return 1;
    }
    
    while (1) {
        int choice = display_menu();
        
        switch (choice) {
            case 1:
                play_wordle_game(dict, 0);
                break;
                
            case 2:
                play_wordle_game(dict, 1);
                break;
                
            case 3: {
                printf("How many tests? (recommended: 10-100): ");
                int num_tests;
                if (scanf("%d", &num_tests) == 1 && num_tests > 0) {
                    while (getchar() != '\n');
                    run_benchmark(dict, num_tests);
                } else {
                    printf("Invalid number\n");
                    while (getchar() != '\n');
                }
                break;
            }
                
            case 4:
                printf("Thanks for playing!\n");
                free_dictionary(dict);
                return 0;
                
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
    
    return 0;
}