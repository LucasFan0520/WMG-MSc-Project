// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    char *note;
    char **labels;
    size_t lcount;
    size_t lcap;
} TrackerItem;

int main() {
    TrackerItem *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ITEM ", 5) == 0) {
            char *name_start = line + 5;
            char *space = strchr(name_start, ' ');
            char *note_start = "";
            if (space) {
                *space = '\0';
                note_start = space + 1;
            }
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                arr = realloc(arr, cap * sizeof(TrackerItem));
            }
            arr[count].name = strdup(name_start);
            arr[count].note = strdup(note_start);
            arr[count].labels = NULL;
            arr[count].lcount = 0;
            arr[count].lcap = 0;
            count++;
        } else if (strncmp(line, "LABEL ", 6) == 0) {
            char *name_start = line + 6;
            char *space = strchr(name_start, ' ');
            if (space) {
                *space = '\0';
                char *lbl = space + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name_start) == 0) {
                        TrackerItem *item = &arr[i];
                        if (item->lcount >= item->lcap) {
                            item->lcap = item->lcap == 0 ? 4 : item->lcap * 2;
                            item->labels = realloc(item->labels, item->lcap * sizeof(char *));
                        }
                        item->labels[item->lcount++] = strdup(lbl);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "UNLABEL ", 8) == 0) {
            char *name_start = line + 8;
            char *space = strchr(name_start, ' ');
            if (space) {
                *space = '\0';
                char *lbl = space + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name_start) == 0) {
                        TrackerItem *item = &arr[i];
                        for (size_t j = 0; j < item->lcount; j++) {
                            if (strcmp(item->labels[j], lbl) == 0) {
                                free(item->labels[j]);
                                for (size_t k = j; k < item->lcount - 1; k++) {
                                    item->labels[k] = item->labels[k + 1];
                                }
                                item->lcount--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name_start = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name_start) == 0) {
                    free(arr[i].name);
                    free(arr[i].note);
                    for (size_t j = 0; j < arr[i].lcount; j++) {
                        free(arr[i].labels[j]);
                    }
                    free(arr[i].labels);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name_start = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name_start) == 0) {
                    printf("%s %s", arr[i].name, arr[i].note);
                    for (size_t j = 0; j < arr[i].lcount; j++) {
                        printf(" %s", arr[i].labels[j]);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].name, arr[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].note);
        for (size_t j = 0; j < arr[i].lcount; j++) {
            free(arr[i].labels[j]);
        }
        free(arr[i].labels);
    }
    free(arr);
    return 0;
}
