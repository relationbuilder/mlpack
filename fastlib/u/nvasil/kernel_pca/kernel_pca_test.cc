/*
 * =====================================================================================
 *
 *       Filename:  kernel_pca_test.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/09/2008 11:26:48 AM EST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Nikolaos Vasiloglou (NV), nvasil@ieee.org
 *        Company:  Georgia Tech Fastlab-ESP Lab
 *
 * =====================================================================================
 */

#include "u/nvasil/kernel_pca/kernel_pca.h"
#include "fastlib/fastlib.h"
#include "base/test.h"
#include <vector>

class KernelPCATest {
 public:    
  void Init() {
    engine_ = new KernelPCA();
    engine_->Init("test_data_3_1000.csv", 5, 20);
  }
  void Destruct() {
    delete engine_;
  }
  void TestGeneralKernelPCA() {
   NOTIFY("Testing KernelPCA...\n");
   Matrix eigen_vectors;
   Vector eigen_values;
   Init();
   engine_->ComputeNeighborhoods();
   double bandwidth;
   engine_->EstimateBandwidth(&bandwidth);
   NOTIFY("Estimated bandwidth %lg ...\n", bandwidth);
   kernel_.set(bandwidth); 
   engine_->LoadAffinityMatrix();
   engine_->ComputeGeneralKernelPCA(kernel_, 5, 
                                    &eigen_vectors,
                                    &eigen_values);

   engine_->SaveToTextFile("results", eigen_vectors, eigen_values);
   Destruct();
   NOTIFY("Test ComputeGeneralKernelPCA passed...!\n");
  }
  void TestLLE() {
    NOTIFY("Testing ComputeLLE\n");
    Matrix eigen_vectors;
    Vector eigen_values;
    Init();
    engine_->ComputeNeighborhoods();
    engine_->LoadAffinityMatrix();
    engine_->ComputeLLE(5,
                         &eigen_vectors,
                         &eigen_values);
    engine_->SaveToTextFile("results", eigen_vectors, eigen_values);
    Destruct();
    NOTIFY("Test ComputeLLE passed...!\n");
  }
  void TestSpectralRegression() {
    NOTIFY("Test ComputeSpectralRegression...\n");
    Matrix embedded_coordinates;
    Vector eigenvalues;
    Init();
    engine_->Compute_neighborhoods();
    double bandwidth;
    engine_->EstimateBandwidth(&bandwidth);
    NOTIFY("Estimated bandwidth %lg ...\n", bandwidth);
    kernel_.set(bandwidth); 
    engine_->LoadAffinityMatrix();
    std::map<index_t, index_t> data_label;
    Matrix embedded_coordinates;
    Vector eigenvalues; 
    engine_->ComputeSpectralRegression(data_label,
                                       &embedded_coordinates, 
                                       &eigenvalues);
    engine_->SaveToTextFile("results", embedded_coords, eigen_values);
    Destruct();

    NOTIFY("Test ComputeSpectralRegression passed...\n"); 
  }
  void TestAll() {
     TestGeneralKernelPCA();
     TestLLE();
     TestSpectralRegression();
  }
 private:
  KernelPCA *engine_;
  KernelPCA::GaussianKernel kernel_;
};

int main() {
  KernelPCATest test;
  test.TestAll();
}
