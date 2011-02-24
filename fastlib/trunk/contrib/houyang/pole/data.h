#ifndef DATA_H
#define DATA_H

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "sparsela.h"

using namespace std;

enum FileFormat {
  csv, arff, svmlight, unknown
};

class Data {
 public:
  // input parameters
  size_t n_source_;
  string fn_;
  FILE   *fp_;
  FileFormat ff_; 
  size_t port_;
  bool   random_;
  // data properties
  vector<Example> EXs_; // examples
  size_t n_ex_; // number of examples
  size_t n_ln_; // number of lines of example file
  // for svmlight format
  size_t max_n_nz_ft_; // maximum number of non-0 features
  size_t max_l_ln_; // maximum length of a text line
  // data stream properties
 public:
  Data(string fn, size_t port, bool random);
 private:
  void InitFromSvmlight();
  void InitFromCsv();
  void InitFromArff();
  bool SorN(int c);
 public:
  bool ReadFileInfo();
  void ReadFromFile();
  void ReadFromPort();
  void Print();
  
  bool GetExample();
};


#endif
