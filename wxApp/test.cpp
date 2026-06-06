#include "tree.hpp"

#include <fstream>
#include <iostream>


using namespace Tree_AG;

//
// FOR SERIALIZATION:
//   I NEED TO MANUALLY REGISTER THE INSTANTATION OF TEMPLATE
//   INCLUDE THE NEEDED #include 
//
  using namespace std;
  BOOST_CLASS_EXPORT_GUID( Node, "node" );
  BOOST_CLASS_EXPORT_GUID( Inner< string >, "inner_string" );
  BOOST_CLASS_EXPORT_GUID( Inner< int >, "inner_int");
  BOOST_CLASS_EXPORT_GUID( Inner< double >, "inner_double");
  BOOST_CLASS_EXPORT_GUID( Leaf< int >, "leaf_int" );
  BOOST_CLASS_EXPORT_GUID( Leaf< string >, "leaf_string" );
  BOOST_CLASS_EXPORT_GUID( Leaf< double >, "leaf_double" );

//

int main(){
   
  Inner<std::string> root("root");
    
  Leaf<std::string> * l = new Leaf<std::string>("leaf test son of root");
  root.add_child(l);

  Inner<std::string> * is = new Inner<std::string>("inner test son of root");
  root.add_child(is);
  
  Leaf<int> * li = new Leaf<int>(10);
  is->add_child(li);

  Inner<int> * n1  = new Inner<int>(1);
  root.add_child(n1);

  //std::cout << std::endl << "******** Depth first visit" << std::endl;
  //depth_first(root);
  
  
  std::cout << std::endl << "******** Serializing" << std::endl;

  // create and open a character archive for output
  std::ofstream ofs("serial");
  boost::archive::text_oarchive oa(ofs);
  // write class instance to archive
  oa << root;

  std::cout << std::endl << "******** Done" << std::endl;

}


