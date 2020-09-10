//
// Tiny red-black tree implementation written in C
//

//
// Copyright (c) 2015, Hyeon Kim
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
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
  data_t _data : 31;
  color_t _color : 1;
  struct node *left, *right, *parent;
} node_t;

static inline data_t get_data(const node_t* this) { return this->_data; }
static inline void set_data(node_t* this, data_t data) { this->_data = data; }

static inline color_t get_color(const node_t *this) { return this == NULL ? BLACK : this->_color; }
static inline void set_color(node_t *this, color_t color) { this->_color = color; }


//
// Interfaces
//
void insert(node_t **root, node_t *node);
node_t *search(const node_t *root, data_t query);
node_t *best_fit(const node_t *root, data_t query);
void delete(node_t **root, node_t *node);
void traverse_inorder(node_t *root, void (*)(data_t data));


//
// Test codes
//
static void per_node(data_t val) { printf("%u\n", val); }
static void destroy(node_t **root) {
  assert(root);

  if (*root == NULL) { return; }
  destroy(&(*root)->left);
  destroy(&(*root)->right);
  free(*root);
  *root = NULL;
}

int main(int argc, char *_[] __attribute__((unused))) {
  // Handle command line argument
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
      node_t *n = malloc(sizeof(node_t));
      set_data(n, val);
      n->left = n->right = n->parent = NULL;

      insert(&root, n);
    } else if (cmd == 'd') {
      node_t *n = search(root, val);

      delete(&root, n);

      free(n);
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
  color_t c = get_color(rhs);
  set_color(rhs, get_color(lhs));
  set_color(lhs, c);

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
void insert(node_t **root, node_t *n) {
  assert(root);

  // If root is NULL, set n as root and return
  if (*root == NULL) {
    set_color(n, BLACK);
    (*root) = n;
    return;
  }

  // Standard BST insertion
  node_t *y, *x = (*root);
  while (x != NULL) {
    y = x;
    x = (get_data(n) < get_data(x)) ? x->left : x->right;
    assert(y != x);
  }
  set_color(n, RED);
  assert(n != y);
  n->parent = y;
  if (get_data(n) < get_data(y)) {
    y->left = n;
  } else {
    y->right = n;
  }

  // Fixup red-black tree
  void insert_rec(node_t*);
  insert_rec(n);

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
    set_color(n, BLACK);
    return;
  }

  // Case 2
  if (get_color(n->parent) == BLACK) { return; }

  // Case 3
  node_t *u = uncle(n);
  if ((u != NULL) && (get_color(u) == RED)) {
    set_color(n->parent, BLACK);
    set_color(u, BLACK);
    node_t *g = grandparent(n);
    set_color(g, RED);
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
  set_color(n->parent, BLACK);
  g = grandparent(n);
  set_color(g, RED);
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
  if (get_data(n) == query) { return (node_t*)n; }
  return get_data(n) > query ? search(n->left, query) : search(n->right, query);
}


//
// Best fit
//
node_t *best_fit(const node_t *n, data_t query) {
  if (n == NULL) { return NULL; }
  if (get_data(n) == query) { return (node_t*)n; }
  if (get_data(n) < query) { return best_fit(n->right, query); }

  node_t *try = best_fit(n->left, query);
  if (try == NULL || get_data(try) < query) { return (node_t*)n; }
  return try;
}


//
// Removal
//
void delete(node_t **root, node_t *n) {
  if (n == NULL) { return; }
  if (n->parent == NULL && n->left == NULL && n->right == NULL) {
    assert(*root == n);
    *root = NULL;
    return;
  }

  if (n->left != NULL && n->right != NULL) {
    // Copy key/value from predecessor and then delete it instead
    node_t *pred = maximum_node(n->left);
    swap_node(n, pred);
  }

  assert(n->left == NULL || n->right == NULL);
  node_t *child = n->right == NULL ? n->left  : n->right;
  if (get_color(n) == BLACK) {
    set_color(n, get_color(child));

    void delete_rec(node_t*);
    delete_rec(n);
  }
  replace_node(root, n, child);
  if (n->parent == NULL && child != NULL) {
    // root should be black
    set_color(child, BLACK);
  }

  // Correct root node's position
  if (*root == NULL) { return; }
  while ((*root)->parent != NULL) {
    assert(*root != (*root)->parent);
    *root = (*root)->parent;
  }
}

void delete_rec(node_t *n) {
  // Case 1
  if (n->parent == NULL) { return; }

  // Case 2
  if (get_color(sibling(n)) == RED) {
    set_color(n->parent, RED);
    set_color(sibling(n), BLACK);
    if (n == n->parent->left) {
      rotate_left(n->parent);
    } else {
      rotate_right(n->parent);
    }
  }

  // Case 3
  if (get_color(n->parent) == BLACK &&
      get_color(sibling(n)) == BLACK &&
      get_color(sibling(n)->left) == BLACK &&
      get_color(sibling(n)->right) == BLACK)
  {
    set_color(sibling(n), RED);
    delete_rec(n->parent);
    return;
  }

  // Case 4
  if (get_color(n->parent) == RED &&
      get_color(sibling(n)) == BLACK &&
      get_color(sibling(n)->left) == BLACK &&
      get_color(sibling(n)->right) == BLACK)
  {
    set_color(sibling(n), RED);
    set_color(n->parent, BLACK);
    return;
  }

  // Case 5
  if (n == n->parent->left &&
      get_color(sibling(n)) == BLACK &&
      get_color(sibling(n)->left) == RED &&
      get_color(sibling(n)->right) == BLACK)
  {
    set_color(sibling(n), RED);
    set_color(sibling(n)->left, BLACK);
    rotate_right(sibling(n));
  } else if (n == n->parent->right &&
      get_color(sibling(n)) == BLACK &&
      get_color(sibling(n)->right) == RED &&
      get_color(sibling(n)->left) == BLACK)
  {
    set_color(sibling(n), RED);
    set_color(sibling(n)->right, BLACK);
    rotate_left(sibling(n));
  }

  // Case 6
  set_color(sibling(n), get_color(n->parent));
  set_color(n->parent, BLACK);
  if (n == n->parent->left) {
    assert(get_color(sibling(n)->right) == RED);
    set_color(sibling(n)->right, BLACK);
    rotate_left(n->parent);
  } else {
    assert(get_color(sibling(n)->left) == RED);
    set_color(sibling(n)->left, BLACK);
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
  func(get_data(node));
  traverse_inorder(node->right, func);
}
