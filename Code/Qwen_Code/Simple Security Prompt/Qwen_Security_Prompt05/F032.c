// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Setting;

Setting *settings = NULL;
int num_settings = 0;

Setting *tx_settings = NULL;
int num_tx_settings = 0;
int in_tx = 0;

int find_setting(Setting *arr, int n, const char *key) {
    for (int i = 0; i < n; i++) {
        if (strcmp(arr[i].key, key) == 0) return i;
    }
    return -1;
}

void set_val(Setting **arr, int *n, const char *key, const char *val) {
    int idx = find_setting(*arr, *n, key);
    if (idx >= 0) {
        free((*arr)[idx].value);
        (*arr)[idx].value = strdup(val);
    } else {
        *arr = realloc(*arr, (*n + 1) * sizeof(Setting));
        (*arr)[*n].key = strdup(key);
        (*arr)[*n].value = strdup(val);
        (*n)++;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64];
        sscanf(line, "%s", cmd);
        
        if (strcmp(cmd, "SET") == 0) {
            char key[256];
            sscanf(line, "%*s %s", key);
            char *val = strstr(line, key);
            if (val) {
                val += strlen(key);
                if (*val == ' ') val++;
            } else {
                val = "";
            }
            set_val(&settings, &num_settings, key, val);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_tx = 1;
            num_tx_settings = 0;
            tx_settings = NULL;
        } else if (strcmp(cmd, "TSET") == 0) {
            if (in_tx) {
                char key[256];
                sscanf(line, "%*s %s", key);
                char *val = strstr(line, key);
                if (val) {
                    val += strlen(key);
                    if (*val == ' ') val++;
                } else {
                    val = "";
                }
                set_val(&tx_settings, &num_tx_settings, key, val);
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_tx) {
                for (int i = 0; i < num_tx_settings; i++) {
                    set_val(&settings, &num_settings, tx_settings[i].key, tx_settings[i].value);
                }
                for (int i = 0; i < num_tx_settings; i++) {
                    free(tx_settings[i].key);
                    free(tx_settings[i].value);
                }
                free(tx_settings);
                tx_settings = NULL;
                num_tx_settings = 0;
                in_tx = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (in_tx) {
                for (int i = 0; i < num_tx_settings; i++) {
                    free(tx_settings[i].key);
                    free(tx_settings[i].value);
                }
                free(tx_settings);
                tx_settings = NULL;
                num_tx_settings = 0;
                in_tx = 0;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char key[256];
            sscanf(line, "%*s %s", key);
            int idx = find_setting(settings, num_settings, key);
            if (idx >= 0) {
                printf("%s\n", settings[idx].value);
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < num_settings; i++) {
                printf("%s %s\n", settings[i].key, settings[i].value);
            }
        }
    }
    
    for (int i = 0; i < num_settings; i++) {
        free(settings[i].key);
        free(settings[i].value);
    }
    free(settings);
    
    if (in_tx) {
        for (int i = 0; i < num_tx_settings; i++) {
            free(tx_settings[i].key);
            free(tx_settings[i].value);
        }
        free(tx_settings);
    }
    
    return 0;
}