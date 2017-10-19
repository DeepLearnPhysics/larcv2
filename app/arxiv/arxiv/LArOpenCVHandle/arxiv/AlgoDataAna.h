#ifndef __ALGODATAANA_H__
#define __ALGODATAANA_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
#include "TChain.h"
#include "TTree.h"
#include "LArbysRecoHolder.h"

namespace larcv {
  class AlgoDataAna : public ProcessBase {
  public:
    AlgoDataAna(const std::string name="AlgoDataAna");
    ~AlgoDataAna(){}

    void configure(const PSet&);
    void initialize();
    bool process(IOManager& mgr) { return true; } 
    void finalize();

    LArOCVSerial* _larocvserial;

    TChain* _reco_chain;
    size_t _reco_index;
    size_t _reco_entries;

    std::string _input_larbys_reco_root_file;
    
    uint _reco_run;
    uint _reco_subrun;
    uint _reco_event;
    uint _reco_entry;

    std::string _reco_tree_name;
    std::string _serial_name;

    TTree* _vertex_tree;
    double _reco_vertex_x;
    double _reco_vertex_y;
    double _reco_vertex_z;
  };

  class AlgoDataAnaProcessFactory : public ProcessFactoryBase {
  public:
    AlgoDataAnaProcessFactory() { ProcessFactory::get().add_factory("AlgoDataAna",this); }
    ~AlgoDataAnaProcessFactory() {}
    ProcessBase* create(const std::string instance_name) { return new AlgoDataAna(instance_name); }
  };
}
#endif
/** @} */ // end of doxygen group 

