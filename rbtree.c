//
// Tiny Red-Black Tree in C
//
// 프로그램 전체에 null pointer는 무조건 등장하지 않는 정책을 사용함.
// nil이라는 전역 객체의 주소값(NIL)을 NIL LEAF 포인터로 사용한다.
// NIL의 left, right, parent는 NIL.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>


//
// Node of red-black tree
//
typedef uint32_t data_t;
typedef enum { BLACK, RED } color_t;
typedef struct node {
  data_t data : 31;
  color_t color : 1;
  struct node *left, *right, *parent;
} node_t;

static node_t nil = { 0, BLACK, &nil, &nil, &nil };
static node_t *const NIL = &nil;

static void insert(node_t **root, data_t data);
static void traverse_inorder(node_t *root, void (*)(data_t data));
static void destroy(node_t **root);


//
// Test if rbtree works fine
//
static void per_node(data_t val) { printf("%u\n", val); }

int main(int argc, char *_[] __attribute__((unused))) {
  if (argc > 1) {
    printf(
        "sizeof(node_t)\n"
        "    %lu bytes\n"
        "    %lu bits\n", sizeof(node_t), sizeof(node_t) * 8);
    return 0;
  }

  node_t *root = NIL;
  data_t val;
  while(scanf("%u", &val) != EOF) {
    insert(&root, val);
  }
  traverse_inorder(root, per_node);
  destroy(&root);

  assert(NIL->left == NIL);
  assert(NIL->right == NIL);
  assert(NIL->parent == NIL);
  return 0;
}


//
// Helper functions
//
static node_t *grandparent(const node_t *n) {
  assert(n);
  assert(n->parent);

  return n != NIL && n->parent != NIL ? n->parent->parent : NIL;
}

static node_t *uncle(const node_t *n) {
  assert(n);

  node_t *g = grandparent(n);
  if (g == NIL) { return NIL; }

  return n->parent == g->left ? g->right : g->left;
}

static node_t *sibling(const node_t *n) {
  assert(n);
  assert(n->parent);

  return n == n->parent->left ? n->parent->right : n->parent->left;
}

static bool is_leaf(const node_t *n) {
  assert(n);
  assert(n != NIL);

  return n->left == NIL && n->right == NIL;
}

static void rotate_left(node_t *n) {
  assert(n);
  assert(n != NIL);

  node_t *c = n->right;
  node_t *p = n->parent;

  if (c->left != NIL) {
    c->left->parent = n;
  }

  n->right = c->left;
  n->parent = c;
  c->left = n;
  c->parent = p;

  if (p == NIL) { return; }
  if (p->left == n) {
    p->left = c;
  } else {
    p->right = c;
  }
}

static void rotate_right(node_t *n) {
  assert(n);
  assert(n != NIL);

  node_t *c = n->left;
  node_t *p = n->parent;

  if (c->right != NIL) {
    c->right->parent = n;
  }

  n->left = c->right;
  n->parent = c;
  c->right = n;
  c->parent = p;

  if (p == NIL) { return; }
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
  assert(root);
  assert(*root);

  // Allocate memory for a new node
  node_t *z = malloc(sizeof(node_t));
  z->data = data;
  z->left = z->right = z->parent = NIL;

  // If root is NIL, set z as root and return
  if (*root == NIL) {
    z->color = BLACK;
    (*root) = z;
    return;
  }

  // Standard BST insertion
  node_t *y, *x = (*root);
  while (x != NIL) {
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
  while ((*root)->parent != NIL) {
    *root = (*root)->parent;
  }
}

void insert_rec(node_t *n) {
  assert(n);

  // Case 1
  if (n->parent == NIL) {
    n->color = BLACK;
    return;
  }

  // Case 2
  if (n->parent->color == BLACK) { return; }

  // Case 3
  node_t *u = uncle(n);
  if ((u != NIL) && (u->color == RED)) {
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
  assert(node);
  assert(func);

  if (node == NIL) { return; }
  traverse_inorder(node->left, func);
  func(node->data);
  traverse_inorder(node->right, func);
}


//
// Deallocate tree
//
void destroy(node_t **root) {
  assert(root);
  assert(*root);

  if (*root == NIL) { return; }
  destroy(&(*root)->left);
  destroy(&(*root)->right);
  free(*root);
  *root = NIL;
}
