// DO NOT MODIFY THIS FILE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "btree.h"

/////////////////////////////////////////////////////////////////////
// DECLARATIONS of HELPER (private) type and functions -- used only in this file
//

// Private helper struct type -- used by _split(…) and _add_key(…) to propagate the value
// to be inserted at the next level up (i.e., when _add_key(…) returns).
typedef struct {
	bool should_insert;
	// .should_insert indicates if we have a key to insert in parent (or become root of empty btree).
	// If .should_insert is false, then .key and .right_child will be ignored.

	int key;
	//.key will be passed up to the parent for insertion (or to populate new root of empty tree).

	BTreeNode* right_child;
    // .right_child is a subtree that belongs just to the right of .key.  The keys in .right_child
	// are all greater than .key. 
} _InsertionToDo;


// Private helper functions - forward declarations
static void _insert_key_and_subtree(BTreeNode* node, int new_key, BTreeNode* right_child);
static _InsertionToDo _split(BTreeNode* node, int new_key, BTreeNode* right_child);
static _InsertionToDo _add_key(int new_key, BTreeNode* node);


/////////////////////////////////////////////////////////////////////
// PUBLIC interface
//

bool insert(BTreeNode** a_root, int new_key) {
	// Insert 'new_key' into the b-tree at *a_root, but only if it does not alerady exist.
	// Return true if it was inserted (i.e., if it did not already exist), or false if it already existed.
	//
	// This function is NOT recursive.  It is only called from main(…) or other external code.

	if(does_contain_key(*a_root, new_key)) {
		return false;
	}
	else {
		_InsertionToDo insertion_from_below = _add_key(new_key, *a_root);
		if(insertion_from_below.should_insert) {
			// Create or replace new root
			//BTreeNode* new_root = malloc_btree_node();  // old: direct access
			BTreeNode* new_root = malloc_btree_node();    // new: wrapped
			*new_root = (BTreeNode) { .num_keys = 1,
									  .keys = { [0] = insertion_from_below.key },
									  .subtrees = { [0] = *a_root,
													[1] = insertion_from_below.right_child } };
			*a_root = new_root;
		}
		return true;
	}
}


/////////////////////////////////////////////////////////////////////
// HELPER (private) functions - only used in this file
//

static void _insert_key_and_subtree(BTreeNode* node, int new_key, BTreeNode* new_right_child) {
	assert(node -> num_keys < MAX_KEYS);

	// When we insert a node as a child of an existing node, we insert a key and a subtree.
	
	// Find the appropriate position in this node, and shift keys/subtrees to make room.
	node -> num_keys += 1;
	int key_idx = node -> num_keys - 1;
	for( ; key_idx > 0 && node -> keys[key_idx - 1] > new_key; key_idx--) { // (compare)
		int right_subtree_idx = key_idx + 1;
		assert(key_idx < MAX_KEYS);
		assert(right_subtree_idx < MAX_SUBTREES);
		node -> keys[key_idx] = node -> keys[key_idx - 1];
		set_subtree(node, right_subtree_idx, get_subtree(node, right_subtree_idx - 1));  // new: wrapped
		//node -> subtrees[right_subtree_idx] = node -> subtrees[right_subtree_idx - 1]; // old: direct access
	}

	assert(key_idx >= 0 && key_idx < node -> num_keys); // Sanity check: key_idx in correct range.
	assert(node -> num_keys <= MAX_KEYS);
	
	// Add the new key and the right child that goes with it in the slot we just opened up.
	node -> keys[key_idx] = new_key;
	set_subtree(node, key_idx + 1, new_right_child);   // new: wrapped
	//node -> subtrees[key_idx + 1] = new_right_child; // old: direct access
}


