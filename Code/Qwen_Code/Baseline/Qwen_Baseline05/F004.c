// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_KEY 100
#define MAX_VAL 1000
#define MAX_TEXT 10000

typedef struct {
    char key[MAX_KEY];
    char value[MAX_VAL];
} Variable;

Variable vars[1000];
int var_count = 0;

int find_var(const char *key) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].key, key) == 0) return i;
    }
    return -1;
}

void render_text(const char *text) {
    char result[MAX_TEXT * 2];
    int r = 0;
    
    for (int i = 0; text[i]; ) {
        if (text[i] == '{') {
            int j = i + 1;
            while (text[j] && text[j] != '}') j++;
            
            if (text[j] == '}') {
                char key[MAX_KEY];
                strncpy(key, text + i + 1, j - i - 1);
                key[j - i - 1] = '\0';
                
                int idx = find_var(key);
                if (idx != -1) {
                    strcpy(result + r, vars[idx].value);
                    r += strlen(vars[idx].value);
                } else {
                    strncpy(result + r, text + i, j - i + 1);
                    r += j - i + 1;
                }
                i = j + 1;
            } else {
                result[r++] = text[i++];
            }
        } else {
            result[r++] = text[i++];
        }
    }
    result[r] = '\0';
    printf("%s\n", result);
}

int main() {
    char cmd[20], key[MAX_KEY], text[MAX_TEXT];
    
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "SET") == 0) {
            scanf("%s", key);
            fgets(text, MAX_TEXT, stdin);
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            if (text[strlen(text) - 1] == '\n') text[strlen(text) - 1] = '\0';
            
            int idx = find_var(key);
            if (idx == -1) {
                strcpy(vars[var_count].key, key);
                strcpy(vars[var_count].value, text);
                var_count++;
            } else {
                strcpy(vars[idx].value, text);
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            scanf("%s", key);
            int idx = find_var(key);
            if (idx != -1) {
                for (int i = idx; i < var_count - 1; i++) {
                    vars[i] = vars[i + 1];
                }
                var_count--;
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            fgets(text, MAX_TEXT, stdin);
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            if (text[strlen(text) - 1] == '\n') text[strlen(text) - 1] = '\0';
            render_text(text);
        } else if (strcmp(cmd, "VARS") == 0) {
            for (int i = 0; i < var_count; i++) {
                printf("%s=%s\n", vars[i].key, vars[i].value);
            }
        }
    }
    
    return 0;
}