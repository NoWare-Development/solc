#include "containers/trie.h"
#include "solc/defs.h"
#include <stdlib.h>

typedef struct __trie_node_t {
  struct __trie_node_t *children[0x100];
  void *data_ptr;
} trie_node_t;

typedef struct __trie_t {
  trie_node_t *root;
} trie_t;

static void trie_node_destroy(trie_node_t *node);

trie_t *trie_create(void)
{
  trie_t *out_trie = malloc(sizeof(trie_t));
  out_trie->root = calloc(1, sizeof(trie_node_t));
  return out_trie;
}

void trie_destroy(trie_t *trie)
{
  SOLC_ASSUME(trie != nullptr);
  trie_node_destroy(trie->root);
  free(trie);
}

void trie_insert(trie_t *trie, const char *str, void *data_ptr)
{
  SOLC_ASSUME(trie != nullptr && str != nullptr && data_ptr != nullptr);
  trie_node_t *cur = trie->root;
  for (; *str; str++) {
    if (cur->children[(sz)*str] == nullptr) {
      trie_node_t *new_node = calloc(1, sizeof(trie_node_t));
      cur->children[(sz)*str] = new_node;
    }
    cur = cur->children[(sz)*str];
  }

  cur->data_ptr = data_ptr;
}

void *trie_get(trie_t *trie, const char *str)
{
  SOLC_ASSUME(trie != nullptr && str != nullptr);
  trie_node_t *cur = trie->root;
  for (; *str; str++) {
    if (cur->children[(sz)*str] == nullptr)
      return nullptr;
    cur = cur->children[(sz)*str];
  }
  return cur->data_ptr;
}

static void trie_node_destroy(trie_node_t *node)
{
  for (sz i = 0; i < 0x100; i++) {
    if (node->children[i] != nullptr)
      trie_node_destroy(node->children[i]);
  }
  free(node);
}
