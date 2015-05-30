//
// Tiny Red-Black Tree in C
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

static void insert(node_t **root, data_t data);
static node_t *search(const node_t *root, data_t query);
static void delete(node_t **root, node_t *node);
static void traverse_inorder(node_t *root, void (*)(data_t data));
static void destroy(node_t **root);

static color_t node_color(node_t *n) {
  return n == NULL ? BLACK : n->color;
}


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

  node_t *root = NULL;
  while (true) {
    int cmd = getchar();
    data_t val;
    if (scanf("%u\n", &val) == EOF) { break; }

    if (cmd == 'i') {
      insert(&root, val);
    } else if (cmd == 'd') {
      node_t *n = search(root, val);
      delete(&root, n);
    } else {
      break;
    }
  }
  traverse_inorder(root, per_node);
  destroy(&root);
  return 0;
}


//
// Helper functions
//
static node_t *grandparent(const node_t *n) {
  return n != NULL && n->parent != NULL ? n->parent->parent : NULL;
}

static node_t *uncle(const node_t *n) {
  node_t *g = grandparent(n);
  if (g == NULL) { return NULL; }

  return n->parent == g->left ? g->right : g->left;
}

static node_t *sibling(const node_t *n) {
  assert(n != NULL);
  assert(n->parent != NULL);

  return n == n->parent->left ? n->parent->right : n->parent->left;
}

static void replace_node(node_t **root, node_t *old, node_t *new) {
  if (old->parent == NULL) {
    // old is root
    *root = new;
  } else if (old == old->parent->left) {
    // old is on parent's left
    old->parent->left = new;
  } else {
    // old is on parent's right
    old->parent->right = new;
  }

  if (new == NULL) { return; }
  new->parent = old->parent;
}

static node_t *maximum_node(node_t *n) {
  assert(n != NULL);
  while (n->right != NULL) {
    n = n->right;
  }
  return n;
}

static void swap_node(node_t *lhs, node_t *rhs) {
  assert(lhs != NULL);
  assert(rhs != NULL);
  assert(lhs->right != rhs);

  // Swap color
  color_t c = rhs->color;
  rhs->color = lhs->color;
  lhs->color = c;

  if (lhs->left == rhs) {
    lhs->left = rhs->left;
    rhs->parent = lhs->parent;
    lhs->parent = rhs;
    rhs->left = lhs;
    rhs->right = lhs->right;
    lhs->right = NULL;
    rhs->right->parent = rhs;
    if(lhs->left != NULL) {
      lhs->left->parent = lhs;
    }
    if (rhs->parent != NULL) {
      if(rhs->parent->left == lhs) {
        rhs->parent->left = rhs;
      } else {
        rhs->parent->right = rhs;
      }
    }
    return;
  }

  void swap_addr(node_t**, node_t**);

  // Swap backref
  if (lhs->left != NULL && rhs->left != NULL) {
    swap_addr(&lhs->left->parent, &rhs->left->parent);
  } else if (lhs->left != NULL) {
    lhs->left->parent = rhs;
  } else if (rhs->left != NULL) {
    rhs->left->parent = lhs;
  }

  if (lhs->right != NULL && rhs->right != NULL) {
    swap_addr(&lhs->right->parent, &rhs->right->parent);
  } else if (lhs->right != NULL) {
    lhs->right->parent = rhs;
  } else if (rhs->right != NULL) {
    rhs->right->parent = lhs;
  }

  node_t **l_backref = lhs->parent == NULL ? NULL :
    lhs->parent->left == lhs ? &lhs->parent->left : &lhs->parent->right;
  node_t **r_backref = rhs->parent == NULL ? NULL :
    rhs->parent->left == rhs ? &rhs->parent->left : &rhs->parent->right;
  if (lhs->parent != NULL && rhs->parent != NULL) {
    swap_addr(l_backref, r_backref);
  } else if (lhs->parent != NULL) {
    *l_backref = rhs;
  } else if (rhs->parent != NULL) {
    *r_backref = lhs;
  }

  // Swap itself
  swap_addr(&lhs->parent, &rhs->parent);
  swap_addr(&lhs->left, &rhs->left);
  swap_addr(&lhs->right, &rhs->right);
}

void swap_addr(node_t **lhs, node_t **rhs) {
  void *t = *rhs;
  *rhs = *lhs;
  *lhs = t;
}

