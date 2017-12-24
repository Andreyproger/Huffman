#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define forn(i, a, b) for (int i = (a); i < (b); i++)
#define nil NULL
#define createInHeap(T) ((T*) malloc(sizeof(T)))
#define createArrayInHeap(T, size) ((T*) malloc(sizeof(T) * (size)))

//-----------
#define Vec(id) struct vec_##id
#define vecStructDecl(T, id) \
Vec(id) {     \
    T *elems; \
    int size; \
    int cap;  \
};

#define emptyVecDecl(T, id)                          \
Vec(id) emptyVec_##id() {                            \
    Vec(id) res = { createArrayInHeap(T, 2), 0, 2 }; \
    return res;                                      \
}

#define atDecl(T, id)          \
T at_##id(Vec(id) *v, int i) { \
    return v->elems[i];        \
}

#define setDecl(T, id)                    \
void set_##id(Vec(id) *v, int i, T val) { \
    v->elems[i] = val;                    \
}

#define pushDecl(T, id)                         \
void push_##id(Vec(id) *v, T val) {             \
    if (v->size < v->cap) {                     \
        v->elems[v->size] = val;                \
        v->size++;                              \
        return;                                 \
    }                                           \
                                                \
    T *next = createArrayInHeap(T, v->cap * 2); \
    forn (i, 0, v->size)                        \
        next[i] = v->elems[i];                  \
    free(v->elems);                             \
    v->elems = next;                            \
    v->cap = v->cap * 2;                        \
    push_##id(v, val);                          \
}

#define insertDecl(T, id)                                \
void insert_##id(Vec(id) *v, int count, T val) { \
    forn (i, 0, count)                                   \
        push_##id(v, val);                               \
}

#define sizeDecl(T, id) \
int size_##id(Vec(id) *v) { return v->size; }

#define pushValuesDecl(T, id) \
void pushValues_##id(Vec(id) *v, Vec(id) *other) { \
    forn (i, 0, other->size) push_##id(v, at_##id(other, i));      \
}

#define cloneDecl(T, id)                         \
Vec(id) clone_##id(Vec(id) *v) { \
    Vec(id) res = emptyVec_##id();       \
    pushValues_##id(&res, v);                    \
    return res;                                  \
}

#define sortDecl(T, id) \
void sort_##id(Vec(id) *v, int (*compare)(const void*, const void*)) { \
    qsort(v->elems, v->size, sizeof(T), compare);                      \
}

#define popDecl(T, id) \
T pop_##id(Vec(id) *v) { T result = v->elems[v->size - 1]; v->size--; return result; }

#define finalizeDecl(T, id)      \
void finalize_##id(Vec(id) *v) { \
    free(v->elems);              \
    v->size = -1;                \
    v->cap  = -1;                \
}

#define vecDecl(T, id)    \
    vecStructDecl(T, id)  \
    emptyVecDecl(T, id)   \
    atDecl(T, id)         \
    setDecl(T, id)        \
    pushDecl(T, id)       \
    insertDecl(T, id)     \
    sizeDecl(T, id)       \
    pushValuesDecl(T, id) \
    cloneDecl(T, id)      \
    sortDecl(T, id)       \
    popDecl(T, id)        \
    finalizeDecl(T, id)

//-----------

vecDecl(bool, b)
vecDecl(unsigned char, c)
vecDecl(int, i)

//-----------
unsigned char asUnsigendChar(char ch) {
    unsigned char *res = (unsigned char*) &ch;
    return *res;
}
//-----------

#define Freq struct sFreq
Freq {
    unsigned char ch;
    int count;
};
Freq createFreq(unsigned char ch, int count) {
    Freq res = { ch, count };
    return res;
}
vecDecl(Freq, f)

Vec(f) countFreq(const unsigned char *str) {
    Vec(i) counts = emptyVec_i();
    insert_i(&counts, 256, 0);

    int len = (int) strlen((char*)str);
    forn (i, 0, len) set_i(&counts, str[i], at_i(&counts, str[i]) + 1);

    Vec(f) result = emptyVec_f();
    forn(i, 0, 256)
        if (at_i(&counts, i) != 0)
            push_f( &result, createFreq((unsigned char) i, at_i(&counts, i)) );

    finalize_i(&counts);
    return result;
}

#define Tree struct sTree
Tree {
    Tree *l, *r;
    unsigned char ch; // opt
    int weight;
};

Tree createTree(Tree *l, Tree *r, unsigned char ch, int weight) {
    Tree res = { l, r, ch, weight };
    return res;
}

Tree* createTreeInHeap(Tree *l, Tree *r, unsigned char ch, int weight) {
    Tree *res = createInHeap(Tree);
    *res = createTree(l, r, ch, weight);
    return res;
}

Tree* createEmptyTreeInHeap() {
    return createTreeInHeap(nil, nil, '\0', -1);
}

