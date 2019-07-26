#ifndef RAFT_TRANSFORMER_H
#define RAFT_TRANSFORMER_H

#include <functional>
#include "defines.h"
#include "Transformer.h"

namespace Raft {
  class Transformer {
  private:
    std::function<void(RaftServerRole, RaftServerRole, Term)> transform;
  public:
    template<class T>
    void bindTransform(T &&func) {
      transform = std::forward<T>(func);
    }
    void Transform(RaftServerRole _fromRole, RaftServerRole _toRole, Term _term);
  };
}

#endif
