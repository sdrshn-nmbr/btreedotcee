// DO NOT MODIFY THIS FILE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "btree.h"

/////////////////////////////////////////////////////////////////////
// PUBLIC functions
//

void free_btree(BTreeNode** a_root) {
	// Free all BTreeNode objects within the b-tree at *a_root.

	if(*a_root != NULL) {
		// Free children first.
		for(int subtree_idx = 0; subtree_idx <= (*a_root) -> num_keys; subtree_idx++) {
			BTreeNode* subtree = get_subtree(*a_root, subtree_idx); // new: wrapped
			free_btree(&subtree);                                   // new: wrapped
			//free_btree(&(*a_root) -> subtrees[subtree_idx]);      // old: direct access
		}

		// Then, free self (i.e., root).
		free(*a_root);
		*a_root = NULL;
	}
}


bool does_contain_key(BTreeNode* root, int key_to_find) {
	// Return true if the b-tree contains the key 'key_to_find'.

	// An empty tree (or subtree) cannot contain any key.
	if(root == NULL) {
		return false;
	}

	// Search keys in this node.
	for(int key_idx = 0; key_idx < root -> num_keys; key_idx++) {
		if(key_to_find == root -> keys[key_idx]) {
			return true;
		}
	}

	// Search subtrees (recursively)
	BTreeNode* subtree = choose_subtree_for_key(root, key_to_find); // <<< RECURSIVE CALL
	return does_contain_key(subtree, key_to_find);
}


size_t count_keys(BTreeNode* root) {
	// Return the number of keys in this b-tree.

	// Number of keys in a tree is the number of keys in the root plus the total number
	// of keys in each of its subtrees (recursively).
	size_t num_nodes = 0;
	if(root != NULL) {
		num_nodes += root -> num_keys;
		for(int subtree_idx = 0; subtree_idx <= root -> num_keys; subtree_idx++) {
			num_nodes += count_keys(get_subtree(root, subtree_idx));  // <<< RECURSIVE CALL
		}
	}
	return num_nodes;
}


/////////////////////////////////////////////////////////////////////
// SUPPORT functions
//

BTreeNode* choose_subtree_for_key(BTreeNode* node, int new_key) {
	// 'new_key' should be inserted into the subtree just left of the first key in nodes that is
	// greater than 'new_key', or else the last subtree if none of the keys is greater.

	int num_keys_less_than_new_key = 0;
	while((num_keys_less_than_new_key < node -> num_keys)
			&& (node -> keys[num_keys_less_than_new_key] < new_key)) {
		num_keys_less_than_new_key += 1;
	}
	assert(num_keys_less_than_new_key >= 0 && num_keys_less_than_new_key <= node -> num_keys);

	//return node -> subtrees[num_keys_less_than_new_key];
	return get_subtree(node, num_keys_less_than_new_key);
}


/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab cc=110: */
