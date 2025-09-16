#ifndef TREE_H
#define TREE_H 1

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/type_traits/is_pointer.hpp>

#include <boost/serialization/serialization.hpp> 
#include <boost/serialization/export.hpp> 
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
//#include <boost/archive/xml_oarchive.hpp>
//#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/ptr_container/serialize_ptr_vector.hpp>
#include <boost/serialization/string.hpp>

#include <iostream>

namespace Tree_AG{

  //
  // A Node
  //          a pure virtual class
  //
  class Node : boost::noncopyable{

    friend class boost::serialization::access;

  protected:
    typedef boost::ptr_vector<Node> Nodes;
    Nodes nodes_;

  private:

  public:
    virtual ~Node(){};

    virtual size_t n_children() const = 0;

    typedef Nodes::iterator iterator;
    typedef Nodes::const_iterator const_iterator;

    inline Node& child(size_t idx){ return nodes_[idx];}
    inline const Node& child(size_t idx) const { return nodes_[idx];}
    inline iterator begin(){ return nodes_.begin(); }
    inline const_iterator begin() const { return nodes_.begin();}
    inline iterator end(){ return nodes_.end(); }
    inline const_iterator end() const { return nodes_.end();}

    // Add a subtree
    // @param n ptr to node 
    //
    inline void add_child(Node * n){ nodes_.push_back(n); }
    
    // Remove a subtree
    // @param it Nodes::iterator
    //
    inline void remove_child(Nodes::iterator it){
      BOOST_ASSERT(it!=nodes_.end());
      nodes_.erase(it);
    }

    // serialization
    //
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
      ar & nodes_;
    }
  
  };

  //
  // An internal node inerith from node
  //
  template <typename T>
  class Inner : public Node{

    friend class boost::serialization::access;  
    
  private:
    T val_;

  public:
    Inner () {};

    Inner(T val) : val_(val)
    { std::cerr << "Created an Inner node with val=" << val << std::endl; }

    inline size_t n_children() const {return nodes_.size();}

    // serialization
    //
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
      ar & boost::serialization::base_object<Node>(*this);
      ar & val_;
    }

  };
  
  //
  // A leaf node inerith from node (and not from tree)
  //
  template <typename T>
  class Leaf : public Node {
    
    friend class boost::serialization::access;  

  private:
    T val_;

  public:

    Leaf(){};
    Leaf(T val) : val_(val)
    { std::cerr << "Created a Leaf node with val=" << val << std::endl; }

    inline size_t n_children() const {return 0;}

    // serialization
    //
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
      ar & boost::serialization::base_object<Node>(*this);
      ar & val_;
    }
  };

  // free functions
  // Depth first node
  // @param node n
  void depth_first(const Node& n);

};


#endif
