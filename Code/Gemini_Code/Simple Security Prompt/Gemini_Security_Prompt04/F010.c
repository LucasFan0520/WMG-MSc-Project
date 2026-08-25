// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *key;
    char **values;
    int vcount;
    int vcap;
} KeyIndex;

int main(void) {
    KeyIndex *index_table = NULL;
    int kcount = 0;
    int kcap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *val = p + 1;
                while (*val == ' ') val++;
                char *val_end = val;
                while (*val_end && *val_end != ' ') val_end++;
                if (*val_end == ' ') *val_end = '\0';
                int found = -1;
                for (int i = 0; i < kcount; i++) {
                    if (strcmp(index_table[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found == -1) {
                    if (kcount >= kcap) {
                        kcap = kcap == 0 ? 16 : kcap * 2;
                        index_table = realloc(index_table, kcap * sizeof(KeyIndex));
                    }
                    index_table[kcount].key = strdup(key);
                    index_table[kcount].values = NULL;
                    index_table[kcount].vcount = 0;
                    index_table[kcount].vcap = 0;
                    found = kcount;
                    kcount++;
                }
                KeyIndex *ki = &index_table[found];
                if (ki->vcount >= ki->vcap) {
                    ki->vcap = ki->vcap == 0 ? 16 : ki->vcap * 2;
                    ki->values = realloc(ki->values, ki->vcap * sizeof(char *));
                }
                ki->values[ki->vcount] = strdup(val);
                ki->vcount++;
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *val = p + 1;
                while (*val == ' ') val++;
                char *val_end = val;
                while (*val_end && *val_end != ' ') val_end++;
                if (*val_end == ' ') *val_end = '\0';
                for (int i = 0; i < kcount; i++) {
                    if (strcmp(index_table[i].key, key) == 0) {
                        for (int j = 0; j < index_table[i].vcount; j++) {
                            if (strcmp(index_table[i].values[j], val) == 0) {
                                free(index_table[i].values[j]);
                                for (int k = j; k < index_table[i].vcount - 1; k++) {
                                    index_table[i].values[k] = index_table[i].values[k + 1];
                                }
                                index_table[i].vcount--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *key = line + 6;
            while (*key == ' ') key++;
            int found = -1;
            for (int i = 0; i < kcount; i++) {
                if (strcmp(index_table[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(index_table[found].key);
                for (int j = 0; j < index_table[found].vcount; j++) free(index_table[found].values[j]);
                free(index_table[found].values);
                for (int i = found; i < kcount - 1; i++) {
                    index_table[i] = index_table[i + 1];
                }
                kcount--;
            }
        } else if (strncmp(line, "VALUES ", 7) == 0) {
            char *key = line + 7;
            while (*key == ' ') key++;
            for (int i = 0; i < kcount; i++) {
                if (strcmp(index_table[i].key, key) == 0) {
                    for (int j = 0; j < index_table[i].vcount; j++) {
                        printf("%s\n", index_table[i].values[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(line, "KEYS") == 0) {
            for (int i = 0; i < kcount; i++) {
                printf("%s\n", index_table[i].key);
            }
        }
        free(line);
    }
    for (int i = 0; i < kcount; i++) {
        free(index_table[i].key);
        for (int j = 0; j < index_table[i].vcount; j++) free(index_table[i].values[j]);
        free(index_table[i].values);
    }
    free(index_table);
    return 0;
}
