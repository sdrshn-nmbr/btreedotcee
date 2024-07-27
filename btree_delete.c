#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "btree.h"

/********************************************************************
|* RULES
|*
|*  1. Do not access 'node -> subtrees' directly.  Use wrappers (accessor functions) instead.
|*   ┌──────────────────────────┬────────────────────────────────────────────┐
|*   │ Instead of this (▼),     │ Use this (▼).                              │
|*   ╞══════════════════════════╪════════════════════════════════════════════╡
|*   │ node -> subtrees[…]      │ get_subtree(node, subtree_idx)             │
|*   ├──────────────────────────┼────────────────────────────────────────────┤
|*   │ node -> subtrees[…] = …; │ set_subtree(node, subtree_idx, new_subtree)│
|*   ├──────────────────────────┼────────────────────────────────────────────┤
|*   │ &(node -> subtrees[…])   │ get_address_of_subtree(node, subtree_idx)  │
|*   └──────────────────────────┴────────────────────────────────────────────┘
|*      ∘ When testing your code, we may modify the wrappers to track which nodes were accessed.
|*      ∘ This applies only to 'node -> subtrees'.  You may access all other fields like usual.
|*
|*  2. Use descriptive names that accurately indicate the variable's type and contents.
|*      ∘ Use 'key_idx' (or 'keyIdx') or 'subtree_idx' (or 'subtreeIdx'), not 'idx', 'i', or 'pos'.
|*      ∘ Use 'copy_of_▒' (or 'copyOf▒') instead of 'temp' if you need to swap two values.
|*      ∘ See code quality standards for other naming conventions.
|*
|*  3. Comment any code that is not self-explanatory.
|*      ∘ See btree_insert.c for examples.
|*
|*  4. Use assertions to document and sanity-check values.
|*      ∘ Ex: Check that indexes are in the expected range.
|*      ∘ Some assertions have already been added to the stub functions below.
|*      ∘ You must add more, as appropriate.  (There is not specific number.)  This is to help you.
|*
|* This is in addition to the requirements in the assignment page.
|*
|* WARNING: There may be penalties for submissions that do not follow these rules.
\*******************************************************************/

static bool _isLeaf(BTreeNode *node)
{
	return get_subtree(node, 0) == NULL;
}

