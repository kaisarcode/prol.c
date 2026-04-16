/**
 * prol - Probable language detector (v0.4.0)
 * 100% logic alignment with kc-tpm (Text Profile Matcher).
 * Single-file integrated implementation for language detection.
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>

#define PROL_VERSION "0.4.0"
#define PROL_NG_SIZE 3
#define PROL_MAX_LANGS 32
#define PROL_MAX_PROF 2048

typedef struct { char g[12]; int n; } KcG;
typedef struct { const char *c; const char *s; KcG p[PROL_MAX_PROF]; int p_sz; long tot; } KcL;

/* Training data - Using more representative text to match kc-tpm behavior */
static KcL ds[PROL_MAX_LANGS] = {
    {"en", "the quick brown fox jumps over the lazy dog. hello my friends how are you? good morning everyone. this project matches short text using n-gram profiles. it is light and fast. documentation is key for any project to succeed. code logic should be clean."},
    {"es", "el zorro marrón salta sobre el perro perezoso. hola mis amigos ¿cómo están? buenos días a todos. este proyecto compara texto corto usando perfiles de n-gramas. es ligero y rápido. la documentación es clave para que cualquier proyecto tenga éxito."},
    {"pt", "a rápida raposa marrom salta sobre o cão preguiçoso. olá meus amigos como vocês estão? bom dia a todos. este projeto compara texto curto usando perfis de n-gramas. é leve e rápido. a documentação é fundamental para o sucesso de qualquer projeto."},
    {"fr", "le renard brun rapide saute par-dessus le chien paresseux. bonjour mes amis, comment allez-vous? bonne matinée à tous. ce projet compare les textes courts à l'aide de profils de n-grammes. c'est léger et rapide. la documentation est primordiale."},
    {"it", "la volpe marrone veloce salta sopra il cane pigro. ciao amici come state? buongiorno a tutti. questo progetto confronta testi brevi utilizzando profili n-gram. è leggero e veloce. la documentazione è la chiave per il successo di ogni progetto."},
    {"de", "der schnelle braune fuchs springt über den faulen hund. hallo meine freunde, wie geht es euch? guten morgen allerseits. dieses projekt vergleicht kurzen text mithilfe von n-gram-profilen. es ist leicht und schnell. dokumentation ist wichtig."},
    {NULL, NULL, {{""}, 0}, 0, 0}
};

static int u8_len(unsigned char c) {
    if (c < 0x80) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

/* Normalizes text (lowercase, collapse spaces, trim) - Matches kc_tpm_norm */
static char* norm(const char *in) {
    if (!in) return NULL;
    size_t len = strlen(in);
    char *out = malloc(len + 1);
    if (!out) return NULL;
    
    size_t j = 0;
    int in_space = 1;
    for (size_t i = 0; i < len; i++) {
        unsigned char c = (unsigned char)in[i];
        if (isspace(c)) {
            if (!in_space) { out[j++] = ' '; in_space = 1; }
        } else {
            if (c >= 'A' && c <= 'Z') out[j++] = (char)(c + 32);
            else out[j++] = (char)c;
            in_space = 0;
        }
    }
    if (j > 0 && out[j-1] == ' ') j--;
    out[j] = '\0';
    return out;
}

static void train(KcL *l) {
    if (l->p_sz > 0) return;
    char *nt = norm(l->s);
    int len = strlen(nt);
    for (int i = 0; i <= len - PROL_NG_SIZE; i += u8_len((unsigned char)nt[i])) {
        char g[12] = {0}; const char *it = nt + i; int bc = 0, cc = 0;
        while (cc < PROL_NG_SIZE && (it - nt) < len) {
            int cl = u8_len((unsigned char)*it);
            if (bc + cl >= 12) break;
            memcpy(g + bc, it, cl); bc += cl; it += cl; cc++;
        }
        if (cc == PROL_NG_SIZE) {
            int f = 0;
            for (int j = 0; j < l->p_sz; j++)
                if (!strcmp(l->p[j].g, g)) { l->p[j].n++; f = 1; break; }
            if (!f && l->p_sz < PROL_MAX_PROF) {
                strcpy(l->p[l->p_sz].g, g); l->p[l->p_sz].n = 1; l->p_sz++;
            }
            l->tot++;
        }
    }
    free(nt);
}

static double calc_score(const char *txt, KcL *l) {
    char *nt = norm(txt);
    if (!nt) return 0.0;
    int len = strlen(nt);
    double ls = 0.0; int tg = 0;
    for (int i = 0; i <= len - PROL_NG_SIZE; i += u8_len((unsigned char)nt[i])) {
        char g[12] = {0}; const char *it = nt + i; int bc = 0, cc = 0;
        while (cc < PROL_NG_SIZE && (it - nt) < len) {
            int cl = u8_len((unsigned char)*it);
            if (bc + cl >= 12) break;
            memcpy(g + bc, it, cl); bc += cl; it += cl; cc++;
        }
        if (cc == PROL_NG_SIZE) {
            int n = 0;
            for (int j = 0; j < l->p_sz; j++)
                if (!strcmp(l->p[j].g, g)) { n = l->p[j].n; break; }
            ls += log(((double)n + 1.0) / ((double)l->tot + l->p_sz));
            tg++;
        }
    }
    free(nt);
    if (!tg) return 0.0;
    return 1.0 / (1.0 + exp(-8.0 * ((ls / tg) - (-5.25))));
}

typedef struct { const char *c; double s; } R;
static int cmp(const void *a, const void *b) {
    if (((R*)b)->s > ((R*)a)->s) return 1;
    return (((R*)b)->s < ((R*)a)->s) ? -1 : 0;
}

int main(int argc, char **argv) {
    double th = 0.001; int lim = 1; const char *txt = NULL; char buf[8192];
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-t") && i + 1 < argc) th = atof(argv[++i]);
        else if (!strcmp(argv[i], "-l") && i + 1 < argc) lim = atoi(argv[++i]);
        else if (argv[i][0] != '-') txt = argv[i];
    }
    if (!txt) { int n = fread(buf, 1, sizeof(buf) - 1, stdin); if (n > 0) { buf[n] = 0; txt = buf; } }
    if (!txt || !*txt) return 0;
    R r[PROL_MAX_LANGS]; int n = 0;
    while (ds[n].c) { train(&ds[n]); r[n].c = ds[n].c; r[n].s = calc_score(txt, &ds[n]); n++; }
    qsort(r, n, sizeof(R), cmp);
    for (int i = 0; i < n && i < lim; i++) {
        if (r[i].s >= th) {
            if (lim == 1) printf("%s\n", r[i].c);
            else printf("%s: %.4f\n", r[i].c, r[i].s);
        }
    }
    return 0;
}
