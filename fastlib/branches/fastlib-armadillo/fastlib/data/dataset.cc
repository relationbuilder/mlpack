/* MLPACK 0.2
 *
 * Copyright (c) 2008, 2009 Alexander Gray,
 *                          Garry Boyer,
 *                          Ryan Riegel,
 *                          Nikolaos Vasiloglou,
 *                          Dongryeol Lee,
 *                          Chip Mappus, 
 *                          Nishant Mehta,
 *                          Hua Ouyang,
 *                          Parikshit Ram,
 *                          Long Tran,
 *                          Wee Chin Wong
 *
 * Copyright (c) 2008, 2009 Georgia Institute of Technology
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
/**
 * @file dataset.cc
 *
 * Implementations for the dataset utilities.
 * 
 * @bug These routines fail when trying to read files linewise that use the Mac
 * eol '\r'.  Both Windows and Unix eol ("\r\n" and '\n') work.  Use the
 * programs 'dos2unix' or 'tr' to convert the '\r's to '\n's.
 *
 */

#include "../base/base.h"

#include "dataset.h"

index_t Dataset::n_labels() const {
  index_t i = 0;
  index_t label_row_idx = matrix_.n_rows - 1; // the last row is for labels
  index_t n_labels = 0;

  double current_label;
  
  ArrayList<double> labels_list;
  labels_list.Init();
  labels_list.PushBack() = matrix_(label_row_idx, 0);
  n_labels++;

  for(i = 1; i < matrix_.n_cols; i++) {
    current_label = matrix_(label_row_idx, i);
    index_t j = 0;
    for (j = 0; j < n_labels; j++) {
      if (current_label == labels_list[j]) {
        break;
      }
    }
    if(j == n_labels) { // new label
      labels_list.PushBack() = current_label;
      n_labels++;
    }
  }

  labels_list.Clear();
  return n_labels;
}

void Dataset::GetLabels(ArrayList<double> &labels_list,
                        ArrayList<index_t> &labels_index,
                        ArrayList<index_t> &labels_ct,
                        ArrayList<index_t> &labels_startpos) const {
  index_t i = 0;
  index_t label_row_idx = matrix_.n_rows - 1; // the last row is for labels
  index_t n_points = matrix_.n_cols;
  index_t n_labels = 0;

  double current_label;

  // these Arraylists need initialization before-hand
  labels_list.Renew();
  labels_index.Renew();
  labels_ct.Renew();
  labels_startpos.Renew();

  labels_index.Init(n_points);
  labels_list.Init();
  labels_ct.Init();
  labels_startpos.Init();

  ArrayList<index_t> labels_temp;
  labels_temp.Init(n_points);
  labels_temp[0] = 0;

  labels_list.PushBack() = matrix_(label_row_idx, 0);
  labels_ct.PushBack() = 1;
  n_labels++;

  for (i = 1; i < n_points; i++) {
    current_label = matrix_(label_row_idx, i);
    index_t j = 0;
    for (j = 0; j < n_labels; j++) {
      if (current_label == labels_list[j]) {
        labels_ct[j]++;
	break;
      }
    }
    labels_temp[i] = j;
    if (j == n_labels) { // new label
      labels_list.PushBack() = current_label; // add new label to list
      labels_ct.PushBack() = 1;
      n_labels++;
    }
  }
  
  labels_startpos.PushBack() = 0;
  for(i = 1; i < n_labels; i++){
    labels_startpos.PushBack() = labels_startpos[i - 1] + labels_ct[i - 1];
  }

  for(i = 0; i < n_points; i++) {
    labels_index[labels_startpos[labels_temp[i]]] = i;
    labels_startpos[labels_temp[i]]++;
  }

  labels_startpos[0] = 0;
  for(i = 1; i < n_labels; i++)
    labels_startpos[i] = labels_startpos[i - 1] + labels_ct[i - 1];

  labels_temp.Clear();
}

