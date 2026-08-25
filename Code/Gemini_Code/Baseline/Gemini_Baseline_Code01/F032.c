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
    int in_tx = 0;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "SET") == 0) {
            char key[256];
            int b = 0;
            if (sscanf(args, "%255s%n", key, &b) == 1) {
                char *val = args + b;
                while (*val == ' ') val++;
                int found = 0;
                for (int i = 0; i < c_count; i++) {
                    if (strcmp(committed[i].key, key) == 0) {
                        free(committed[i].value);
                        committed[i].value = strdup(val);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    committed = realloc(committed, (c_count + 1) * sizeof(Setting));
                    committed[c_count].key = strdup(key);
                    committed[c_count].value = strdup(val);
                    c_count++;
                }
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_tx = 1;
            for (int i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            free(staged);
            staged = NULL;
            s_count = 0;
        } else if (strcmp(cmd, "TSET") == 0) {
            if (in_tx) {
                char key[256];
                int b = 0;
                if (sscanf(args, "%255s%n", key, &b) == 1) {
                    char *val = args + b;
                    while (*val == ' ') val++;
                    int found = 0;
                    for (int i = 0; i < s_count; i++) {
                        if (strcmp(staged[i].key, key) == 0) {
                            free(staged[i].value);
                            staged[i].value = strdup(val);
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        staged = realloc(staged, (s_count + 1) * sizeof(Setting));
                        staged[s_count].key = strdup(key);
                        staged[s_count].value = strdup(val);
                        s_count++;
                    }
                }
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_tx) {
                for (int i = 0; i < s_count; i++) {
                    int found = 0;
                    for (int j = 0; j < c_count; j++) {
                        if (strcmp(committed[j].key, staged[i].key) == 0) {
                            free(committed[j].value);
                            committed[j].value = strdup(staged[i].value);
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        committed = realloc(committed, (c_count + 1) * sizeof(Setting));
                        committed[c_count].key = strdup(staged[i].key);
                        committed[c_count].value = strdup(staged[i].value);
                        c_count++;
                    }
                }
                in_tx = 0;
                for (int i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                free(staged);
                staged = NULL;
                s_count = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (in_tx) {
                in_tx = 0;
                for (int i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                free(staged);
                staged = NULL;
                s_count = 0;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char key[256];
            if (sscanf(args, "%255s", key) == 1) {
                int found = 0;
                if (in_tx) {
                    for (int i = 0; i < s_count; i++) {
                        if (strcmp(staged[i].key, key) == 0) {
                            printf("%s\n", staged[i].value);
                            found = 1;
                            break;
                        }
                    }
                }
                if (!found) {
                    for (int i = 0; i < c_count; i++) {
                        if (strcmp(committed[i].key, key) == 0) {
                            printf("%s\n", committed[i].value);
                            found = 1;
                            break;
                        }
                    }
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