static _InsertionToDo _split(BTreeNode* node_to_split, int new_key, BTreeNode* right_child) {
	assert(node_to_split -> num_keys == MAX_KEYS);

	// Decide where to split and where the new node will go.
	int num_keys_in_left  = MIN_KEYS;
	bool should_insert_in_right = (new_key > node_to_split -> keys[num_keys_in_left]); // (compare)
	if(should_insert_in_right) {
		num_keys_in_left += 1; // because we will be detaching one key to become the parent_key
	}
	int num_keys_in_right = node_to_split -> num_keys - num_keys_in_left;

	// Sanity checks:
	assert(num_keys_in_left + num_keys_in_right == node_to_split -> num_keys); // Same # of keys before/after
	assert(num_keys_in_left  >= MIN_KEYS && num_keys_in_left  <= MAX_KEYS);
	assert(num_keys_in_right >= MIN_KEYS && num_keys_in_right <= MAX_KEYS);

	// 'new_left' is the node_to_split.  We will simply move some keys/subtrees out of it.
	BTreeNode* new_left  = node_to_split;  // ... just for clarity in the code below
	new_left -> num_keys = num_keys_in_left;

	// 'new_right' is a newly created node_to_split.
	//BTreeNode* new_right = malloc(sizeof *new_right); // old: direct access
	BTreeNode* new_right = malloc_btree_node();         // new: wrapped
	*new_right = (BTreeNode) { .num_keys = num_keys_in_right, .subtrees = { 0 }, .keys = { 0 } };

	// Copy keys and subtrees from new_left into new_right.
	for(int key_idx_in_right = 0; key_idx_in_right <= num_keys_in_right; key_idx_in_right++) {
		int key_idx_in_left = key_idx_in_right + num_keys_in_left;
		assert(key_idx_in_left >= num_keys_in_left);
		assert(key_idx_in_left <= num_keys_in_left + num_keys_in_right);

		if(key_idx_in_right < num_keys_in_right) {
			assert(key_idx_in_left < num_keys_in_left + num_keys_in_right);
			new_right -> keys[key_idx_in_right] = new_left -> keys[key_idx_in_left];
		}
		//new_right -> subtrees[key_idx_in_right] = new_left -> subtrees[key_idx_in_left];
		set_subtree(new_right, key_idx_in_right, get_subtree(new_left, key_idx_in_left));
	}

	// Insert in 'new_left' or 'new_right', depending on where 'new_key' belongs (determined above).
	BTreeNode* node_to_insert_into = should_insert_in_right ? new_right : new_left;
	_insert_key_and_subtree(node_to_insert_into, new_key, right_child);

	// Detach the last key from 'new_left'.  This will be the parent of new_left and new_right.
	int new_parent_key = new_left -> keys[new_left -> num_keys - 1];
	new_left -> num_keys -= 1;

	// Move subtree just to the right of the key just detached to beginning of 'new_right'.
	//new_right -> subtrees[0] = new_left -> subtrees[new_left -> num_keys + 1]; // old: direct access
	set_subtree(new_right, 0, get_subtree(new_left, new_left -> num_keys + 1));  // new: wrapped
	// ... because keys in that subtree are greater than 'new_parent_key', but less than all keys in
	// 'new_right'.  When traversing, they should come between new_parent_key and keys in new_right.

	assert(new_left  -> num_keys == MIN_KEYS);
	assert(new_right -> num_keys == MIN_KEYS || new_right -> num_keys == MIN_KEYS + 1);

	return (_InsertionToDo) { .should_insert = true, .key = new_parent_key, .right_child = new_right };
}


static _InsertionToDo _add_key(int new_key, BTreeNode* node) {
	if(node == NULL) {  // Base case: Tree is empty, or we are at an empty subtree of a leaf.
		return (_InsertionToDo) { .should_insert = true, .key = new_key, .right_child = NULL };
	}
	else {
		// Insert into child recursively, until we get to a leaf.
		BTreeNode* subtree = choose_subtree_for_key(node, new_key);
		_InsertionToDo insertion_from_below = _add_key(new_key, subtree); // <<< RECURSIVE CALL

		// Actual insertion takes place as we return, on the way back up from leaf nodes to root.
		if(insertion_from_below.should_insert) {
			if(node -> num_keys == MAX_KEYS) { // Node is full, so split it.  
				return _split(node, insertion_from_below.key, insertion_from_below.right_child);
			}
			else { // Just insert in this node and be done with it.
				_insert_key_and_subtree(node, insertion_from_below.key, insertion_from_below.right_child);
			}
		}
		return (_InsertionToDo) { .should_insert = false };
	}
}

/* Credit: This was loosely adapted from https://www.programiz.com/dsa/b-tree by Alexander J Quinn,
 *         though this code bears practically no resemblance to the code at that site.
 */
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab cc=110: */