bool isFinal(Tree *t) {
    return t->l == nil && t->r == nil;
}
vecDecl(Tree *, ts)


Tree *unite(Tree *a, Tree *b) {
    return createTreeInHeap(a, b, (char)0, a->weight + b->weight);
}

void deleteTree(Tree *t) {
    if (!isFinal(t)) {
        deleteTree(t->l);
        deleteTree(t->r);
    }
    free(t);
}

Vec(ts) toSmallTrees(Vec(f) freqs) {
    Vec(ts) smallTrees = emptyVec_ts();
    forn (i, 0, freqs.size) {
        Tree *toInsert = createTreeInHeap(nil, nil, at_f(&freqs, i).ch, at_f(&freqs, i).count);
        push_ts(&smallTrees, toInsert);
    }
    return smallTrees;
}

int treeCompare(const void* a, const void* b) {
    return ((Tree*)b)->weight - ((Tree*)a)->weight;
}
Tree *makeTree(const unsigned char* str) {
    Vec(f) freqs = countFreq(str);
    Vec(ts) trees = toSmallTrees(freqs);
    finalize_f(&freqs);

    while (trees.size != 1) {
        sort_ts(&trees, treeCompare);
        Tree *a = pop_ts(&trees);
        Tree *b = pop_ts(&trees);

        push_ts(&trees, unite(a, b));
    }

    Tree *res = pop_ts(&trees);
    finalize_ts(&trees);
    return res;
}

void buildCodes(Tree *t, Vec(b) curCode, Vec(b) *codesOut /*array of codes*/) {
    if (isFinal(t))
        codesOut[t->ch] = clone_b(&curCode);
    else {
        Vec(b) forL = clone_b(&curCode);
        push_b(&forL, (bool)0);
        buildCodes(t->l, forL, codesOut);

        Vec(b) forR = clone_b(&curCode);
        push_b(&forR, (bool)1);
        buildCodes(t->r, forR, codesOut);

        finalize_b(&forL);
        finalize_b(&forR);
    }
}

Vec(b) *getCodes(Tree *t) {
    Vec(b) *res = createArrayInHeap(Vec(b), 256);
    forn (i, 0, 256)
        res[i] = emptyVec_b();
    buildCodes(t, emptyVec_b(), res);
    return res;
}

Vec(b) encodeString(unsigned char *str, Vec(b) *codes) {
    Vec(b) res = emptyVec_b();
    int len = (int) strlen((char*) str);
    forn(i, 0, len) {
        Vec(b) encodedChar = codes[str[i]];
        pushValues_b(&res, &encodedChar);
    }
    return res;
}

Vec(c) decodeFromPos(Tree *t, Vec(b) *encoded, int startPos, int endPos) {
    Tree *root = t;
    Tree *cur = t;

    Vec(c) res = emptyVec_c();
    forn (i, startPos, endPos) {
        if (at_b(encoded, i) == 0)
            cur = cur->l;
        else
            cur = cur->r;

        if (isFinal(cur)) {
            push_c(&res, cur->ch);
            cur = root;
        }
    }

    return res;
}

Vec(c) decode(Tree *t, Vec(b) *encoded) {
    return decodeFromPos(t, encoded, 0, encoded->size);
}

Vec(c) to_string(Vec(b) *v) {
    Vec(c) res = emptyVec_c();
    forn (i, 0, size_b(v)) {
        push_c(&res, asUnsigendChar((at_b(v, i) == true) ? '1' : '0'));
    }
    return res;
}

void printString(Vec(c) *str) {
    push_c(str, '\0');
    printf("%s", str->elems);
    pop_c(str);
}

void printStringln(Vec(c) *str) {
    printString(str);
    printf("\n");
}

void printAsStringln(Vec(b) *v) {
    Vec(c) asString = to_string(v);
    printStringln(&asString);
    finalize_c(&asString);
}

//------

#define numToVecDecl(T, len) \
Vec(b) T##_ToVec(T num) { \
    Vec(b) res = emptyVec_b(); \
    forn(i, 0, len) { \
        push_b(&res, (num & (1 << i)) == 0 ? false : true); \
    } \
    return res; \
}

#define appendNumToVecDecl(T, len) \
void append_##T(Vec(b) *v, T num) { \
    Vec(b) numCode = T##_ToVec(num); \
    pushValues_b(v, &numCode); \
    finalize_b(&numCode); \
}

#define getNumFromVecDecl(T, len) \
T get_##T##_FromPos(Vec(b) *v, int pos) { \
    T result = 0; \
    forn (i, 0, len) { \
        if (i + pos < v->size && at_b(v, i + pos) != 0) \
            result |= 1 << i; \
    } \
    return result; \
}

#define numVecDecls(T, len) \
    numToVecDecl(T, len) \
    appendNumToVecDecl(T, len) \
    getNumFromVecDecl(T, len) \

