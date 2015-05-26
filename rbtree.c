#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>


//
// Node of red-black tree
//
typedef long data_t;
typedef enum { RED, BLACK } color_t;
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
static void per_node(data_t val) { printf("%ld\n", val); }

int main() {
  node_t *root = NULL;
  long val;
  while(scanf("%ld", &val) != EOF) { insert(&root, val); }
  traverse_inorder(root, per_node);
  return 0;
}


//
// Helper functions
//
static node_t *grandparent(node_t *n) {
  return n && n->parent ? n->parent->parent : NULL;
}

static node_t *uncle(node_t *n) {
  node_t *g = grandparent(n);
  if (!g) { return NULL; }

  return n->parent == g->left ? g->right : g->left;
}

static node_t *sibling(node_t *n) {
  // Precondition
  assert(n != NULL);

  return n == n->parent->left ? n->parent->right : n->parent->left;
}

static bool is_leaf(node_t *n) {
  // Precondition
  assert(n != NULL);

  return n->left == NULL && n->right == NULL;
}

static void rotate_left(node_t *n) {
  // Precondition
  assert(n != NULL);

  node_t *c = n->right;
  node_t *p = n->parent;

  if (c->left != NULL) {
    c->left->parent = n;
  }

  n->right = c->left;
  n->parent = c;
  c->left = n;
  c->parent = p;

  if (p == NULL) { return; }
  if (p->left == n) {
    p->left = c;
  } else {
    p->right = c;
  }
}

static void rotate_right(node_t *n) {
  // Precondition
  assert(n != NULL);

  node_t *c = n->left;
  node_t *p = n->parent;

  if (c->right != NULL) {
    c->right->parent = n;
  }

  n->left = c->right;
  n->parent = c;
  c->right = n;
  c->parent = p;

  if (p == NULL) { return; }
  if (p->left == n) {
    p->left = c;
  } else {
    p->right = c;
  }
}


//
// Insert
//
static void insert_rec(node_t*);

void insert(node_t **root, data_t data) {
  // Allocate memory for a new node
  node_t *z = malloc(sizeof(node_t));
  z->data = data;
  z->left = z->right = z->parent = NULL;

  // If root is NULL, set z as root and return
  if (*root == NULL) {
    z->color = BLACK;
    (*root) = z;
    return;
  }

  // Standard BST insertion
  node_t *y, *x = (*root);
  while (x != NULL) {
    y = x;
    x = (z->data < x->data) ? x->left : x->right;
  }
  z->color = RED;
  z->parent = y;
  if (z->data < y->data) {
    y->left = z;
  } else {
    y->right = z;
  }

  // Fixup red-black tree
  insert_rec(z);

  // Correct root node's position
  while ((*root)->parent) {
    *root = (*root)->parent;
  }
}

void insert_rec(node_t *n) {
  // Precondition
  assert(n != NULL);

  // Case 1
  if (n->parent == NULL) {
    n->color = BLACK;
    return;
  }

  // Case 2
  if (n->parent->color == BLACK) { return; }

  // Case 3
  node_t *u = uncle(n);
  if ((u != NULL) && (u->color == RED)) {
    n->parent->color = BLACK;
    u->color = BLACK;
    node_t *g = grandparent(n);
    g->color = RED;
    insert_rec(g);
    return;
  }

  // Case 4
  node_t *g = grandparent(n);
  if ((n == n->parent->right) && (n->parent == g->left)) {
    rotate_left(n->parent);
    n = n->left;
  } else if ((n == n->parent->left) && (n->parent == g->right)) {
    rotate_right(n->parent);
    n = n->right;
  }

  // Case 5
  n->parent->color = BLACK;
  g = grandparent(n);
  g->color = RED;
  if (n == n->parent->left) {
    rotate_right(g);
  } else {
    rotate_left(g);
  }
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
