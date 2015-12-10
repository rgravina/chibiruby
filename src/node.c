#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "node.h"

static const char *TypeString[] = {
  "nSCOPE", "nLITERAL"
};

// for adding to before we know the production was matched.
static Node* tempNodeRoot;
static Node* tempNodeTail;

void crb_node_init(bool debug) {
  tree = (NodeTree*)malloc(sizeof(NodeTree));
  tree->debug = debug;
  tree->root = NULL;
  tree->tail = NULL;
  tempNodeRoot = NULL;
  tempNodeTail = NULL;
}

void crb_node_add_node(NodeType type) {
  // TODO: implement this properly. For now just replacing the old node.
  Node* node = (Node*)malloc(sizeof(Node));
  node->type = type;
  node->next = NULL;
  if (tempNodeRoot == NULL) {
    tempNodeRoot = tempNodeTail = node;    
  } else {
    tempNodeTail->next = node;    
    tempNodeTail = node;
  }

  if (tree->debug) {
    printf("NODE: %s\n", TypeString[type]);
  }
}

void crb_node_rollback() {
  // free the temporary tree
  if (tree->debug) {
    puts("rolling back nodes...");
  }
  Node* node = tempNodeRoot;
  Node* tempNode;
  while(node != NULL) {
    tempNode = node;
    node = node->next;
    if (tree->debug) {
      printf("freeing: %s\n", TypeString[tempNode->type]);
    }
    free(tempNode);
  }
  tempNodeRoot = tempNodeTail = NULL;
}

void crb_node_commit() {
  // point the tail of the real node tree to the temp tree
  if (tempNodeRoot == NULL) {
    return;
  }
  if (tree->debug) {
    puts("committing nodes...");
  }
  if (tree->root == NULL) {
    tree->root = tree->tail = tempNodeRoot;
  } else {
    Node* node = tree->tail;
    node->next = tempNodeRoot;
    tree->tail = tempNodeRoot;
  }
  tempNodeRoot = tempNodeTail = NULL;
}

void crb_node_print_tree() {
  if (tree->root == NULL) {
    puts("No nodes.");
  } else {
    puts("\n=== NODES ===");
    Node* node = tree->root;
    while(node != NULL) {
      printf("node: %s\n", TypeString[node->type]);
      node = node->next;
    }
  }
}

