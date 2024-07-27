// DO NOT MODIFY THIS FILE
#ifndef __BTREE_H__
#define __BTREE_H__

// Note: Your code must work with any value of MAX_SUBTREES.
#define MAX_SUBTREES (4)
#define MAX_KEYS ((MAX_SUBTREES)-1)
#define MIN_KEYS ((MAX_KEYS) / 2)
#define MIN_SUBKEYS ((MIN_KEYS) + 1)

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

typedef struct _BTreeNode
{
	int keys[MAX_KEYS];
	int num_keys;
	struct _BTreeNode *subtrees[MAX_SUBTREES];
} BTreeNode;

//____________________________________________________________
// GENERAL -- Implemented in btree.c.
//

// PUBLIC interface
void free_btree(BTreeNode **a_root);
bool does_contain_key(BTreeNode *root, int key_to_find);
size_t count_keys(BTreeNode *root);

// SUPPORT function(s) -- might be used in multiple files.
BTreeNode *choose_subtree_for_key(BTreeNode *node, int new_key);

//____________________________________________________________
// DELETE -- Implemented in btree_delete.c
//

// SUPPORT type -- would not be used by external users but might be used in testing
typedef struct
{
	int key;
	BTreeNode *node;
	int key_idx;
} KeyLocation;

bool delete_key(BTreeNode **a_root, int key);
void merge_subtree_with_next(BTreeNode *parent, int subtree_idx);
void shift_key_forward(BTreeNode *parent, int src_subtree_idx);
void shift_key_backward(BTreeNode *parent, int src_subtree_idx);
KeyLocation get_in_order_predecessor(KeyLocation key_location);
KeyLocation get_in_order_successor(KeyLocation key_location);

//____________________________________________________________
// INSERT -- Implemented in btree_insert.c
//
bool insert(BTreeNode **a_root, int new_key);

//____________________________________________________________
// WRAPPER functions -- Implemented in btree_wrappers.c
//
// These wrapper functions take the place of directly accessing subtrees within nodes
// and creating new BTreeNode objects.
//

BTreeNode *malloc_btree_node();
// └▶  malloc(sizeof *new_btree_node)

BTreeNode *get_subtree(BTreeNode *node, int subtree_idx);
// └▶  node -> subtrees[subtree_idx]

void set_subtree(BTreeNode *node, int subtree_idx, BTreeNode *new_subtree);
// └▶  node -> subtrees[subtree_idx] = new_subtree;

BTreeNode **get_address_of_subtree(BTreeNode *node, int subtree_idx);
// └▶  &(node -> subtrees[subtree_idx])

//____________________________________________________________
// QUERY functions -- Implemented in btree_query.c
//

typedef struct
{
	int *keys;
	size_t num_keys;
} KeyQueryResult;
// For clarity/convenience: This type lets you return an array of keys along with its size.

int query_min(BTreeNode *root);
int query_max(BTreeNode *root);
KeyQueryResult query_k_largest(BTreeNode *root, size_t k);
KeyQueryResult query_k_smallest(BTreeNode *root, size_t k);

//____________________________________________________________
// PRINTING -- Implemented in btree_print.c.
//

// PUBLIC interface
void print_btree(BTreeNode *root);
size_t print_keys(BTreeNode *root, char const *prefix, char const *delimiter, char const *suffix);

//____________________________________________________________
// SUPPORT ASSERTIONS -- Implemented in btree_assert.c.
//
void assert_node_keys_lt(BTreeNode *node, int key_value);
void assert_node_keys_gt(BTreeNode *node, int key_value);
void assert_node_keys_in_ascending_order(BTreeNode *node);
void assert_node_correct(BTreeNode *node);

#endif /* end of include guard: __BTREE_H__ */
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab cc=110: */
