
#ifndef __LARBYSIMAGEANABASE_H__
#define __LARBYSIMAGEANABASE_H__

#include <TFile.h>
#include "Base/larcv_base.h"
#include "Base/PSet.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterManager.h"

namespace larcv {

  class LArbysImageAnaBase : public larcv_base {

  public:
    
    LArbysImageAnaBase(const std::string name="LArbysImageAnaBase");
    ~LArbysImageAnaBase(){}

    virtual void Configure(const larcv::PSet&) = 0;
    virtual void Initialize () = 0;
    virtual bool Analyze    (larocv::ImageClusterManager& mgr) =0;
    virtual void Finalize   (TFile* fout = nullptr) = 0;
      
    void EventID(const size_t entry,
		 const size_t run, const size_t subrun, const size_t event)
    { _entry = entry; _run = run; _subrun = subrun; _event = event; }

  protected:

    /// Unique event keys
    size_t _run;
    size_t _subrun;
    size_t _event;
    size_t _entry;
  };

}

#endif
