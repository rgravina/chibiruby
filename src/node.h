#include <stdbool.h>

typedef enum nodeType {nSCOPE, nLITERAL} NodeType;
typedef struct node {
  NodeType type;
} Node;

typedef struct nodeTree {
  bool debug;
  Node* node;
} NodeTree;

// node root
NodeTree* node;

void crb_node_init(bool debug);
void crb_node_begin();
void crb_node_rollback();
void crb_node_commit();
void crb_node_add_node(NodeType type);
