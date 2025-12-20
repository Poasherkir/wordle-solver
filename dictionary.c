#include "wordle.h"

Dictionary* create_dictionary(int capacity) {
    Dictionary* dict = (Dictionary*)malloc(sizeof(Dictionary));
    if (!dict) return NULL;
    
    dict->words = (Word*)malloc(capacity * sizeof(Word));
    if (!dict->words) {
        free(dict);
        return NULL;
    }
    
    dict->count = 0;
    dict->capacity = capacity;
    return dict;
}

void free_dictionary(Dictionary* dict) {
    if (dict) {
        if (dict->words) free(dict->words);
        free(dict);
    }
}

int load_dictionary(Dictionary* dict, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open dictionary file '%s'\n", filename);
        return -1;
    }
    
    char line[MAX_LINE_LENGTH];
    int loaded = 0;
    
    while (fgets(line, sizeof(line), file) && loaded < dict->capacity) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) == 0) continue;
        if (strlen(line) != WORD_LENGTH) continue;
        
        int valid = 1;
        for (int i = 0; i < WORD_LENGTH; i++) {
            if (!isalpha(line[i])) {
                valid = 0;
                break;
            }
        }
        
        if (!valid) continue;
        
        to_uppercase(line);
        strcpy(dict->words[loaded].word, line);
        loaded++;
    }
    
    fclose(file);
    dict->count = loaded;
    
    printf("Loaded %d words from dictionary\n", loaded);
    return loaded;
}

int is_valid_word(Dictionary* dict, const char* word) {
    char upper_word[WORD_LENGTH + 1];
    strcpy(upper_word, word);
    to_uppercase(upper_word);
    
    for (int i = 0; i < dict->count; i++) {
        if (strcmp(dict->words[i].word, upper_word) == 0) {
            return 1;
        }
    }
    return 0;
}

void to_uppercase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}