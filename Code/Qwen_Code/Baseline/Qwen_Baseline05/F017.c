// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_KEY 100
#define MAX_VAL 1000

typedef struct {
    char key[MAX_KEY];
    char value[MAX_VAL];
} Config;

Config configs[1000];
int config_count = 0;

int find_config(const char *key) {
    for (int i = 0; i < config_count; i++) {
        if (strcmp(configs[i].key, key) == 0) return i;
    }
    return -1;
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    
    char line[1200];
    while (fgets(line, sizeof(line), f)) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        
        char *eq = strchr(line, '=');
        if (eq) {
            *eq = '\0';
            char *key = line;
            char *value = eq + 1;
            
            strcpy(configs[config_count].key, key);
            strcpy(configs[config_count].value, value);
            config_count++;
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    
    char cmd[20], key[MAX_KEY], value[MAX_VAL];
    
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "SET") == 0) {
            scanf("%s", key);
            fgets(value, MAX_VAL, stdin);
            if (value[0] == ' ') memmove(value, value + 1, strlen(value));
            if (value[strlen(value) - 1] == '\n') value[strlen(value) - 1] = '\0';
            
            int idx = find_config(key);
            if (idx == -1) {
                strcpy(configs[config_count].key, key);
                strcpy(configs[config_count].value, value);
                config_count++;
            } else {
                strcpy(configs[idx].value, value);
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            scanf("%s", key);
            int idx = find_config(key);
            if (idx != -1) {
                for (int i = idx; i < config_count - 1; i++) {
                    configs[i] = configs[i + 1];
                }
                config_count--;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            scanf("%s", key);
            int idx = find_config(key);
            if (idx != -1) {
                printf("%s\n", configs[idx].value);
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (int i = 0; i < config_count; i++) {
                printf("%s=%s\n", configs[i].key, configs[i].value);
            }
        }
    }
    
    return 0;
}