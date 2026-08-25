/* F032.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Setting;

int main(void) {
    Setting *committed = NULL;
    int c_count = 0;
    int c_cap = 0;

    Setting *staged = NULL;
    int s_count = 0;
    int s_cap = 0;

    int in_tx = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *args = line + 4;
            char *space = strchr(args, ' ');
            char *k = NULL;
            char *v = NULL;
            if (space) {
                *space = '\0';
                k = args;
                v = space + 1;
            } else {
                k = args;
                v = "";
            }
            int found = 0;
            for (int i = 0; i < c_count; i++) {
                if (strcmp(committed[i].key, k) == 0) {
                    char *n_val = strdup(v);
                    if (n_val) {
                        free(committed[i].value);
                        committed[i].value = n_val;
                    }
                    found = 1;
                    break;
                }
            }
            if (!found) {
                char *n_key = strdup(k);
                char *n_val = strdup(v);
                if (n_key && n_val) {
                    if (c_count >= c_cap) {
                        int new_cap = c_cap == 0 ? 4 : c_cap * 2;
                        Setting *tmp = realloc(committed, new_cap * sizeof(Setting));
                        if (tmp) {
                            committed = tmp;
                            c_cap = new_cap;
                        } else {
                            free(n_key); free(n_val);
                            if (space) *space = ' ';
                            continue;
                        }
                    }
                    committed[c_count].key = n_key;
                    committed[c_count].value = n_val;
                    c_count++;
                } else {
                    free(n_key); free(n_val);
                }
            }
            if (space) *space = ' ';
        } else if (strcmp(line, "BEGIN") == 0) {
            in_tx = 1;
            for (int i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            s_count = 0;
        } else if (strncmp(line, "TSET ", 5) == 0) {
            if (!in_tx) continue;
            char *args = line + 5;
            char *space = strchr(args, ' ');
            char *k = NULL;
            char *v = NULL;
            if (space) {
                *space = '\0';
                k = args;
                v = space + 1;
            } else {
                k = args;
                v = "";
            }
            int found = 0;
            for (int i = 0; i < s_count; i++) {
                if (strcmp(staged[i].key, k) == 0) {
                    char *n_val = strdup(v);
                    if (n_val) {
                        free(staged[i].value);
                        staged[i].value = n_val;
                    }
                    found = 1;
                    break;
                }
            }
            if (!found) {
                char *n_key = strdup(k);
                char *n_val = strdup(v);
                if (n_key && n_val) {
                    if (s_count >= s_cap) {
                        int new_cap = s_cap == 0 ? 4 : s_cap * 2;
                        Setting *tmp = realloc(staged, new_cap * sizeof(Setting));
                        if (tmp) {
                            staged = tmp;
                            s_cap = new_cap;
                        } else {
                            free(n_key); free(n_val);
                            if (space) *space = ' ';
                            continue;
                        }
                    }
                    staged[s_count].key = n_key;
                    staged[s_count].value = n_val;
                    s_count++;
                } else {
                    free(n_key); free(n_val);
                }
            }
            if (space) *space = ' ';
        } else if (strcmp(line, "COMMIT") == 0) {
            if (!in_tx) continue;
            for (int i = 0; i < s_count; i++) {
                int found = 0;
                for (int j = 0; j < c_count; j++) {
                    if (strcmp(committed[j].key, staged[i].key) == 0) {
                        char *n_val = strdup(staged[i].value);
                        if (n_val) {
                            free(committed[j].value);
                            committed[j].value = n_val;
                        }
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    char *n_key = strdup(staged[i].key);
                    char *n_val = strdup(staged[i].value);
                    if (n_key && n_val) {
                        if (c_count >= c_cap) {
                            int new_cap = c_cap == 0 ? 4 : c_cap * 2;
                            Setting *tmp = realloc(committed, new_cap * sizeof(Setting));
                            if (tmp) {
                                committed = tmp;
                                c_cap = new_cap;
                            } else {
                                free(n_key); free(n_val);
                                continue;
                            }
                        }
                        committed[c_count].key = n_key;
                        committed[c_count].value = n_val;
                        c_count++;
                    } else {
                        free(n_key); free(n_val);
                    }
                }
            }
            for (int i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            s_count = 0;
            in_tx = 0;
        } else if (strcmp(line, "ABORT") == 0) {
            if (!in_tx) continue;
            for (int i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            s_count = 0;
            in_tx = 0;
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *k = line + 4;
            for (int i = 0; i < c_count; i++) {
                if (strcmp(committed[i].key, k) == 0) {
                    printf("%s\n", committed[i].value);
                    break;
                }
            }
        } else if (strcmp(line, "DUMP") == 0) {
            for (int i = 0; i < c_count; i++) {
                printf("%s %s\n", committed[i].key, committed[i].value);
            }
        }
    }
    free(line);
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
