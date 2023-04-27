#ifndef INCLUDED_STREAMS4CPP_COLLECTOR_H_
#define INCLUDED_STREAMS4CPP_COLLECTOR_H_
#include <functional>

namespace aalbatross::utils::streams {

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