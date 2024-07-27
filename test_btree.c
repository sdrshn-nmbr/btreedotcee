#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "btree.h"

void shift_tests(BTreeNode* root) {
    // Test shifting functions
    printf("Initial B-tree:\n");
    print_btree(root);

    // Test shift_key_forward
    printf("\nTesting Shift Forward:\n");
    shift_key_forward(root, 0);
    print_btree(root);

    // Test shift_key_backward
    printf("\nTesting Shift Backward:\n");
    shift_key_backward(root, 1);
    print_btree(root);

    // Additional shift tests

    // Test shift_key_forward at the boundary
    printf("\nTesting Shift Forward at the Boundary:\n");
    shift_key_forward(root, root->num_keys - 1);
    print_btree(root);

    // Test shift_key_backward at the boundary
    printf("\nTesting Shift Backward at the Boundary:\n");
    shift_key_backward(root, 0);
    print_btree(root);

    // Test shift_key_forward on an empty B-tree
    printf("\nTesting Shift Forward on an Empty B-tree:\n");
    free_btree(&root);
    root = NULL;
    shift_key_forward(root, 0);
    print_btree(root);

    // Test shift_key_backward on an empty B-tree
    printf("\nTesting Shift Backward on an Empty B-tree:\n");
    free_btree(&root);
    root = NULL;
    shift_key_backward(root, 0);
    print_btree(root);
}

void merge_tests(BTreeNode* root) {
    // Test merging functions
    printf("Initial B-tree:\n");
    print_btree(root);

    // Test merge_subtree_with_next without delete_key
	printf("\nTesting Merge at index 0:\n");
	merge_subtree_with_next(root, 0);
	print_btree(root);
}

int main(int argc, char* argv[]) {
	srand(1);
	BTreeNode* root = NULL;
	insert(&root, 6);
	print_btree(root);
	insert(&root, 8);
	print_btree(root);
	insert(&root, 9);
	print_btree(root);
	insert(&root, 7);
	print_btree(root);
	insert(&root, 5);
	print_btree(root);
	insert(&root, 1);
	print_btree(root);
	insert(&root, 2);
	print_btree(root);

	//
	// Uncomment if you want to see this on a large btree.
	//
	// Make the b-tree huge (>400 keys)
	
	// for(int i = 0; i < 400; i++) {
	// 	int random_num = rand() % 1000;
	// 	bool did_insert = insert(&root, random_num);
	// 	if(!did_insert) {
	// 		printf("Did not insert %d because it is already in the btree.\n", random_num);
	// 	}
	// }
	// print_keys(root, "Root: ", ", ", "\n");
	// print_btree(root);
	
	//

	print_keys(root, "Keys: [", " → ", "]\n");

	KeyQueryResult largest_5 = query_k_largest(root, 5);
	for(int i = 0; i < largest_5.num_keys; i++) {
		printf("largest_5.keys[%d] == %d\n", i, largest_5.keys[i]);
	}
	free(largest_5.keys);

	KeyQueryResult smallest_5 = query_k_smallest(root, 5);
	for(int i = 0; i < smallest_5.num_keys; i++) {
		printf("smallest_5.keys[%d] == %d\n", i, smallest_5.keys[i]);
	}
	free(smallest_5.keys);

	free_btree(&root);

	// MERGE

    BTreeNode* root2 = NULL;

    // Insert keys into the B-tree
    int keys[] = {6, 8, 9, 7, 5, 1, 2, 10, 12, 15, 11, 13, 14, 3, 4};
    for (int i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
        insert(&root2, keys[i]);
    }

    BTreeNode* root3 = get_subtree(root2, 0);
    // Run merge function tests
    print_btree(root2);
    // delete_key(&root2, 6);
    print_btree(root2);
    merge_tests(root3);
    print_btree(root2);

    // SHIFT

    printf("Shifting 4\n");
    shift_key_forward(root3, 0);
    print_btree(root2);

    printf("Now shifting back 4\n");
    shift_key_backward(root3, 1);
    print_btree(root2);

    // DELETE

    printf("Testing deleting key in leaf node\n");
    delete_key(&root2, 6);
    print_btree(root2);

    printf("Testing deleting key in non-leaf node: 13\n");
    delete_key(&root2, 13);
    print_btree(root2);

    printf("Testing deleting key in non-leaf node: 8\n");
    delete_key(&root2, 8);
    print_btree(root2);

    printf("Testing deleting key in non-leaf node: 9\n");
    delete_key(&root2, 9);
    print_btree(root2);

	free_btree(&root2);

    return EXIT_SUCCESS;
}

/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
