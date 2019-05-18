#ifndef LARCV_GRAPH_CXX
#define LARCV_GRAPH_CXX

#include "Graph.h"
#include <set>
#include <deque>
#include <assert.h>
namespace larcv {

  const std::vector<larcv::NodeID_t> Graph::primaries() const
  {
    std::vector<larcv::NodeID_t> res;
    for(auto const& n : _nodes) {
      if(!(n.valid())) continue;
      if(n.valid_parent()) continue;
      res.push_back(n.id);
    }
    return res;
  }
  
  std::vector<NodeID_t> Graph::secondaries(NodeID_t root) const {
      
    std::vector<size_t> res;
    if(!contains(root)) return res;
    
    std::set<NodeID_t> temp_res;
    std::deque<NodeID_t> node_list;
    node_list.push_back(root);
    while(node_list.size()) {
      auto const& node_id = node_list.front();
      if(contains(node_id)) {
	temp_res.insert(node_id);
	for(auto const& child : _nodes[node_id].children)
	  node_list.push_back(child);
      }
      node_list.pop_front();
    }
    for(auto const& node_id : temp_res) res.push_back(node_id);
    return res;
  }
  

  void Graph::insert(Node n) {
    // Check if the input node has valid ID. If not, we assign a new ID
    if(n.valid()) {
      // Resize container
      if(_nodes.size() <= n.id) _nodes.resize((n.id+1));
      // Assert this id is not yet registered
      assert(!(_nodes[n.id].valid()));
      // Set
      _nodes[n.id] = n;
    }else{
      n.id = _nodes.size();
      _nodes.push_back(n);
    }
    // Check claimed children
    for(auto const& child : n.children) {
      if(_nodes.size() <= child) {
	_nodes.resize((child+1));
	_nodes[child].id = child;
      }
      if(_nodes[child].parent != kINVALID_NODEID)
	assert(_nodes[child].parent == n.id);
      else
	_nodes[child].parent = n.id;
    }
    // Check claimed parent
    if(n.valid_parent()) {
      if(_nodes.size() <= n.parent) {
	_nodes.resize((n.parent+1));
	_nodes[n.parent].id = n.parent;
      }
      _nodes[n.parent].children.insert(n.id);
    }
  }

}
#endif


