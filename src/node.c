#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "node.h"

static const char *TypeString[] = {
  "nSTATIC", "nLITERAL"
};

void crb_node_init(bool debug) {
  node = (NodeTree*)malloc(sizeof(NodeTree));
  node->debug = debug;
  node->node = NULL;
}

void crb_node_add_node(NodeType type) {
  // TODO: implement this properly. For now just replacing the old node.
  if (node->node != NULL) {
    free(node->node);
  }
  node->node = (Node*)malloc(sizeof(Node));
  node->node->type = type;
  if (node->debug) {
    printf("NODE: %s\n", TypeString[type]);
  }
}

void crb_node_begin() {
}

void crb_node_rollback() {
}

void crb_node_commit() {
}
