/* F020.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char **body;
    int bcount;
    int bcap;
} Record;

static Record *records = NULL;
static int rcount = 0;
static int rcap = 0;

static int ensure_rcap(void) {
    if (rcount >= rcap) {
        int newcap = rcap == 0 ? 8 : rcap * 2;
        Record *tmp = realloc(records, sizeof(Record) * newcap);
        if (!tmp) return -1;
        records = tmp;
        rcap = newcap;
    }
    return 0;
}

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_record(const char *title) {
    for (int i = 0; i < rcount; i++) {
        if (strcmp(records[i].title, title) == 0)
            return i;
    }
    return -1;
}

static int record_add_body(Record *r, const char *line) {
    if (r->bcount >= r->bcap) {
        int newcap = r->bcap == 0 ? 4 : r->bcap * 2;
        char **tmp = realloc(r->body, sizeof(char *) * newcap);
        if (!tmp) return -1;
        r->body = tmp;
        r->bcap = newcap;
    }
    r->body[r->bcount] = dup_str(line);
    if (!r->body[r->bcount]) return -1;
    r->bcount++;
    return 0;
}

static void free_record(Record *r) {
    free(r->title);
    for (int i = 0; i < r->bcount; i++)
        free(r->body[i]);
    free(r->body);
}

static void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    int in_record = 0;
    int cur = -1;
    while (fgets(line, sizeof(line), f)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (len > 1 && line[len - 2] == '\r') line[len - 2] = '\0';
        if (strcmp(line, "--") == 0) {
            in_record = 0;
            cur = -1;
            continue;
        }
        if (!in_record) {
            if (ensure_rcap() != 0) continue;
            records[rcount].title = dup_str(line);
            records[rcount].body = NULL;
            records[rcount].bcount = 0;
            records[rcount].bcap = 0;
            if (!records[rcount].title) continue;
            cur = rcount;
            rcount++;
            in_record = 1;
        } else {
            if (cur >= 0) {
                record_add_body(&records[cur], line);
            }
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "FIND") == 0) {
            const char *title = line + offset;
            while (*title == ' ') title++;
            int idx = find_record(title);
            if (idx >= 0) {
                printf("%s\n", records[idx].title);
                for (int i = 0; i < records[idx].bcount; i++)
                    printf("%s\n", records[idx].body[i]);
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            const char *title = line + offset;
            while (*title == ' ') title++;
            int idx = find_record(title);
            if (idx < 0) continue;
            free_record(&records[idx]);
            for (int i = idx; i < rcount - 1; i++)
                records[i] = records[i + 1];
            rcount--;
        } else if (strcmp(cmd, "ADD") == 0) {
            const char *title = line + offset;
            while (*title == ' ') title++;
            if (find_record(title) >= 0) continue;
            if (ensure_rcap() != 0) continue;
            records[rcount].title = dup_str(title);
            records[rcount].body = NULL;
            records[rcount].bcount = 0;
            records[rcount].bcap = 0;
            if (!records[rcount].title) continue;
            int cur = rcount;
            rcount++;
            while (fgets(line, sizeof(line), stdin)) {
                len = strlen(line);
                if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
                if (strcmp(line, "--") == 0) break;
                record_add_body(&records[cur], line);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < rcount; i++) {
                printf("%s\n", records[i].title);
                for (int j = 0; j < records[i].bcount; j++)
                    printf("%s\n", records[i].body[j]);
                printf("--\n");
            }
        }
    }
    for (int i = 0; i < rcount; i++)
        free_record(&records[i]);
    free(records);
    return 0;
}