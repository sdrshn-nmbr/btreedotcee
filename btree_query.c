#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "btree.h"

// Instructor solution add/removes about 25 lines in this file.

int query_min(BTreeNode *root)
{
	// Return the smallest (minimum value) key in the b-tree.
	//
	// Rules:  (1) No recursion.  (2) no malloc(…)  (3) Θ(h)

	// while (root->subtrees[0])
	while (get_subtree(root, 0) != NULL)
	{
		root = get_subtree(root, 0);
	}

	return root->keys[0]; // min val
}

int query_max(BTreeNode *root)
{
	// Return the largest (maximum value) key in the b-tree.
	//
	// Rules:  (1) No recursion.  (2) no malloc(…)  (3) Θ(h)

	// while (root->subtrees[root->num_keys])
	while (get_subtree(root, root->num_keys) != NULL)
	{
		// root = root->subtrees[root->num_keys];
		root = get_subtree(root, root->num_keys);
	}

	return root->keys[root->num_keys - 1]; // max val
}

void reverse_iot(BTreeNode *node, int *k_largest_keys, int *num_keys_found, size_t k)
{
	if (node == NULL || *num_keys_found >= k)
	{
		return;
	}

	// Traverse right subtrees
	for (int i = node->num_keys; i > 0; i--)
	{
		// reverse_iot(node->subtrees[i], k_largest_keys, num_keys_found, k);
		reverse_iot(get_subtree(node, i), k_largest_keys, num_keys_found, k);
		
		// Process current node
		if (*num_keys_found < k)
		{
			k_largest_keys[(*num_keys_found)++] = node->keys[i - 1];
		}
	}

	// Traverse left subtree
	// reverse_iot(node->subtrees[0], k_largest_keys, num_keys_found, k);
	reverse_iot(get_subtree(node, 0), k_largest_keys, num_keys_found, k);
}

KeyQueryResult query_k_largest(BTreeNode *root, size_t k)
{
	// Return an array of the (up to) k largest keys in the b-tree.

	// Rules:  (1) O(h + k)

	int *k_largest_keys = malloc(k * (sizeof *k_largest_keys));
	int num_keys_found = 0;

	reverse_iot(root, k_largest_keys, &num_keys_found, k);

	return (KeyQueryResult){.keys = k_largest_keys, .num_keys = num_keys_found};
}

void iot(BTreeNode *node, int *k_smallest_keys, int *num_keys_found, size_t k)
{
	if (node == NULL || *num_keys_found >= k)
	{
		return;
	}

	// Traverse left subtrees
	for (int i = 0; i < node->num_keys; i++)
	{
		// iot(node->subtrees[i], k_smallest_keys, num_keys_found, k);
		iot(get_subtree(node, i), k_smallest_keys, num_keys_found, k);

		// Process current node
		if (*num_keys_found < k)
		{
			k_smallest_keys[(*num_keys_found)++] = node->keys[i];
		}
	}

	// Traverse right subtree
	// iot(node->subtrees[node->num_keys], k_smallest_keys, num_keys_found, k);
	iot(get_subtree(node, node->num_keys), k_smallest_keys, num_keys_found, k);
}

KeyQueryResult query_k_smallest(BTreeNode *root, size_t k)
{
	// Return an array of the (up to) k smallest keys in the b-tree.

	// Rules:  (1) O(h + k)

	int *k_smallest_keys = malloc(k * sizeof *k_smallest_keys);
	int num_keys_found = 0;

	iot(root, k_smallest_keys, &num_keys_found, k);

	return (KeyQueryResult){.keys = k_smallest_keys, .num_keys = num_keys_found};
}

/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */