// DO NOT MODIFY THIS FILE
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdarg.h>
#include "btree.h"

// Declare private helper functions.
static char* _repeat_char(char ch, size_t num_copies);
static void _print_btree_subtree(BTreeNode* root, char const* prefix, int indent_level);
static char* _malloc_sprintf(char const* format, ...);
void _print_keys(BTreeNode* root, size_t num_keys, size_t* a_num_printed,
					char const* prefix, char const* delimiter, char const* suffix);

void print_btree(BTreeNode* root) {
	if(root == NULL) {
		printf("EMPTY");
	}
	else {
		_print_btree_subtree(root, "", 0);
	}
	printf("\n");
}

size_t print_keys(BTreeNode* root, char const* prefix, char const* delimiter, char const* suffix) {
	size_t num_printed_so_far = 0;
	size_t num_keys = count_keys(root);
	_print_keys(root, num_keys, &num_printed_so_far, prefix, delimiter, suffix);
	return num_keys;
}

static void _print_btree_subtree(BTreeNode* root, char const* prefix, int indent_level) {
	// Recursive helper for print_btree(…).
	bool SHOW_NULL_SUBTREES = false;
	char* indent_str = _repeat_char(' ', indent_level * 2);
	printf("%s", indent_str);
	printf("%s", prefix);
	if(root == NULL) {
		printf("NULL\n");
	}
	else {
		for(int key_idx = 0; key_idx < root -> num_keys; key_idx++) {
			printf("%s%d", (key_idx == 0 ? "{ " : ", "), root -> keys[key_idx]);
		}
		printf(" }\n");
		char* new_prefix = NULL;
		if(get_subtree(root, 0) != NULL || SHOW_NULL_SUBTREES) {
			new_prefix = _malloc_sprintf("< %d: ", root -> keys[0]);
			_print_btree_subtree(get_subtree(root, 0), new_prefix, indent_level + 1);
			free(new_prefix);
			new_prefix = NULL;
		}
		for(int key_idx = 0; key_idx < root -> num_keys; key_idx++) {
			if(get_subtree(root, key_idx + 1) != NULL || SHOW_NULL_SUBTREES) {
				new_prefix = _malloc_sprintf("> %d: ", root -> keys[key_idx]);
				_print_btree_subtree(get_subtree(root, key_idx + 1), new_prefix, indent_level + 1);
				free(new_prefix);
				new_prefix = NULL;
			}
		}
	}
	free(indent_str);
}

static char* _repeat_char(char ch, size_t num_copies) {
	// Create a string on the heap with 'num_copies' copies of 'ch' (+ '\0').
	// Caller is responsible for freeing.
	char* buf = malloc((num_copies + 1) * sizeof *buf);
	for(size_t i = 0; i < num_copies; i++) {
		buf[i] = ch;
	}
	buf[num_copies] = '\0';
	return buf;
}

static char* _malloc_sprintf(char const* format, ...) {
	// This is like sprintf(…) (printf(…) to a string), except that this allocates the right
	// amount of memory.  Caller is responsible for freeing.
	va_list more_args, more_args_copy;
	va_start(more_args, format);
	va_copy(more_args_copy, more_args);
	size_t buf_size = vsnprintf(NULL, 0, format, more_args_copy) + 1;
	assert(buf_size >= 1);
	char* buf = malloc(buf_size * sizeof *buf);
	vsnprintf(buf, buf_size, format, more_args);
	va_end(more_args_copy);
	va_end(more_args);
	return buf;
}


void _print_keys(BTreeNode* root, size_t num_keys, size_t* a_num_printed,
					char const* prefix, char const* delimiter, char const* suffix) {
	// Recursive helper for print_keys(…).
	if(root != NULL) {
		for(int subtree_idx = 0; subtree_idx <= root -> num_keys; subtree_idx++) {
			// Print keys in subtree (recursively)
			BTreeNode* subtree = get_subtree(root, subtree_idx);
			_print_keys(subtree, num_keys, a_num_printed, prefix, delimiter, suffix); // <<< RECURSIVE CALL

			if(subtree_idx < root -> num_keys) {
				// Print a key
				char const* str_before = (*a_num_printed == 0 ? prefix : delimiter);
				printf("%s%d", str_before, root -> keys[subtree_idx]);
				*a_num_printed += 1;

				if(*a_num_printed == num_keys) {
					printf("%s", suffix);
				}
			}
		}
	}
}

/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
