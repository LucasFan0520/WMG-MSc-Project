/* F031.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *name;
    char *note;
    struct Record *next;
} Record;

Record *head = NULL;
Record *tail = NULL;

Record *find_record(const char *name) {
    Record *r = head;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void append_record(const char *name, const char *note) {
    if (find_record(name)) return;
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->name = strdup(name);
    r->note = strdup(note);
    r->next = NULL;
    if (tail) tail->next = r;
    else head = r;
    tail = r;
}

void delete_record(const char *name) {
    Record *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_record_cmd(const char *name) {
    Record *r = find_record(name);
    if (!r) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s\n", r->name, r->note);
}

void report_records(void) {
    Record *r = head;
    while (r) {
        printf("%s %s\n", r->name, r->note);
        r = r->next;
    }
}

void free_all(void) {
    Record *r = head;
    while (r) {
        Record *tmp = r;
        r = r->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "BATCH") == 0) {
            int n;
            if (sscanf(line + 6, "%d", &n) == 1) {
                char **names = malloc(n * sizeof(char *));
                char **notes = malloc(n * sizeof(char *));
                int valid = 1;
                for (int i = 0; i < n; i++) {
                    char buf[1024];
                    if (!fgets(buf, sizeof(buf), stdin)) {
                        valid = 0;
                        break;
                    }
                    buf[strcspn(buf, "\n")] = '\0';
                    char name[256];
                    if (sscanf(buf, "%255s", name) == 1) {
                        char *note = buf;
                        while (*note && *note != ' ') note++;
                        while (*note && *note == ' ') note++;
                        names[i] = strdup(name);
                        notes[i] = strdup(note);
                    } else {
                        valid = 0;
                        break;
                    }
                }
                if (valid) {
                    for (int i = 0; i < n; i++) {
                        append_record(names[i], notes[i]);
                    }
                }
                for (int i = 0; i < n; i++) {
                    free(names[i]);
                    free(notes[i]);
                }
                free(names);
                free(notes);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_record(p);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_record_cmd(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    free_all();
    return 0;
}