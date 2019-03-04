#include "WireInfoData.h"

namespace divalgo {

  void getUstart( const int wireid, std::vector<float>& pos ) {
    pos.resize(3);
    for (int i=0; i<3; i++)
      pos.at(i) = uplaneStart[wireid][i];
  };

  void getUend( const int wireid, std::vector<float>& pos ) {
    pos.resize(3);
    for (int i=0; i<3; i++)
      pos.at(i) = uplaneEnd[wireid][i];
  };

  void getVstart( const int wireid, std::vector<float>& pos ) {
    pos.resize(3);
    for (int i=0; i<3; i++)
      pos.at(i) = vplaneStart[wireid][i];
  };

  void getVend( const int wireid, std::vector<float>& pos ) {
    pos.resize(3);
    for (int i=0; i<3; i++)
      pos.at(i) = vplaneEnd[wireid][i];
  };

  void getYstart( const int wireid, std::vector<float>& pos ) {
    pos.resize(3);
    for (int i=0; i<3; i++)
      pos.at(i) = yplaneStart[wireid][i];
  };

  void getYend( const int wireid, std::vector<float>& pos ) {
    pos.resize(3);
    for (int i=0; i<3; i++)
      pos.at(i) = yplaneEnd[wireid][i];
  };

}
