// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Packet {
    char *id;
    int headerLen;
    int payloadLen;
    char *header;
    char *payload;
    struct Packet *next;
} Packet;

Packet *head = NULL, *tail = NULL;

Packet *find_packet(const char *id) {
    Packet *p = head;
    while (p) {
        if (strcmp(p->id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void add_packet(const char *id, int hLen, int pLen, const char *header, const char *payload) {
    Packet *p = malloc(sizeof(Packet));
    p->id = malloc(strlen(id) + 1);
    strcpy(p->id, id);
    p->headerLen = hLen;
    p->payloadLen = pLen;
    p->header = malloc(hLen + 1);
    memcpy(p->header, header, hLen);
    p->header[hLen] = '\0';
    p->payload = malloc(pLen + 1);
    memcpy(p->payload, payload, pLen);
    p->payload[pLen] = '\0';
    p->next = NULL;
    if (tail) {
        tail->next = p;
        tail = p;
    } else {
        head = tail = p;
    }
}

void drop_packet(const char *id) {
    Packet *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->id, id) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->id);
            free(curr->header);
            free(curr->payload);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void print_header(const char *id) {
    Packet *p = find_packet(id);
    if (p) printf("%s\n", p->header);
}

void print_payload(const char *id) {
    Packet *p = find_packet(id);
    if (p) printf("%s\n", p->payload);
}

void print_stats(void) {
    int total = 0;
    Packet *p = head;
    while (p) {
        total += p->payloadLen;
        p = p->next;
    }
    printf("%d\n", total);
}

void free_all(void) {
    Packet *p = head;
    while (p) {
        Packet *next = p->next;
        free(p->id);
        free(p->header);
        free(p->payload);
        free(p);
        p = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "PACKET") == 0) {
            char *id = strtok(NULL, " ");
            char *hLen_str = strtok(NULL, " ");
            char *pLen_str = strtok(NULL, " ");
            if (!id || !hLen_str || !pLen_str) continue;
            int hLen = atoi(hLen_str);
            int pLen = atoi(pLen_str);
            char *rest = strtok(NULL, "");
            if (!rest) continue;
            int restLen = strlen(rest);
            if (restLen < hLen + pLen) continue;
            char *header = malloc(hLen + 1);
            memcpy(header, rest, hLen);
            header[hLen] = '\0';
            char *payload = malloc(pLen + 1);
            memcpy(payload, rest + hLen, pLen);
            payload[pLen] = '\0';
            add_packet(id, hLen, pLen, header, payload);
            free(header);
            free(payload);
        } else if (strcmp(cmd, "DROP") == 0) {
            char *id = strtok(NULL, " ");
            if (id) drop_packet(id);
        } else if (strcmp(cmd, "HEADER") == 0) {
            char *id = strtok(NULL, " ");
            if (id) print_header(id);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char *id = strtok(NULL, " ");
            if (id) print_payload(id);
        } else if (strcmp(cmd, "STATS") == 0) {
            print_stats();
        }
    }
    free(line);
    free_all();
    return 0;
}