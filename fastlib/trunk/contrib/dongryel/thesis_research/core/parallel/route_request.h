/** @file route_request.h
 *
 *  @author Dongryeol Lee (dongryel@cc.gatech.edu)
 */

#ifndef CORE_PARALLEL_ROUTE_REQUEST_H
#define CORE_PARALLEL_ROUTE_REQUEST_H

#include <boost/mpi/communicator.hpp>
#include <boost/serialization/serialization.hpp>
#include <vector>
#include "core/table/sub_table.h"

namespace core {
namespace parallel {

template<typename ObjectType>
class RouteRequest {
  public:

    typedef core::parallel::RouteRequest<ObjectType> RouteRequestType;

  private:

    /** @brief The list of MPI ranks for which the subtable should be
     *         forwarded to.
     */
    std::vector<int> destinations_;

    /** @brief An internal variable to keep track of the number of
     *         destinations passed to the next routing destination.
     */
    int num_routed_;

    /** @brief The next destination to which the next routing message
     *         should be forwarded.
     */
    int next_destination_;

    /** @brief The MPI rank of the calling process.
     */
    int rank_;

    /** @brief The object to be routed.
     */
    ObjectType object_;

    int stage_;

  private:

    void ComputeNextDestination_() {
      next_destination_ = rank_ ^(1 << stage_);
    }

  public:

    int stage() const {
      return stage_;
    }

    int rank() const {
      return rank_;
    }

    int next_destination(boost::mpi::communicator &comm) const {
      RouteRequestType *this_modifiable =
        const_cast < RouteRequestType * >(this);
      this_modifiable->rank_ = comm.rank();
      this_modifiable->ComputeNextDestination_();
      return next_destination_;
    }

    /** @brief Returns the number of destinations passed to the next
     *         routing destination after an asynchronous send is
     *         issued.
     */
    int num_routed() const {
      return num_routed_;
    }

    int num_destinations() const {
      return destinations_.size();
    }

    bool remove_from_destination_list(int destination_in) {
      typename std::vector<int>::iterator it =
        std::find(
          destinations_.begin(), destinations_.end(), destination_in);
      if(it != destinations_.end()) {
        (*it) = destinations_.back();
        destinations_.pop_back();
        return true;
      }
      else {
        return false;
      }
    }

    RouteRequest() {
      num_routed_ = 0;
      next_destination_ = 0;
      rank_ = 0;
      stage_ = 0;
    }

    ObjectType &object() {
      return object_;
    }

    const ObjectType &object() const {
      return object_;
    }

    const std::vector<int> &destinations() const {
      return destinations_;
    }

    void add_destination(int new_dest_in) {
      destinations_.push_back(new_dest_in);
    }

    void add_destinations(boost::mpi::communicator &comm) {
      for(int i = 0; i < comm.size(); i++) {
        if(i != comm.rank()) {
          destinations_.push_back(i);
        }
      }
    }

    template<class Archive>
    void save(Archive &ar, const unsigned int version) const {
      RouteRequestType *this_modifiable =
        const_cast< RouteRequestType * >(this);

      // Count how many messages were routed.
      std::vector<int> filtered;

      int mask_next_destination = next_destination_ & (1 << stage_);
      for(int i = 0; i < static_cast<int>(destinations_.size()); i++) {
        int masked_destination = (destinations_[i]) & (1 << stage_) ;
        if((mask_next_destination ^ masked_destination) == 0) {
          filtered.push_back(destinations_[i]);
          this_modifiable->destinations_[i] = destinations_.back();
          this_modifiable->destinations_.pop_back();
          i--;
        }
      }
      this_modifiable->num_routed_ = filtered.size();
      ar & num_routed_;
      for(int i = 0; i < num_routed_; i++) {
        ar & filtered[i];
      }

      // Increment the stage.
      this_modifiable->stage_++;
      ar & stage_;

      // Save the object, only if the number of routed messages is
      // at least 1.
      if(filtered.size() > 0) {
        ar & object_;
      }
    }

    template<class Archive>
    void load(Archive &ar, const unsigned int version) {

      // Load the size.
      int size;
      ar & size;
      destinations_.resize(size);
      for(int i = 0; i < size; i++) {
        ar & destinations_[i];
      }

      // Load the stage.
      ar & stage_;

      // Load the object, if the message is not empty.
      if(size > 0) {
        ar & object_;
      }
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()

    /** @brief The assignment operator.
     */
    void operator=(const RouteRequestType &route_request_in) {
      destinations_ = route_request_in.destinations();
      num_routed_ = route_request_in.num_routed();
      rank_ = route_request_in.rank();
      object_ = route_request_in.object();
      stage_ = route_request_in.stage();
    }

    /** @brief The copy constructor.
     */
    RouteRequest(const RouteRequestType &route_request_in) {
      this->operator=(route_request_in);
    }

    /** @brief Initializes the routing message for the first time.
     */
    void Init(boost::mpi::communicator &comm) {
      rank_ = comm.rank();
      stage_ = 0;
    }

    /** @brief Copies from another routing message.
     */
    void Init(
      boost::mpi::communicator &comm,
      const RouteRequestType &source_in) {
      destinations_ = source_in.destinations();
      next_destination_ = source_in.next_destination(comm);
      num_routed_ = 0;
      rank_ = comm.rank();
      object_ = source_in.object();
      stage_ = source_in.stage();
    }
};
}
}

#endif
