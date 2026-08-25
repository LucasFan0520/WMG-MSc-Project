// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char* key;
    char* value;
} Config;

Config* configs = NULL;
size_t c_count = 0;
size_t c_cap = 0;

void cleanup() {
    for (size_t i = 0; i < c_count; i++) {
        free(configs[i].key);
        free(configs[i].value);
    }
    free(configs);
}

int main(int argc, char** argv) {
    if (argc < 2) return 1;
    
    FILE* f = fopen(argv[1], "r");
    if (!f) return 1;
    
    char* line = NULL;
    size_t len = 0;
    while (getline(&line, &len, f) != -1) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        char* eq = strchr(line, '=');
        if (eq) {
            *eq = '\0';
            char* key = line;
            char* value = eq + 1;
            
            if (c_count == c_cap) {
                size_t new_cap = c_cap == 0 ? 4 : c_cap * 2;
                Config* temp = realloc(configs, new_cap * sizeof(Config));
                if (!temp) { free(line); fclose(f); return 1; }
                configs = temp;
                c_cap = new_cap;
            }
            configs[c_count].key = strdup(key);
            configs[c_count].value = strdup(value);
            if (!configs[c_count].key || !configs[c_count].value) { free(line); fclose(f); return 1; }
            c_count++;
        }
    }
    fclose(f);
    free(line);
    
    line = NULL;
    len = 0;
    while (getline(&line, &len, stdin) != -1) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char* ptr = line;
        while (*ptr && isspace((unsigned char)*ptr)) ptr++;
        if (!*ptr) continue;
        
        char* cmd = ptr;
        while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
        if (*ptr) {
            *ptr = '\0';
            ptr++;
        }
        
        if (strcmp(cmd, "SET") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* key = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* value = ptr;
            
            int found = 0;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(configs[i].key, key) == 0) {
                    found = 1;
                    char* temp = strdup(value);
                    if (!temp) { cleanup(); free(line); return 1; }
                    free(configs[i].value);
                    configs[i].value = temp;
                    break;
                }
            }
            if (!found) {
                if (c_count == c_cap) {
                    size_t new_cap = c_cap == 0 ? 4 : c_cap * 2;
                    Config* temp = realloc(configs, new_cap * sizeof(Config));
                    if (!temp) { cleanup(); free(line); return 1; }
                    configs = temp;
                    c_cap = new_cap;
                }
                configs[c_count].key = strdup(key);
                configs[c_count].value = strdup(value);
                if (!configs[c_count].key || !configs[c_count].value) { cleanup(); free(line); return 1; }
                c_count++;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* key = ptr;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(configs[i].key, key) == 0) {
                    free(configs[i].key);
                    free(configs[i].value);
                    for (size_t j = i; j < c_count - 1; j++) {
                        configs[j] = configs[j + 1];
                    }
                    c_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* key = ptr;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(configs[i].key, key) == 0) {
                    printf("%s\n", configs[i].value);
                    break;
                }
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (size_t i = 0; i < c_count; i++) {
                printf("%s=%s\n", configs[i].key, configs[i].value);
            }
        }
    }
    
    free(line);
    cleanup();
    return 0;
}