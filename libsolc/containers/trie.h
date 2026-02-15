#ifndef __SOLC_CONTAINER_TRIE_H__
#define __SOLC_CONTAINER_TRIE_H__

typedef struct __trie_t trie_t;

trie_t *trie_create(void);
void trie_destroy(trie_t *trie);

void trie_insert(trie_t *trie, const char *str, void *data_ptr);
void *trie_get(trie_t *trie, const char *str);

#endif // __SOLC_CONTAINER_TRIE_H__
