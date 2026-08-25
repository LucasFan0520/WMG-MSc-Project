// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Setting;

int main() {
    Setting *committed = NULL;
    int c_count = 0;
    Setting *staged = NULL;
    int s_count = 0;
    int tx_active = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "SET") == 0) {
            char key[256];
            int k_len = 0;
            if (sscanf(ptr, "%255s%n", key, &k_len) > 0) {
                char *val = ptr + k_len;
                while (*val == ' ') val++;
                int found = -1;
                for (int i = 0; i < c_count; i++) {
                    if (strcmp(committed[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(committed[found].value);
                    committed[found].value = strdup(val);
                } else {
                    committed = realloc(committed, (c_count + 1) * sizeof(Setting));
                    committed[c_count].key = strdup(key);
                    committed[c_count].value = strdup(val);
                    c_count++;
                }
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            tx_active = 1;
            for (int i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            free(staged);
            staged = NULL;
            s_count = 0;
        } else if (strcmp(cmd, "TSET") == 0) {
            if (tx_active) {
                char key[256];
                int k_len = 0;
                if (sscanf(ptr, "%255s%n", key, &k_len) > 0) {
                    char *val = ptr + k_len;
                    while (*val == ' ') val++;
                    int found = -1;
                    for (int i = 0; i < s_count; i++) {
                        if (strcmp(staged[i].key, key) == 0) {
                            found = i;
                            break;
                        }
                    }
                    if (found != -1) {
                        free(staged[found].value);
                        staged[found].value = strdup(val);
                    } else {
                        staged = realloc(staged, (s_count + 1) * sizeof(Setting));
                        staged[s_count].key = strdup(key);
                        staged[s_count].value = strdup(val);
                        s_count++;
                    }
                }
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (tx_active) {
                for (int i = 0; i < s_count; i++) {
                    int found = -1;
                    for (int j = 0; j < c_count; j++) {
                        if (strcmp(committed[j].key, staged[i].key) == 0) {
                            found = j;
                            break;
                        }
                    }
                    if (found != -1) {
                        free(committed[found].value);
                        committed[found].value = strdup(staged[i].value);
                    } else {
                        committed = realloc(committed, (c_count + 1) * sizeof(Setting));
                        committed[c_count].key = strdup(staged[i].key);
                        committed[c_count].value = strdup(staged[i].value);
                        c_count++;
                    }
                }
                for (int i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                free(staged);
                staged = NULL;
                s_count = 0;
                tx_active = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (tx_active) {
                for (int i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                free(staged);
                staged = NULL;
                s_count = 0;
                tx_active = 0;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char key[256];
            if (sscanf(ptr, "%255s", key) > 0) {
                int found = -1;
                if (tx_active) {
                    for (int i = 0; i < s_count; i++) {
                        if (strcmp(staged[i].key, key) == 0) {
                            found = i;
                            break;
                        }
                    }
                    if (found != -1) {
                        printf("%s\n", staged[found].value);
                        continue;
                    }
                }
                found = -1;
                for (int i = 0; i < c_count; i++) {
                    if (strcmp(committed[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    printf("%s\n", committed[found].value);
                } else {
                    printf("NOT_FOUND\n");
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < c_count; i++) {
                printf("%s=%s\n", committed[i].key, committed[i].value);
            }
        }
    }
    for (int i = 0; i < c_count; i++) {
        free(committed[i].key);
        free(committed[i].value);
    }
    free(committed);
    for (int i = 0; i < s_count; i++) {
        free(staged[i].key);
        free(staged[i].value);
    }
    free(staged);
    return 0;
}
