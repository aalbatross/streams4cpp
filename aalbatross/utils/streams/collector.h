#ifndef INCLUDED_STREAMS4CPP_COLLECTOR_H_
#define INCLUDED_STREAMS4CPP_COLLECTOR_H_
#include <functional>

namespace aalbatross::utils::streams {

template<typename Supplier, typename Accumulator, typename Finisher>
class Collector {
 private:
  Supplier dSupplier;
  Accumulator dAccumulator;
  Finisher dFinisher;

 public:
  ~Collector() = default;

  explicit Collector(Supplier &&supplier, Accumulator &&accumulator, Finisher &&finisher) : dSupplier(supplier), dAccumulator(accumulator), dFinisher(finisher) {}

  Supplier supplier() const { return dSupplier; }
  Finisher finisher() const { return dFinisher; };
  Accumulator accumulator() const { return dAccumulator; };

  template<typename T>
  auto apply(std::vector<T> &input) const {
    auto container = dSupplier();
    for (T &item : input) {
      dAccumulator(container, item);
    }
    return dFinisher(container);
  }
};
}// namespace aalbatross::utils::streams
#endif