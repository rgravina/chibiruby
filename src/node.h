#include <stdbool.h>

typedef enum nodeType {nSCOPE, nLITERAL} NodeType;
typedef struct node {
  NodeType type;
  struct node* next;
} Node;

typedef struct nodeTree {
  bool debug;
  Node* root;
  Node* tail;
} NodeTree;

// node root
NodeTree* tree;

void crb_node_init(bool debug);
void crb_node_rollback();
void crb_node_commit();
void crb_node_add_node(NodeType type);
void crb_node_print_tree();
