#include "Transformer.h"

namespace Raft {
	void Transformer::Transform(RaftServerRole _fromRole, RaftServerRole _toRole, Term _term) {
    transform(_fromRole, _toRole, _term);
  }
}