static void rotate_left(node_t *n) {
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
void insert(node_t **root, data_t data) {
  assert(root);

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
  void insert_rec(node_t*);
  insert_rec(z);

  // Correct root node's position
  while ((*root)->parent != NULL) {
    assert(*root != (*root)->parent);
    *root = (*root)->parent;
  }
}

void insert_rec(node_t *n) {
  assert(n);

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
    return insert_rec(g);
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
// Search
//
node_t *search(const node_t *n, data_t query) {
  if (n == NULL) { return NULL; }
  if (n->data == query) { return (node_t*)n; }
  return n->data > query ? search(n->left, query) : search(n->right, query);
}


//
// Removal
//
void delete(node_t **root, node_t *n) {
  if (n == NULL) { return; }

  if (n->parent == NULL && n->left == NULL && n->right == NULL) {
    assert(*root == n);
    *root = NULL;
  } else {
    if (n->left != NULL && n->right != NULL) {
      // Copy key/value from predecessor and then delete it instead
      node_t *pred = maximum_node(n->left);
      swap_node(n, pred);
    }

    assert(n->left == NULL || n->right == NULL);
    node_t *child = n->right == NULL ? n->left  : n->right;
    if (node_color(n) == BLACK) {
      n->color = node_color(child);

      void delete_rec(node_t*);
      delete_rec(n);
    }
    replace_node(root, n, child);
    if (n->parent == NULL && child != NULL) {
      // root should be black
      child->color = BLACK;
    }

    // Correct root node's position
    if (*root == NULL) { return; }
    while ((*root)->parent != NULL) {
      assert(*root != (*root)->parent);
      *root = (*root)->parent;
    }
  }

  free(n);
}

void delete_rec(node_t *n) {
  // Case 1
  if (n->parent == NULL) { return; }

  // Case 2
  if (node_color(sibling(n)) == RED) {
    n->parent->color = RED;
    sibling(n)->color = BLACK;
    if (n == n->parent->left) {
      rotate_left(n->parent);
    } else {
      rotate_right(n->parent);
    }
  }

  // Case 3
  if (node_color(n->parent) == BLACK &&
      node_color(sibling(n)) == BLACK &&
      node_color(sibling(n)->left) == BLACK &&
      node_color(sibling(n)->right) == BLACK)
  {
    sibling(n)->color = RED;
    return delete_rec(n->parent);
  }

  // Case 4
  if (node_color(n->parent) == RED &&
      node_color(sibling(n)) == BLACK &&
      node_color(sibling(n)->left) == BLACK &&
      node_color(sibling(n)->right) == BLACK)
  {
    sibling(n)->color = RED;
    n->parent->color = BLACK;
    return;
  }

  // Case 5
  if (n == n->parent->left &&
      node_color(sibling(n)) == BLACK &&
      node_color(sibling(n)->left) == RED &&
      node_color(sibling(n)->right) == BLACK)
  {
    sibling(n)->color = RED;
    sibling(n)->left->color = BLACK;
    rotate_right(sibling(n));
  } else if (n == n->parent->right &&
      node_color(sibling(n)) == BLACK &&
      node_color(sibling(n)->right) == RED &&
      node_color(sibling(n)->left) == BLACK)
  {
    sibling(n)->color = RED;
    sibling(n)->right->color = BLACK;
    rotate_left(sibling(n));
  }

  // Case 6
  sibling(n)->color = node_color(n->parent);
  n->parent->color = BLACK;
  if (n == n->parent->left) {
    assert (node_color(sibling(n)->right) == RED);
    sibling(n)->right->color = BLACK;
    rotate_left(n->parent);
  } else {
    assert (node_color(sibling(n)->left) == RED);
    sibling(n)->left->color = BLACK;
    rotate_right(n->parent);
  }
}


//
// Traverse arbitrary binary tree in inorder fashion
//
void traverse_inorder(node_t *node, void (*func)(data_t data)) {
  assert(func);

  if (node == NULL) { return; }
  traverse_inorder(node->left, func);
  func(node->data);
  traverse_inorder(node->right, func);
}


//
// Deallocate tree
//
void destroy(node_t **root) {
  assert(root);

  if (*root == NULL) { return; }
  destroy(&(*root)->left);
  destroy(&(*root)->right);
  free(*root);
  *root = NULL;
}
