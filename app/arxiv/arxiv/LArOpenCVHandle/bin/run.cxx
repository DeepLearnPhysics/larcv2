#include "Processor/ProcessDriver.h"

int main(int argc, char** argv) {

  larcv::ProcessDriver proc("ProcessDriver");
  auto cfg = std::string(argv[1]);
  std::cout << "Got configuration: " << cfg << std::endl;
  proc.configure(cfg);
  
  auto file = std::string(argv[2]);
  std::cout << "Got file: " << file << std::endl;
  proc.override_input_file(std::vector<std::string>(1,file));

  proc.override_ana_file("aho1.root");
  proc.override_output_file("aho2.root");

  proc.initialize();

  proc.batch_process(0,1);

  proc.finalize();

  return 0;
}