bool _delete_key_helper(BTreeNode **a_root, int key)
{
    BTreeNode *root = *a_root;

    // Search for the key in the current node
    int key_idx = 0;
    while (key_idx < root->num_keys && key > root->keys[key_idx])
    {
        key_idx++;
    }

    // Check if the key is found in the current node
    if (key_idx < root->num_keys && key == root->keys[key_idx])
    {
        // Key found in an internal node
        if (_isLeaf(root))
        {
            // Key is in a leaf node - simply delete it
            for (int i = key_idx; i < root->num_keys - 1; i++)
            {
                root->keys[i] = root->keys[i + 1];
            }
            root->num_keys--;

            return true;
        }
        else // Key is in an internal node
        {
            BTreeNode *left_child = get_subtree(root, key_idx);
            BTreeNode *right_child = get_subtree(root, key_idx + 1);

            // Check if either the left or right child has > MIN_KEYS
            if (left_child->num_keys > MIN_KEYS)
            {
                // Use in-order predecessor
                KeyLocation predecessor = get_in_order_predecessor((KeyLocation){.key = key, .node = root, .key_idx = key_idx});
                root->keys[key_idx] = predecessor.key;
                // Recursively delete the key in the leaf node
                return _delete_key_helper(get_address_of_subtree(root, key_idx), predecessor.key);
            }
            else if (right_child->num_keys > MIN_KEYS)
            {
                // Use in-order successor
                KeyLocation successor = get_in_order_successor((KeyLocation){.key = key, .node = root, .key_idx = key_idx});
                root->keys[key_idx] = successor.key;
                // Recursively delete the key in the leaf node
                return _delete_key_helper(get_address_of_subtree(root, key_idx + 1), successor.key);
            }
            else
            {
                // Both children have the minimum number of keys, merge with the left child
                merge_subtree_with_next(root, key_idx);
                // Recursively delete the key in the merged subtree
                return _delete_key_helper(get_address_of_subtree(root, key_idx), key);
            }
        }
    }
    else
    {
        // Key not found in the current node, proceed to the appropriate subtree
        BTreeNode *subtree = get_subtree(root, key_idx);
        if (_delete_key_helper(&subtree, key))
        {
            // Key was deleted in the subtree, update the parent node
            set_subtree(root, key_idx, subtree);

            // Check if the current node is skinny after deletion
            if (root->num_keys < MIN_KEYS)
            {
                // The current node is skinny - try to fix it

                // Determine the index of the left and right siblings
                int left_sibling_idx = (key_idx > 0) ? key_idx - 1 : -1;
                int right_sibling_idx = (key_idx < root->num_keys) ? key_idx + 1 : -1;

                if (left_sibling_idx >= 0 && get_subtree(root, left_sibling_idx)->num_keys > MIN_KEYS)
                {
                    shift_key_backward(root, key_idx); // Borrow a key from the left sibling
                }
                else if (right_sibling_idx >= 0 && get_subtree(root, right_sibling_idx)->num_keys > MIN_KEYS)
                {
                    shift_key_forward(root, key_idx); // Borrow a key from the right sibling
                }
                else
                {
                    // Both siblings have MIN_KEYS, merge with the left sibling
                    merge_subtree_with_next(root, left_sibling_idx);
                }
            }

            return true; // Key deleted successfully
        }
        else
        {
            return false; //? Key not found in the subtree
        }
    }
}

bool delete_key(BTreeNode **a_root, int key)
{
	if (*a_root == NULL)
	{
		return false; // error handling for misuse of function
	}

	return _delete_key_helper(a_root, key);
}

// Function to merge two subtrees of a parent node
void merge_subtree_with_next(BTreeNode *parent, int subtree_1_idx)
{
	int subtree_2_idx = subtree_1_idx + 1;
	assert(subtree_1_idx >= 0 && subtree_1_idx < parent->num_keys);
	assert(subtree_2_idx >= 0 && subtree_2_idx <= parent->num_keys);

	// Get children to be merged
	BTreeNode *subtree_1 = get_subtree(parent, subtree_1_idx);
	BTreeNode *subtree_2 = get_subtree(parent, subtree_2_idx);
	assert(subtree_1->num_keys + subtree_2->num_keys <= MAX_KEYS);

	// Move key from parent between the two merging subtrees to end of left subtree
	subtree_1->keys[subtree_1->num_keys] = parent->keys[subtree_1_idx];

	// Copy keys and subtrees from subtree_2 to subtree_1
	for (int i = 0; i < subtree_2->num_keys; i++)
	{
		subtree_1->keys[subtree_1->num_keys + 1 + i] = subtree_2->keys[i];
		set_subtree(subtree_1, subtree_1->num_keys + 1 + i, get_subtree(subtree_2, i));
	}

	subtree_1->num_keys += subtree_2->num_keys + 1; // Update number of keys in subtree_1

	// Move keys and subtrees in parent backward
	for (int i = subtree_1_idx + 1; i < parent->num_keys; i++)
	{
		parent->keys[i - 1] = parent->keys[i];
		set_subtree(parent, i, get_subtree(parent, i + 1)); // Also update subtrees
	}

	parent->num_keys--;

	free_btree(&subtree_2);
}

