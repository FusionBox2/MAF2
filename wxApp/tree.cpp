#include "tree.hpp"
#include <boost/foreach.hpp>
namespace Tree_AG{

  void processNode(const Node * n){

  }

  // Depth first node
  // @param node n
  void depth_first(const Node& n){

    if (n.n_children()==0)
      std::cout << "Leaf" << std::endl;
    else{ 
      std::cout << "\tInner " << std::endl;

      Node::const_iterator it = n.begin();
      Node::const_iterator it_end = n.end();
      for (; it != it_end; ++it){
	depth_first(*it);
      }
    }
  }

};
