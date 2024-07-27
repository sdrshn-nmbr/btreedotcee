// DO NOT MODIFY THIS FILE
// You may modify it temporarily for your own testing, but your code must work with the original.

#include "btree.h"

/////////////////////////////////////////////////////////////////////
// WRAPPER functions
//
// Use get_subtree(…), get_address_of_subtree(…), and set_subtree(…) instead of
// accessing 'node -> subtrees[…]' directly.
//
// These allow us to check the correctness of your code.  We may modify these functions
// to track which nodes were accessed.
//
// There is also a wrapper for malloc(sizeof *new_btree_node) but you should not need that
// for the homework.  We use that in btree_insert.c to track the nodes that were created.

BTreeNode* get_subtree(BTreeNode* node, int subtree_idx) {
	// We might add additional code to this function when testing your submission.
	return node -> subtrees[subtree_idx];
}

BTreeNode** get_address_of_subtree(BTreeNode* node, int subtree_idx) {
	// We might add additional code to this function when testing your submission.
	return &(node -> subtrees[subtree_idx]);
}

void set_subtree(BTreeNode* node, int subtree_idx, BTreeNode* new_subtree) {
	// We might add additional code to this function when testing your submission.
	node -> subtrees[subtree_idx] = new_subtree;
}

BTreeNode* malloc_btree_node() {
	BTreeNode* new_node = malloc(sizeof *new_node);
	return new_node;
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab cc=110: */
