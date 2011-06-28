/***
 * @file nca_main.cc
 * @author Ryan Curtin
 *
 * Executable for Neighborhood Components Analysis.
 */
#include <fastlib/fastlib.h>
#include <mlpack/core/kernels/lmetric.h>

#include "nca.h"

// Define parameters.
PROGRAM_INFO("Neighborhood Components Analysis", 
    "documentation not done yet", "");

PARAM_STRING_REQ("input_file", "Input dataset to run NCA on.", "");
PARAM_STRING_REQ("output_file", "Output file for learned distance matrix.", "");

using namespace mlpack;
using namespace mlpack::nca;
using namespace mlpack::kernel;
using namespace std;

int main(int argc, char* argv[]) {
  IO::ParseCommandLine(argc, argv);

  arma::mat data;
  data::Load(IO::GetParam<string>("input_file").c_str(), data);

  arma::uvec labels(data.n_cols);
  for (index_t i = 0; i < data.n_cols; i++)
    labels[i] = (int) data(data.n_rows - 1, i);

  data.shed_row(data.n_rows - 1);

  NCA<LMetric<2> > nca(data, labels);

  arma::mat distance;

  nca.LearnDistance(distance);

  data::Save(IO::GetParam<string>("output_file").c_str(), distance);
}
