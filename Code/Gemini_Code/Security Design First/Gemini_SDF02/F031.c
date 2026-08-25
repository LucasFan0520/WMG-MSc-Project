/* F031.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Record;

int main(void) {
    Record *db = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) break;
        while (*p == ' ') p++;

        if (strcmp(cmd, "BATCH") == 0) {
            int n = atoi(p);
            if (n <= 0) continue;
            char **batch_lines = malloc((size_t)n * sizeof(char *));
            if (!batch_lines) continue;
            int valid = 1;
            for (int i = 0; i < n; i++) {
                char *b_line = NULL;
                size_t b_len = 0;
                if (getline(&b_line, &b_len, stdin) == -1) {
                    valid = 0;
                    free(b_line);
                    for (int j = 0; j < i; j++) free(batch_lines[j]);
                    break;
                }
                size_t blen = strlen(b_line);
                while (blen > 0 && (b_line[blen - 1] == '\n' || b_line[blen - 1] == '\r')) {
                    b_line[blen - 1] = '\0';
                    blen--;
                }
                char *bp = b_line;
                while (*bp == ' ') bp++;
                if (*bp == '\0') valid = 0;
                char *sp = bp;
                while (*sp && *sp != ' ') sp++;
                if (*sp == '\0') valid = 0;
                batch_lines[i] = b_line;
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    char *bp = batch_lines[i];
                    while (*bp == ' ') bp++;
                    char *name = bp;
                    char *sp = bp;
                    while (*sp && *sp != ' ') sp++;
                    if (*sp) { *sp = '\0'; sp++; }
                    while (*sp == ' ') sp++;
                    char *note = sp;

                    if (count >= capacity) {
                        size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                        Record *temp = realloc(db, new_cap * sizeof(Record));
                        if (!temp) break;
                        db = temp;
                        capacity = new_cap;
                    }
                    db[count].name = strdup(name);
                    db[count].note = strdup(note);
                    if (db[count].name && db[count].note) count++;
                }
            }
            for (int i = 0; i < n; i++) {
                if (i < n && batch_lines[i]) free(batch_lines[i]);
            }
            free(batch_lines);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            for (size_t i = 0; i < count; ) {
                if (strcmp(db[i].name, name) == 0) {
                    free(db[i].name);
                    free(db[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        db[j] = db[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(db[i].name, name) == 0) {
                    printf("%s %s\n", db[i].name, db[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", db[i].name, db[i].note);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(db[i].name);
        free(db[i].note);
    }
    free(db);
    return 0;
}
