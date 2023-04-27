#ifndef INCLUDED_STREAMS4CPP_COLLECTOR_H_
#define INCLUDED_STREAMS4CPP_COLLECTOR_H_
#include <functional>

namespace aalbatross::utils::streams {
/**
 * \class Collector
 * \brief A mutable reduction operation that accumulates input elements into a mutable result container, optionally transforming the accumulated result into a final representation after all input elements have been processed.
 * A Collector is specified by three functions that work together to accumulate entries into a mutable result container, and optionally perform a final transform on the result. They are:
 *
 * creation of a new result container (supplier())
 * incorporating a new data element into a result container (accumulator())
 * performing an optional final transform on the container (finisher())
 *
 *
 * @tparam Supplier Function for creation of a new result container
 * @tparam Accumulator Function for accumulating a new data element into a result container
 * @tparam Finisher Function performing an optional final transform on the container
 */
template<typename Supplier, typename Accumulator, typename Finisher>
class Collector {
 private:
  Supplier dSupplier_;
  Accumulator dAccumulator_;
  Finisher dFinisher_;

 public:
  ~Collector() = default;

  explicit Collector(Supplier &&supplier, Accumulator &&accumulator, Finisher &&finisher) : dSupplier_(supplier), dAccumulator_(accumulator), dFinisher_(finisher) {}

  Collector(Collector &) = default;
  Collector(Collector &&) noexcept = default;

  Collector &operator=(const Collector &) = default;
  Collector &operator=(Collector &&) noexcept = default;

  Supplier supplier() const { return dSupplier_; }
  Finisher finisher() const { return dFinisher_; };
  Accumulator accumulator() const { return dAccumulator_; };

  template<typename T>
  auto apply(std::vector<T> &input) const {
    auto container = dSupplier_();
    for (T &item : input) {
      dAccumulator_(container, item);
    }
    return dFinisher_(container);
  }
};
}// namespace aalbatross::utils::streams
#endif