success_t Dataset::InitFromFile(const char *fname) {
  TextLineReader reader;

  if (PASSED(reader.Open(fname))) {
    return InitFromFile(reader, fname);
  } else {
    matrix_.reset();
    info_.Init();
    NONFATAL("Could not open file '%s' for reading.", fname);
    return SUCCESS_FAIL;
  }
}

success_t Dataset::InitFromFile(TextLineReader& reader,
    const char *filename) {
  success_t result;

  result = info_.InitFromFile(reader, filename);
  if (PASSED(result)) {
    result = info_.ReadMatrix(reader, matrix_);
  } else {
    matrix_.reset();
  }

  return result;
}


success_t Dataset::WriteCsv(const char *fname, bool header) const {
  TextWriter writer;

  if (!PASSED(writer.Open(fname))) {
    NONFATAL("Couldn't open '%s' for writing.", fname);
    return SUCCESS_FAIL;
  } else {
    if (header) {
      info_.WriteCsvHeader(",\t", writer);
    }
    info_.WriteMatrix(matrix_, ",\t", writer);
    return writer.Close();
  }
}

success_t Dataset::WriteArff(const char *fname) const {
  TextWriter writer;

  if (!PASSED(writer.Open(fname))) {
    NONFATAL("Couldn't open '%s' for writing.", fname);
    return SUCCESS_FAIL;
  } else {
    info_.WriteArffHeader(writer);
    info_.WriteMatrix(matrix_, ",", writer);
    return writer.Close();
  }
}

void Dataset::SplitTrainTest(int folds, int fold_number,
    const ArrayList<index_t>& permutation,
    Dataset& train, Dataset& test) const {
  // determine number of points in test and training sets
  index_t n_test = (n_points() + folds - fold_number - 1) / folds;
  index_t n_train = n_points() - n_test;

  // initialize blank training data set
  train.InitBlank();
  train.info().InitCopy(info());

  // initialize blank testing data set
  test.InitBlank();
  test.info().InitCopy(info());

  // set sizes of training and test datasets
  train.matrix().set_size(n_features(), n_train);
  test.matrix().set_size(n_features(), n_test);

  index_t i_train = 0;
  index_t i_test = 0;
  index_t i_orig = 0;

  for (i_orig = 0; i_orig < n_points(); i_orig++) {
    double *dest;

    if (unlikely((i_orig - fold_number) % folds == 0)) {
      // put this column into the test set
      dest = test.matrix().colptr(i_test);
      i_test++;
    } else {
      // put this column into the training set
      dest = train.matrix().colptr(i_train);
      i_train++;
    }

    // copy the column over in memory
    memcpy(dest,
        this->matrix().colptr(permutation[i_orig]),
        sizeof(double) * n_features());
  }

  DEBUG_ASSERT(i_train == train.n_points());
  DEBUG_ASSERT(i_test == test.n_points());
}

success_t data::Load(const char *fname, arma::mat& matrix) {
  TextLineReader reader;
  DatasetInfo info; // we will ignore this, but it reads our matrix
  success_t result;

  // clear our matrix
  matrix.reset();

  if (PASSED(reader.Open(fname))) {
    // read our file, since it has successfully opened
    result = info.InitFromFile(reader, fname);
    if (PASSED(result)) {
      result = info.ReadMatrix(reader, matrix);
    }
  } else {
    NONFATAL("Could not open file '%s' for reading.", fname);
    return SUCCESS_FAIL;
  }

  return result;
}

success_t data::Save(const char *fname, const arma::mat& matrix) {
  TextWriter writer;

  // temporary info object that will help write our CSV
  DatasetInfo info;
  info.InitContinuous(matrix.n_rows);

  if (!PASSED(writer.Open(fname))) {
    NONFATAL("Couldn't open '%s' for writing.", fname);
    return SUCCESS_FAIL;
  }

  info.WriteMatrix(matrix, ",\t", writer);
  return writer.Close();
}
