// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *key;
    char *value;
} Setting;

void set_setting(Setting **arr, size_t *count, size_t *cap, const char *key, const char *value) {
    for (size_t i = 0; i < *count; i++) {
        if (strcmp((*arr)[i].key, key) == 0) {
            char *nv = strdup(value);
            if (nv) {
                free((*arr)[i].value);
                ((*arr)[i].value) = nv;
            }
            return;
        }
    }
    if (*count >= *cap) {
        size_t ncap = *cap == 0 ? 4 : *cap * 2;
        Setting *narr = realloc(*arr, ncap * sizeof(Setting));
        if (narr) {
            *arr = narr;
            *cap = ncap;
        }
    }
    if (*count < *cap) {
        (*arr)[*count].key = strdup(key);
        (*arr)[*count].value = strdup(value);
        (*count)++;
    }
}

int main(void) {
    Setting *com = NULL;
    size_t com_count = 0;
    size_t com_cap = 0;
    Setting *tx = NULL;
    size_t tx_count = 0;
    size_t tx_cap = 0;
    int in_tx = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "BEGIN") == 0) {
                in_tx = 1;
                for (size_t i = 0; i < tx_count; i++) {
                    free(tx[i].key);
                    free(tx[i].value);
                }
                tx_count = 0;
            } else if (strcmp(cmd, "COMMIT") == 0) {
                if (in_tx) {
                    for (size_t i = 0; i < tx_count; i++) {
                        set_setting(&com, &com_count, &com_cap, tx[i].key, tx[i].value);
                    }
                    for (size_t i = 0; i < tx_count; i++) {
                        free(tx[i].key);
                        free(tx[i].value);
                    }
                    tx_count = 0;
                    in_tx = 0;
                }
            } else if (strcmp(cmd, "ABORT") == 0) {
                if (in_tx) {
                    for (size_t i = 0; i < tx_count; i++) {
                        free(tx[i].key);
                        free(tx[i].value);
                    }
                    tx_count = 0;
                    in_tx = 0;
                }
            } else if (strcmp(cmd, "DUMP") == 0) {
                for (size_t i = 0; i < com_count; i++) {
                    printf("%s %s\n", com[i].key, com[i].value);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "SET") == 0) {
            char *key = args;
            char *p2 = strchr(key, ' ');
            if (p2) {
                *p2 = '\0';
                char *value = p2 + 1;
                set_setting(&com, &com_count, &com_cap, key, value);
            }
        } else if (strcmp(cmd, "TSET") == 0) {
            if (in_tx) {
                char *key = args;
                char *p2 = strchr(key, ' ');
                if (p2) {
                    *p2 = '\0';
                    char *value = p2 + 1;
                    set_setting(&tx, &tx_count, &tx_cap, key, value);
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = args;
            char *found_val = NULL;
            if (in_tx) {
                for (size_t i = 0; i < tx_count; i++) {
                    if (strcmp(tx[i].key, key) == 0) {
                        found_val = tx[i].value;
                        break;
                    }
                }
            }
            if (!found_val) {
                for (size_t i = 0; i < com_count; i++) {
                    if (strcmp(com[i].key, key) == 0) {
                        found_val = com[i].value;
                        break;
                    }
                }
            }
            if (found_val) {
                printf("%s\n", found_val);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < com_count; i++) {
        free(com[i].key);
        free(com[i].value);
    }
    free(com);
    for (size_t i = 0; i < tx_count; i++) {
        free(tx[i].key);
        free(tx[i].value);
    }
    free(tx);
    return 0;
}