// Function to shift a key forward from a left source tree to a parent and from parent to a right destination subtree
void shift_key_forward(BTreeNode *parent, int src_subtree_idx)
{
	int dst_subtree_idx = src_subtree_idx + 1;
	// assert(src_subtree_idx >= 0 && src_subtree_idx < parent->num_keys);
	// assert(dst_subtree_idx >= 1 && dst_subtree_idx <= parent->num_keys);

	BTreeNode *src_subtree = get_subtree(parent, src_subtree_idx);
	BTreeNode *dst_subtree = get_subtree(parent, dst_subtree_idx);

	// Move last key from src_subtree to parent
	int parent_val = parent->keys[src_subtree_idx];

	parent->keys[src_subtree_idx] = src_subtree->keys[src_subtree->num_keys - 1];
	dst_subtree->num_keys++; // Update number of keys in dst_subtree

	for (size_t i = get_subtree(parent, dst_subtree_idx)->num_keys - 1; i > 0; i--)
	{
		dst_subtree->keys[i] = dst_subtree->keys[i - 1];
	}

	dst_subtree->keys[0] = parent_val; // set first value in
	src_subtree->num_keys--;		   // Update number of keys in src_subtree
}

// Function to shift a key forward from a right source tree to a parent and from parent to a left destination subtree
void shift_key_backward(BTreeNode *parent, int src_subtree_idx)
{
	int dst_subtree_idx = src_subtree_idx - 1;
	// assert(src_subtree_idx >= 1 && src_subtree_idx <= parent->num_keys);
	// assert(dst_subtree_idx >= 0 && dst_subtree_idx < parent->num_keys);

	BTreeNode *src_subtree = get_subtree(parent, src_subtree_idx);
	BTreeNode *dst_subtree = get_subtree(parent, dst_subtree_idx);

	// assert(src_subtree->num_keys > MIN_KEYS);	  // Assert: source has ≥1 extra node
	// assert(dst_subtree->num_keys < MAX_KEYS - 1); // Assert: destination has enough space

	// Move first key from src_subtree to parent
	int parent_val = parent->keys[dst_subtree_idx];

	parent->keys[dst_subtree_idx] = src_subtree->keys[0];

	dst_subtree->num_keys++; // Update number of keys in dst_subtree
	for (size_t i = 0; i < src_subtree->num_keys - 1; i++)
	{
		src_subtree->keys[i] = src_subtree->keys[i + 1];
	}

	dst_subtree->keys[dst_subtree->num_keys - 1] = parent_val; // set first value
	src_subtree->num_keys--;								   // Update number of keys in src_subtree
}

// in-order predecessor of a key is the max key in its left subtree
KeyLocation get_in_order_predecessor(KeyLocation key_location)
{
	assert(key_location.node != NULL); // not a leaf

	BTreeNode *pred_node = key_location.node->subtrees[key_location.key_idx]; // left subtree
	// BTreeNode *pred_node = key_location.node->subtrees[key_location.key_idx]; // Cannot use get_subtree because of nature of KeyLocation struct

	// while (pred_node->subtrees[pred_node->num_keys] != NULL)
	while (get_subtree(pred_node, pred_node->num_keys) != NULL)
	{
		// pred_node = pred_node->subtrees[pred_node->num_keys];
		pred_node = get_subtree(pred_node, pred_node->num_keys); // maximum key in left subtree
	}

	return (KeyLocation){.key = pred_node->keys[pred_node->num_keys - 1], .node = pred_node, .key_idx = pred_node->num_keys - 1};
}

// in-order successor of a key is the min key in its right subtree
KeyLocation get_in_order_successor(KeyLocation key_location)
{
	assert(key_location.node != NULL); // not a leaf

	BTreeNode *succ_node = key_location.node->subtrees[key_location.key_idx + 1]; // right subtree
	// BTreeNode *succ_node = key_location.node->subtrees[key_location.key_idx + 1]; // Cannot use get_subtree because of nature of KeyLocation struct

	// while (succ_node->subtrees[0] != NULL)
	while (get_subtree(succ_node, 0) != NULL)
	{
		// succ_node = succ_node->subtrees[0];
		succ_node = get_subtree(succ_node, 0); // minimum key in right subtree
	}

	return (KeyLocation){.key = succ_node->keys[0], .node = succ_node, .key_idx = 0};
}