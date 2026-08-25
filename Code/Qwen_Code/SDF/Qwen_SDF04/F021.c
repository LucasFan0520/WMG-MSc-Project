// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char* name; char* note; } Ing;
typedef struct { char* name; Ing* ings; size_t i_count, i_cap; } Recipe;

Recipe* recipes = NULL;
size_t r_count = 0, r_cap = 0;

void cleanup() {
    for (size_t i = 0; i < r_count; i++) {
        for (size_t j = 0; j < recipes[i].i_count; j++) {
            free(recipes[i].ings[j].name);
            free(recipes[i].ings[j].note);
        }
        free(recipes[i].ings);
        free(recipes[i].name);
    }
    free(recipes);
}

int main() {
    char* line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char* ptr = line;
        while (*ptr && isspace((unsigned char)*ptr)) ptr++;
        if (!*ptr) continue;
        char* cmd = ptr;
        while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
        if (*ptr) { *ptr = '\0'; ptr++; }

        if (strcmp(cmd, "RECIPE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            if (!*ptr) continue;
            if (r_count == r_cap) {
                size_t new_cap = r_cap == 0 ? 4 : r_cap * 2;
                Recipe* temp = realloc(recipes, new_cap * sizeof(Recipe));
                if (!temp) { cleanup(); free(line); return 1; }
                recipes = temp; r_cap = new_cap;
            }
            recipes[r_count].name = strdup(ptr);
            recipes[r_count].ings = NULL;
            recipes[r_count].i_count = 0;
            recipes[r_count].i_cap = 0;
            if (!recipes[r_count].name) { cleanup(); free(line); return 1; }
            r_count++;
        } else if (strcmp(cmd, "ING") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* rname = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* iname = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* note = ptr;

            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, rname) == 0) {
                    if (recipes[i].i_count == recipes[i].i_cap) {
                        size_t new_cap = recipes[i].i_cap == 0 ? 4 : recipes[i].i_cap * 2;
                        Ing* temp = realloc(recipes[i].ings, new_cap * sizeof(Ing));
                        if (!temp) { cleanup(); free(line); return 1; }
                        recipes[i].ings = temp; recipes[i].i_cap = new_cap;
                    }
                    recipes[i].ings[recipes[i].i_count].name = strdup(iname);
                    recipes[i].ings[recipes[i].i_count].note = strdup(note);
                    if (!recipes[i].ings[recipes[i].i_count].name || !recipes[i].ings[recipes[i].i_count].note) {
                        cleanup(); free(line); return 1;
                    }
                    recipes[i].i_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* rname = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* iname = ptr;

            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, rname) == 0) {
                    for (size_t j = 0; j < recipes[i].i_count; j++) {
                        if (strcmp(recipes[i].ings[j].name, iname) == 0) {
                            free(recipes[i].ings[j].name);
                            free(recipes[i].ings[j].note);
                            for (size_t k = j; k < recipes[i].i_count - 1; k++) recipes[i].ings[k] = recipes[i].ings[k + 1];
                            recipes[i].i_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, ptr) == 0) {
                    for (size_t j = 0; j < recipes[i].i_count; j++) {
                        free(recipes[i].ings[j].name);
                        free(recipes[i].ings[j].note);
                    }
                    free(recipes[i].ings);
                    free(recipes[i].name);
                    for (size_t j = i; j < r_count - 1; j++) recipes[j] = recipes[j + 1];
                    r_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, ptr) == 0) {
                    printf("%s\n", recipes[i].name);
                    for (size_t j = 0; j < recipes[i].i_count; j++) printf("  %s %s\n", recipes[i].ings[j].name, recipes[i].ings[j].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < r_count; i++) printf("%s\n", recipes[i].name);
        }
    }
    free(line);
    cleanup();
    return 0;
}