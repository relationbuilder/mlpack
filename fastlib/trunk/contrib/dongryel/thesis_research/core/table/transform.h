/** @file transform.h
 *  @brief Transforms the table such that it is standardized or scaled to
 *         a unit hypercube.
 *
 *  @author Dongryeol Lee (dongryel@cc.gatech.edu)
 */

#ifndef CORE_TABLE_TRANSFORM_H
#define CORE_TABLE_TRANSFORM_H

#include "fastlib/core/table.h"

class Standardize {
  public:

    static void Transform(core::table::Table &table_in) {

      // Means and standard deviations of each dimension.
      std::vector<double> means(table_in.n_attributes(), 0.0);
      std::vector<double> standard_deviations(table_in.n_attributes(), 0.0);

      // Loop through each point in the table and compute
      // means/standard_deviations for each dimension.
      for(int i = 0; i < table_in.n_entries(); i++) {
        arma::vec point;
        table_in.get(i, &point);
        for(int d = 0; d < table_in.n_attributes(); d++) {
          double delta = point[d] - means[d];
          means[d] = means[d] + delta / static_cast<double>(i + 1);
          standard_deviations[d] = standard_deviations[d] + delta *
                                   (point[d] - means[d]);
        }
      }
      for(int d = 0; d < table_in.n_attributes(); d++) {
        standard_deviations[d] = sqrt(
                                   standard_deviations[d] /
                                   static_cast<double>(table_in.n_entries() - 1));
      }
      for(int i = 0; i < table_in.n_entries(); i++) {
        arma::vec point;
        for(int d = 0; d < table_in.n_attributes(); d++) {
          point[d] = (point[d] - means[d]) / standard_deviations[d] ;
        }
      }
    }
};

#endif
