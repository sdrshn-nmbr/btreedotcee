static bool _iot_for_delete(BTreeNode **a_root, BTreeNode *node, int key) {
    if (node == NULL) {
        return false;
    }

    bool keyDeleted = false;
    int key_idx;

    // Find the key in the node
    for (key_idx = 0; key_idx < node->num_keys && key > node->keys[key_idx]; key_idx++);

    // If the key is found in the node
    if (key_idx < node->num_keys && key == node->keys[key_idx]) {
		printf("Key found in node\n");
        keyDeleted = true;

        // Case 1: Key in Leaf Node
        if (_isLeaf(node) && node->num_keys > MIN_KEYS) {
			printf("Case 1: Key in Leaf Node\n");
            // Shift keys backward
			for (int i = key_idx; i < node->num_keys - 1; i++) {
				node->keys[i] = node->keys[i + 1];
			}
			node->num_keys--;
        }
        // Case 2: Key in Internal Node with Immediate Predecessor or Successor
        else {
            // Find immediate predecessor or successor
            KeyLocation pred_location = get_in_order_predecessor((KeyLocation){.node = node, .key_idx = key_idx});
            KeyLocation succ_location = get_in_order_successor((KeyLocation){.node = node, .key_idx = key_idx});

            // Choose the immediate predecessor or successor based on availability
            KeyLocation replacement_location = (pred_location.node != NULL) ? pred_location : succ_location;

            // Replace the key with the chosen replacement
            node->keys[key_idx] = replacement_location.key;

            // Recursively delete the replacement key from its subtree
            _iot_for_delete(a_root, get_subtree(node, key_idx), replacement_location.key);
        }
    }
    // Case 3: Key not found in the current node
    else {
        // Recursively search in the appropriate subtree
        key_idx = (key_idx > 0) ? key_idx - 1 : key_idx; // Adjust key_idx if needed for the subtree
        keyDeleted = _iot_for_delete(a_root, get_subtree(node, key_idx), key);
    }

    // Check for underflow
    if (node->num_keys < MIN_KEYS) {
        // Try borrowing from immediate siblings
        bool borrowed = false;

        // Try borrowing from the right sibling
        if (key_idx < node->num_keys - 1) {
            BTreeNode *right_sibling = get_subtree(node, key_idx + 1);

            if (right_sibling->num_keys > MIN_KEYS) {
                // Borrow the key from the right sibling
                node->keys[node->num_keys] = right_sibling->keys[0];
                shift_key_forward(right_sibling, 0);
                set_subtree(node, node->num_keys + 1, get_subtree(right_sibling, 0));
                node->num_keys++;
                borrowed = true;
            }
        }

        // Try borrowing from the left sibling if necessary
        if (!borrowed && key_idx > 0) {
            BTreeNode *left_sibling = get_subtree(node, key_idx - 1);

            if (left_sibling->num_keys > MIN_KEYS) {
                // Borrow the key from the left sibling
                shift_key_forward(node, key_idx - 1);
                node->keys[key_idx - 1] = left_sibling->keys[left_sibling->num_keys - 1];
                set_subtree(node, key_idx, get_subtree(left_sibling, left_sibling->num_keys));
                left_sibling->num_keys--;
                node->num_keys++;
                borrowed = true;
            }
        }

        // If borrowing is not possible, merge with siblings or handle root underflow
        if (!borrowed) {
            // Try merging with the right sibling
            if (key_idx < node->num_keys - 1) {
                BTreeNode *right_sibling = get_subtree(node, key_idx + 1);
                if (node->num_keys + right_sibling->num_keys + 1 <= MAX_KEYS) {
                    // Merge with the right sibling
                    merge_subtree_with_next(node, key_idx);
                } else {
                    // Merge is not possible, delete the right sibling node
                    _delete_node(right_sibling);
                }
            }
            // Try merging with the left sibling
            else if (key_idx > 0) {
                BTreeNode *left_sibling = get_subtree(node, key_idx - 1);
                if (left_sibling->num_keys + node->num_keys + 1 <= MAX_KEYS) {
                    // Merge with the left sibling
                    merge_subtree_with_next(node, key_idx - 1);
                } else {
                    // Merge is not possible, delete the left sibling node
                    _delete_node(left_sibling);
                }
            }
            // Handle root underflow
            else if (key_idx == 0 && node->num_keys == 1) {
                // Update the root when it becomes empty after merging
                *a_root = get_subtree(node, 0);
                _delete_node(node);
            }
        }
    }

    return keyDeleted;
}

static bool _find_and_merge(BTreeNode **a_root, int key, int key_subtree_idx)
{
	// An empty tree (or subtree) cannot contain any key.
	if (*a_root == NULL)
	{
		return false;
	}

	// Search keys in this node.
	for (int key_idx = 0; key_idx < (*a_root)->num_keys; key_idx++)
	{
		if (key == (*a_root)->keys[key_idx])
		{
			return true;
		}
		else if ((*a_root)->num_keys == MIN_KEYS && key_idx < (*a_root)->num_keys - 1 && _get_parent(*a_root, key) != NULL)
		{
			// Merge with the right sibling
			BTreeNode *right_sibling = get_subtree(*a_root, key_idx + 1);
			if ((*a_root)->num_keys + right_sibling->num_keys + 1 <= MAX_KEYS)
			{
				// Merge with the right sibling
				merge_subtree_with_next(*a_root, key_subtree_idx);
			}
		}
		else if ((*a_root)->num_keys == MIN_KEYS && key_idx == (*a_root)->num_keys - 1 && _get_parent(*a_root, key) != NULL)
		{
			// Merge with the left sibling
			BTreeNode *left_sibling = get_subtree(*a_root, key_idx - 1);
			if (left_sibling->num_keys + (*a_root)->num_keys + 1 <= MAX_KEYS)
			{
				// Merge with the left sibling
				merge_subtree_with_next(*a_root, key_subtree_idx - 1);
			}
		}
		
	}

	// Search subtrees (recursively)
	BTreeNode *subtree = choose_subtree_for_key(*a_root, key); // <<< RECURSIVE CALL
	return does_contain_key(subtree, key);
}

