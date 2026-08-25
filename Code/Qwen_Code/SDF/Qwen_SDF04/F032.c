// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char* key; char* value; } Setting;

Setting* settings = NULL;
size_t s_count = 0, s_cap = 0;

Setting* staged = NULL;
size_t st_count = 0, st_cap = 0;
int in_trans = 0;

void cleanup() {
    for (size_t i = 0; i < s_count; i++) { free(settings[i].key); free(settings[i].value); }
    free(settings);
    for (size_t i = 0; i < st_count; i++) { free(staged[i].key); free(staged[i].value); }
    free(staged);
}

void set_val(Setting** arr, size_t* count, size_t* cap, const char* key, const char* val) {
    for (size_t i = 0; i < *count; i++) {
        if (strcmp((*arr)[i].key, key) == 0) {
            char* temp = strdup(val);
            if (!temp) exit(1);
            free((*arr)[i].value);
            (*arr)[i].value = temp;
            return;
        }
    }
    if (*count == *cap) {
        size_t new_cap = *cap == 0 ? 4 : *cap * 2;
        Setting* temp = realloc(*arr, new_cap * sizeof(Setting));
        if (!temp) exit(1);
        *arr = temp; *cap = new_cap;
    }
    (*arr)[*count].key = strdup(key);
    (*arr)[*count].value = strdup(val);
    if (!(*arr)[*count].key || !(*arr)[*count].value) exit(1);
    (*count)++;
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

        if (strcmp(cmd, "SET") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* key = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            set_val(&settings, &s_count, &s_cap, key, ptr);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_trans = 1;
            st_count = 0;
        } else if (strcmp(cmd, "TSET") == 0) {
            if (in_trans) {
                while (*ptr && isspace((unsigned char)*ptr)) ptr++;
                char* key = ptr;
                while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
                if (*ptr) { *ptr = '\0'; ptr++; }
                while (*ptr && isspace((unsigned char)*ptr)) ptr++;
                set_val(&staged, &st_count, &st_cap, key, ptr);
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_trans) {
                for (size_t i = 0; i < st_count; i++) {
                    set_val(&settings, &s_count, &s_cap, staged[i].key, staged[i].value);
                }
                st_count = 0;
                in_trans = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (in_trans) {
                for (size_t i = 0; i < st_count; i++) { free(staged[i].key); free(staged[i].value); }
                st_count = 0;
                in_trans = 0;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < s_count; i++) {
                if (strcmp(settings[i].key, ptr) == 0) {
                    printf("%s\n", settings[i].value);
                    break;
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (size_t i = 0; i < s_count; i++) {
                printf("%s=%s\n", settings[i].key, settings[i].value);
            }
        }
    }
    free(line);
    cleanup();
    return 0;
}