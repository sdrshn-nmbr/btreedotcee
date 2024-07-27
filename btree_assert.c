// DO NOT MODIFY THIS FILE

// You can add your own assert_▒▒▒(…) functions in btree_query.c or btree_delete.c though you
// probably will not need to.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "btree.h"

void assert_node_keys_lt(BTreeNode* node, int key_value) {
	if(node != NULL) {
		for(int key_idx = 0; key_idx < node -> num_keys; key_idx++) {
			assert(node -> keys[key_idx] < key_value);
		}
	}
}

void assert_node_keys_gt(BTreeNode* node, int key_value) {
	if(node != NULL) {
		for(int key_idx = 0; key_idx < node -> num_keys; key_idx++) {
			assert(node -> keys[key_idx] > key_value);
		}
	}
}

void assert_node_keys_in_ascending_order(BTreeNode* node) {
	if(node != NULL) {
		for(int key_idx = 1; key_idx < node -> num_keys; key_idx++) {
			assert(node -> keys[key_idx - 1] < node -> keys[key_idx]);
		}
	}
}

void assert_node_correct(BTreeNode* node) {
	if(node != NULL) {
		// Check that number of keys in each subtree is between MIN_KEYS and MAX_KEYS.
		if(get_subtree(node, 0) != NULL) {
			for(int subtree_idx = 0; subtree_idx <= node -> num_keys; subtree_idx++) {
				BTreeNode* subtree = get_subtree(node, subtree_idx);
				assert(subtree -> num_keys >= MIN_KEYS);
				assert(subtree -> num_keys <= MAX_KEYS);
			}
		}

		// Note: We do this check on the subtrees (instead of on 'node') because root can
		// have as few as 1 key, while all other nodes must have at least MIN_KEYS subtrees.
		// However, the maximum number of keys applies to all nodes.
		assert(node -> num_keys <= MAX_KEYS);

		// Check that keys in this node are in ascending order.
		assert_node_keys_in_ascending_order(node);

		// Check range of keys in subtrees
		for(int key_idx = 0; key_idx < node -> num_keys; key_idx++) {
			assert_node_keys_lt(get_subtree(node, key_idx),     node -> keys[key_idx]);
			assert_node_keys_gt(get_subtree(node, key_idx + 1), node -> keys[key_idx]);
		}

		// Do all of the above checks on each subtree of this node.
		for(int key_idx = 0; key_idx <= node -> num_keys; key_idx++) {
			assert_node_correct(get_subtree(node, key_idx));
		}
	}
}