// Helper function to delete a node and free its memory
static void _delete_node(BTreeNode *node) {
    if (node != NULL) {
        free(node);
    }
}

static bool _delete_key_with_merge(BTreeNode **a_root, int key) {
    if (*a_root == NULL) {
        return false; // Key not found
    }

    bool keyDeleted = false;

    // Search keys in this node.
    for (int key_idx = 0; key_idx < (*a_root)->num_keys; key_idx++) {
        if (key == (*a_root)->keys[key_idx]) {
            keyDeleted = true;

            // Perform deletion based on the case (leaf or internal node)
            if (_isLeaf(*a_root)) {
                // Case 1: Key in Leaf Node
                for (int j = key_idx; j < (*a_root)->num_keys - 1; j++) {
                    shift_key_backward(*a_root, j + 1);
                }

                (*a_root)->num_keys--;
            } else {
                // Case 2: Key in Internal Node
                // Find immediate predecessor or successor
                KeyLocation pred_location = get_in_order_predecessor((KeyLocation){.node = *a_root, .key_idx = key_idx});
                KeyLocation succ_location = get_in_order_successor((KeyLocation){.node = *a_root, .key_idx = key_idx});
                // Choose the immediate predecessor or successor based on availability
                KeyLocation replacement_location = (pred_location.node != NULL) ? pred_location : succ_location;
                // Replace the key with the chosen replacement
                (*a_root)->keys[key_idx] = replacement_location.key;

                // Recursively delete the replacement key from its subtree
                _delete_key_with_merge(get_address_of_subtree(*a_root, key_idx + 1), replacement_location.key);
            }

            break; // Key found and processed, exit loop
        }
    }

    // Check for underflow and merge subtrees if needed
    if ((*a_root)->num_keys < MIN_KEYS) {
        // Try merging with immediate siblings
        int subtree_idx = 0; // Assume it's the leftmost subtree

        // Try merging with the right sibling
        if (subtree_idx < (*a_root)->num_keys - 1) {
            BTreeNode *right_sibling = get_subtree(*a_root, subtree_idx + 1);

            if (right_sibling->num_keys > MIN_KEYS) {
                // Borrow the key from the right sibling
                (*a_root)->keys[(*a_root)->num_keys] = right_sibling->keys[0];
                shift_key_forward(right_sibling, 0);
                set_subtree(*a_root, (*a_root)->num_keys + 1, get_subtree(right_sibling, 0));
                (*a_root)->num_keys++;
                keyDeleted = true;
            }
        }

        // Try merging with the left sibling if necessary
        if (!keyDeleted && subtree_idx > 0) {
            BTreeNode *left_sibling = get_subtree(*a_root, subtree_idx - 1);

            if (left_sibling->num_keys > MIN_KEYS) {
                // Borrow the key from the left sibling
                shift_key_forward(*a_root, subtree_idx - 1);
                (*a_root)->keys[subtree_idx - 1] = left_sibling->keys[left_sibling->num_keys - 1];
                set_subtree(*a_root, subtree_idx, get_subtree(left_sibling, left_sibling->num_keys));
                left_sibling->num_keys--;
                (*a_root)->num_keys++;
                keyDeleted = true;
            }
        }

        // If merging is not possible, consider other cases for handling underflow
        if (!keyDeleted) {
            // Try merging with the right sibling
            if (subtree_idx < (*a_root)->num_keys - 1) {
                BTreeNode *right_sibling = get_subtree(*a_root, subtree_idx + 1);
                if ((*a_root)->num_keys + right_sibling->num_keys + 1 <= MAX_KEYS) {
                    // Merge with the right sibling
                    merge_subtree_with_next(*a_root, subtree_idx);
                }
            }
            // Try merging with the left sibling
            else if (subtree_idx > 0) {
                BTreeNode *left_sibling = get_subtree(*a_root, subtree_idx - 1);
                if (left_sibling->num_keys + (*a_root)->num_keys + 1 <= MAX_KEYS) {
                    // Merge with the left sibling
                    merge_subtree_with_next(*a_root, subtree_idx - 1);
                }
            }
            // Handle root underflow
            else if (subtree_idx == 0 && (*a_root)->num_keys == 1) {
                // Update the root when it becomes empty after merging
                *a_root = get_subtree(*a_root, 0);
                _delete_node(*a_root); // Free memory of merged node
            }
        }
    }

    return keyDeleted;
}

static BTreeNode *_get_parent(BTreeNode *root, int key_to_find)
{
	// An empty tree (or subtree) cannot contain any key.
	if (root == NULL)
	{
		return NULL;
	}

	for (int subtree_idx = 0; subtree_idx <= root->num_keys; subtree_idx++)
	{
		BTreeNode *subtree = get_subtree(root, subtree_idx);
		for (int key_idx = 0; key_idx < subtree->num_keys; key_idx++)
		{
			if (key_to_find == subtree->keys[key_idx])
			{
				return root;
			}
		}
	}

	// Search subtrees (recursively)
	BTreeNode *subtree = choose_subtree_for_key(root, key_to_find); // <<< RECURSIVE CALL
	return _get_parent(subtree, key_to_find);
}