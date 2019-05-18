/**
 * \file Graph.h
 *
 * \ingroup DataFormat
 * 
 * \brief Class def header for a class Graph
 *
 * @author kazuhiro
 */

/** \addtogroup DataFormat

    @{*/
#ifndef LARCV_GRAPH_H
#define LARCV_GRAPH_H

#include "DataFormatTypes.h"
#include <iostream>
#include <cmath>
namespace larcv {

  /**
     \class Node
  */
  class Node {
  public:
    Node() : id(kINVALID_NODEID), parent(kINVALID_NODEID) {}
    ~Node() {}

    NodeID_t id;
    NodeID_t parent;
    std::set<larcv::NodeID_t> children;

    inline bool valid() const { return id != kINVALID_NODEID; }
    inline bool valid_parent() const { return parent != kINVALID_NODEID; }
  };


  /**
     \class Graph
  */
  class Graph {
  public:
    Graph() {}
    ~Graph() {}

    inline const std::vector<larcv::Node>& Nodes() const { return _nodes; }

    inline bool contains(const NodeID_t& id) const { return (_nodes.size() > id); }

    inline bool contains(const Node& n) const { return contains(n.id); }

    const std::vector<larcv::NodeID_t> primaries() const;

    std::vector<NodeID_t> secondaries(NodeID_t root) const;

    void insert(Node n);

    void clear() { _nodes.clear(); }
    
  private:
    std::vector<larcv::Node> _nodes;
  };
  
}
#endif
/** @} */ // end of doxygen group 

