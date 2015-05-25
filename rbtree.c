#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


//
// Node of red-black tree
//
typedef uint32_t data_t;
typedef enum { red, black } color_t;
typedef struct node {
  data_t data;
  color_t color;
  struct node *left, *right, *parent;
} node_t;

static void insert(node_t **root, data_t data);
static void traverse_inorder(node_t *root, void (*)(data_t data));


//
// Test if rbtree works fine
//
void test(data_t val) {
  static uint32_t prev = 0;

  if (prev > val) {
    fputs("Test failed!", stderr);
    exit(1);
  }

  prev = val;
  printf(" %d", val);
}

int main() {
  char names[][128] = { "fixtures/test00", "fixtures/test01", "fixtures/test02" };

  size_t i;
  for (i = 0; i < sizeof names/sizeof names[0]; ++i) {
    FILE *input = fopen(names[i], "r");
    node_t *root = NULL;

    printf("Inserted:");
    uint32_t val;
    while(fscanf(input, "%u", &val) != EOF) {
      printf(" %d", val);
      fflush(stdout);
      insert(&root, val);
    }
    putchar('\n');

    printf("Stored:  ");
    traverse_inorder(root, test);
    putchar('\n');
    putchar('\n');
  }
  return 0;
}



//
// Insertion
//
void insert(node_t **root, data_t data) {
  // Allocate memory for a new node
  node_t *z = malloc(sizeof(node_t));
  z->data = data;
  z->left = z->right = z->parent = NULL;

  // If root is NULL, set z as root and return
  if (*root == NULL) {
    z->color = black;
    (*root) = z;
    return;
  }


  //
  // Standard BST insertion
  //
  node_t *y, *x = (*root);
  while (x != NULL) {
    y = x;
    x = (z->data < x->data) ? x->left : x->right;
  }
  z->color = red;
  z->parent = y;
  if (z->data > y->data) {
    y->right = z;
  } else {
    y->left = z;
  }


  //
  // Fixup the Red-Black tree
  //

  // Iterate until z is not a root, and z's parent color is red
  while (z != *root && z->parent->color == red) {
    // Find uncle
    node_t *uncle = z->parent == z->parent->parent->left ?
      z->parent->parent->right :
      z->parent->parent->left;

    // Check if the uncle is RED
    if (uncle->color == red) {
      z->parent->color = uncle->color = black;
      z->parent->parent->color = red;
      z = z->parent->parent;
      continue;
    }

    // Otherwise, there are four cases; LL, LR, RL and RR
    void rotate_left(node_t*);
    void rotate_right(node_t*);

    if (z->parent == z->parent->parent->left && z == z->parent->left) {
      // On Left-Left (LL) case
      //
      // 1. Swap color of parent and grandparent
      // 2. Right Rotate Grandparent
      color_t c = z->parent->color ;
      z->parent->color = z->parent->parent->color;
      z->parent->parent->color = c;
      rotate_right(z->parent->parent);
    }

    if (z->parent == z->parent->parent->left && z == z->parent->right) {
      // On Left-Right (LR) case
      //
      // 1. Swap color of current node  and grandparent
      // 2. Left Rotate Parent
      // 3. Right Rotate Grand Parent
      color_t c = z->color;
      z->color = z->parent->parent->color;
      z->parent->parent->color = c;
      rotate_left(z->parent);
      rotate_right(z->parent->parent);
    }

    if (z->parent == z->parent->parent->right && z == z->parent->right) {
      // On Right-Right (RR) case
      //
      // 1. Swap color of parent and grandparent
      // 2. Left Rotate Grandparent
      color_t c = z->parent->color;
      z->parent->color = z->parent->parent->color;
      z->parent->parent->color = c;
      rotate_left(z->parent->parent);
    }

    if (z->parent == z->parent->parent->right && z == z->parent->left) {
      // On Right-Left (RL) case
      //
      // 1. Swap color of current node  and grandparent
      // 2. Right Rotate Parent
      // 3. Left Rotate Grand Parent
      color_t c = z->color;
      z->color = z->parent->parent->color;
      z->parent->parent->color = c;
      rotate_right(z->parent);
      rotate_left(z->parent->parent);
    }
  }

  // Root should always be black
  (*root)->color = black;
}


//
// Helper functions
//
void rotate_left(node_t *node) {
  node_t *child = node->right;
  node->right = child->left;

  if (node->right != NULL) {
    node->right->parent = node;
  }
  child->parent = node->parent;

  if (node == node->parent->left) {
    node->parent->left = child;
  } else {
    node->parent->right = child;
  }
  child->left = node;
  node->parent = child;
}


void rotate_right(node_t *node) {
  node_t *child = node->left;
  node->left = child->right;

  if (node->left != NULL) {
    node->left->parent = node;
  }
  child->parent = node->parent;

  if (node == node->parent->left) {
    node->parent->left = child;
  } else {
    node->parent->right = child;
  }
  child->right = node;
  node->parent = child;
}


//
// Traverse arbitrary binary tree in inorder fashion
//
void traverse_inorder(node_t *node, void (*func)(data_t data)) {
  if (!node) { return; }

  traverse_inorder(node->left, func);
  func(node->data);
  traverse_inorder(node->right, func);
}
