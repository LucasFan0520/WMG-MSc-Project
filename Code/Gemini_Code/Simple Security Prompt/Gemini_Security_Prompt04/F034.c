// F034.c
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
    char *name;
    char *note;
} NoteItem;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    NoteItem *store = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(line, "LOAD") == 0) {
            FILE *fp = fopen(argv[1], "r");
            if (fp) {
                NoteItem *temp = NULL;
                int t_count = 0;
                int t_cap = 0;
                int valid = 1;
                while (1) {
                    char *fline = read_line(fp);
                    if (!fline) break;
                    char *p = fline;
                    while (*p == ' ') p++;
                    if (*p == '\0') {
                        free(fline);
                        continue;
                    }
                    char *space = strchr(p, ' ');
                    if (!space) {
                        valid = 0;
                        free(fline);
                        break;
                    }
                    *space = '\0';
                    char *note = space + 1;
                    while (*note == ' ') note++;
                    if (t_count >= t_cap) {
                        t_cap = t_cap == 0 ? 16 : t_cap * 2;
                        temp = realloc(temp, t_cap * sizeof(NoteItem));
                    }
                    temp[t_count].name = strdup(p);
                    temp[t_count].note = strdup(note);
                    t_count++;
                    free(fline);
                }
                fclose(fp);
                if (valid) {
                    for (int i = 0; i < count; i++) {
                        free(store[i].name);
                        free(store[i].note);
                    }
                    free(store);
                    store = temp;
                    count = t_count;
                    cap = t_cap;
                } else {
                    for (int i = 0; i < t_count; i++) {
                        free(temp[i].name);
                        free(temp[i].note);
                    }
                    free(temp);
                }
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *note = p + 1;
                while (*note == ' ') note++;
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    store = realloc(store, cap * sizeof(NoteItem));
                }
                store[count].name = strdup(name);
                store[count].note = strdup(note);
                count++;
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(store[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(store[found].name);
                free(store[found].note);
                for (int i = found; i < count - 1; i++) {
                    store[i] = store[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", store[i].name, store[i].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].note);
    }
    free(store);
    return 0;
}