numVecDecls(char, 8)
numVecDecls(int, 32)

//---

#define QNode struct sQNode
QNode {
    QNode *next;
    Tree *val;
};
#define Q struct sQ
Q {
    QNode *begin;
    QNode *end;
};

Tree *pop(Q *q) {
    Tree *res = q->begin->val;
    if (q->begin == q->end) {
        free(q->begin);
        q->begin = q->end = nil;
    } else {
        free(q->begin);
        q->begin = q->begin->next;
    }
    return res;
}

void push(Q *q, Tree *t) {
    if (q->end == nil) {
        q->begin = q->end = createInHeap(QNode);
    } else {
        q->end->next = createInHeap(QNode);
        q->end = q->end->next;
    }

    q->end->next = nil;
    q->end->val  = t;
}

Vec(b) treeToVec(Tree *root) {
    Vec(b) res = emptyVec_b();
    Q q = { nil, nil };
    push(&q, root);
    while (q.begin != nil) {
        Tree *t = pop(&q);
        if (isFinal(t)) {
            push_b(&res, 0);
            append_char(&res, t->ch);
        } else {
            push_b(&res, 1);
            push(&q, t->l);
            push(&q, t->r);
        }
    }
    return res;
}

Tree* vecToTree(Vec(b) *v, /*out*/ int *lastPos) {
    Tree *root = createEmptyTreeInHeap();
    Q q = { nil, nil };
    push(&q, root);
    for (int i = 0; i < v->size; ) {
        if (q.begin == nil) {
            *lastPos = i;
            break;
        }

        Tree *cur = pop(&q);
        if (at_b(v, i) == 0) {
            cur->ch = asUnsigendChar(get_char_FromPos(v, i + 1));
            i += 9;
        } else {
            cur->l = createEmptyTreeInHeap();
            cur->r = createEmptyTreeInHeap();
            push(&q, cur->l);
            push(&q, cur->r);
            i++;
        }
    }
    return root;
}

void printTreeAsVec(Tree *t) {
    Vec(b) v = treeToVec(t);
    Vec(c) str = to_string(&v);

    printStringln(&str);
    finalize_c(&str);
    finalize_b(&v);
}

Vec(b) createFullCode(unsigned char *str) {
    Tree *t = makeTree(str);
    Vec(b) *codes = getCodes(t);
    Vec(b) encodedStr = encodeString(str, codes);
    Vec(b) encodedTree = treeToVec(t);

    Vec(b) code = emptyVec_b();
    pushValues_b(&code, &encodedTree);
    append_int(&code, encodedStr.size);
    pushValues_b(&code, &encodedStr);

    deleteTree(t);
    forn (i, 0, 256)
        finalize_b(&codes[i]);
    free(codes);
    finalize_b(&encodedStr);
    finalize_b(&encodedTree);

    return code;
}

Vec(c) decodeFromFullCode(Vec(b) *code) {
    int lenPos;
    Tree *t = vecToTree(code, &lenPos);
    int length = get_int_FromPos(code, lenPos);
    int encodedStart = lenPos + 32;
    int encodedEnd   = encodedStart + length;
    Vec(c) decoded = decodeFromPos(t, code, encodedStart, encodedEnd);

    deleteTree(t);

    return decoded;
}

void writeVecC(Vec(c) *v, FILE *out) {
    forn (i, 0, v->size) {
        char ch = at_c(v, i);
        fwrite(&ch, 1, 1, out);
    }
}

void writeVecB(Vec(b) *v, FILE *out) {
    forn (i, 0, (v->size + 7) / 8) {
        char ch = get_char_FromPos(v, i*8);
        fwrite(&ch, 1, 1, out);
    }
}

Vec(b) readVecBFrom(FILE *in) {
    Vec(b) res = emptyVec_b();
    char readed;
    while (fread(&readed, 1, 1, in) != 0) {
        append_char(&res, readed);
    }
    return res;
}

void encoderMode() {
    Vec(c) readed = emptyVec_c();
    FILE *in = fopen("input.txt", "r");
    unsigned char ch;
    while (fread(&ch, 1, 1, in) != 0) {
        push_c(&readed, ch);
    }
    push_c(&readed, '\0');
    fclose(in);

    Vec(b) code = createFullCode(readed.elems);

    FILE *out = fopen("output.txt", "w");
    writeVecB(&code, out);
    fclose(out);
}

void decoderMode() {
    FILE *in = fopen("output.txt", "r");
    Vec(b) readed = readVecBFrom(in);
    Vec(c) decoded = decodeFromFullCode(&readed);
    fclose(in);

    FILE *out = fopen("output2.txt", "w");
    writeVecC(&decoded, out);
    fclose(out);
}

int main() {
#ifdef MISTMAC
    freopen("input.txt", "r", stdin);
    //freopen("output.txt", "w", stdout);
#endif

    encoderMode();
    decoderMode();
    return 0;
}
