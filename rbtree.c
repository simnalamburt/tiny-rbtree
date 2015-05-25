#include <stdio.h>
#include <stdlib.h>


//
// Node of red-black tree
//
typedef enum { red, black } color;
typedef struct node {
  int data;
  color color;
  struct node *left, *right, *parent;
} node;

static void insert(node **root, int data);
static void traverse_inorder(node *root, void (*)(int data));


//
// Test if rbtree works fine
//
void per_data(int data) { printf(" %d", data); }

int main() {
  node *root = NULL;
  insert(&root, 5);
  insert(&root, 3);
  insert(&root, 7);
  insert(&root, 2);
  insert(&root, 4);
  insert(&root, 6);
  insert(&root, 8);
  insert(&root, 11);

  // Expected output: 2 3 4 5 6 7 8 11
  printf("inorder Traversal Is :");
  traverse_inorder(root, per_data);
  puts("");

  return 0;
}



static node *grandparent(node *n);
static node *uncle(node *n);
static void rotate_left(node**, node*);
static void rotate_right(node**, node*);


//
// Insertion
//
void insert(node **root, int data) {
  // Allocate memory for a new node
  node *z = malloc(sizeof(node));
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
  node *y, *x = (*root);
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
    // Find uncle and store uncle in y
    node *y = z->parent == z->parent->parent->left ?
      z->parent->parent->right :
      z->parent->parent->left;

    if (y->color == red) {
      // If uncle is RED
      //
      // 1. Change color of parent and uncle as BLACK
      // 2. Change color of grandparent as RED
      // 3. Move z to grandparent
      y->color = black;
      z->parent->color = black;
      z->parent->parent->color = red;
      z = z->parent->parent;
    } else {
      // Otherwise, there are four cases; LL, LR, RL and RR

      if (z->parent == z->parent->parent->left && z == z->parent->left) {
        // On Left-Left (LL) case
        //
        // 1. Swap color of parent and grandparent
        // 2. Right Rotate Grandparent
        color c = z->parent->color ;
        z->parent->color = z->parent->parent->color;
        z->parent->parent->color = c;
        rotate_right(root, z->parent->parent);
      }

      if (z->parent == z->parent->parent->left && z == z->parent->right) {
        // On Left-Right (LR) case
        //
        // 1. Swap color of current node  and grandparent
        // 2. Left Rotate Parent
        // 3. Right Rotate Grand Parent
        color c = z->color;
        z->color = z->parent->parent->color;
        z->parent->parent->color = c;
        rotate_left(root, z->parent);
        rotate_right(root, z->parent->parent);
      }

      if (z->parent == z->parent->parent->right && z == z->parent->right) {
        // On Right-Right (RR) case
        //
        // 1. Swap color of parent and grandparent
        // 2. Left Rotate Grandparent
        color c = z->parent->color;
        z->parent->color = z->parent->parent->color;
        z->parent->parent->color = c;
        rotate_left(root, z->parent->parent);
      }

      if (z->parent == z->parent->parent->right && z == z->parent->left) {
        // On Right-Left (RL) case
        //
        // 1. Swap color of current node  and grandparent
        // 2. Right Rotate Parent
        // 3. Left Rotate Grand Parent
        color c = z->color;
        z->color = z->parent->parent->color;
        z->parent->parent->color = c;
        rotate_right(root, z->parent);
        rotate_left(root, z->parent->parent);
      }
    }
  }

  // Root should always be black
  (*root)->color = black;
}


//
// Helper functions
//
node *grandparent(node *n) {
  if (n == NULL || n->parent == NULL) { return NULL; }

  return n->parent->parent;
}

node *uncle(node *n) {
  node *g = grandparent(n);
  if (g == NULL) { return NULL; }

  return n->parent == g->left ? g->right : g->left;
}


void rotate_left(node **root, node *x) {
  // Variable 'y' stores the address x's right child
  node *y = x->right;

  // Store y's left subtree's pointer as x's right child
  x->right = y->left;

  // Update parent pointer of x's right
  if (x->right != NULL) {
    x->right->parent = x;
  }

  // Update y's parent pointer
  y->parent = x->parent;

  // If x's parent is null, make y as root of tree
  // Otherwise, store y at the place of x
  if (x->parent == NULL) {
    *root = y;
  } else if (x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }

  // Make x as left child of y
  y->left = x;

  // Update parent pointer of x
  x->parent = y;
}


void rotate_right(node **root, node *y) {
  node *x = y->left;
  y->left = x->right;
  if (x->right != NULL) {
    x->right->parent = y;
  }
  x->parent =y->parent;
  if (x->parent == NULL) {
    (*root) = x;
  } else if (y == y->parent->left) {
    y->parent->left = x;
  } else {
    y->parent->right = x;
  }
  x->right = y;
  y->parent = x;
}


//
// Traverse arbitrary binary tree in inorder fashion
//
void traverse_inorder(node *node, void (*func)(int data)) {
  if (!node) { return; }

  traverse_inorder(node->left, func);
  func(node->data);
  traverse_inorder(node->right, func);
}
