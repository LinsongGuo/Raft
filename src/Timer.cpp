#include <boost/chrono/chrono.hpp>
#include "defines.h"

namespace Raft {
  Timer getTime() {
    return boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::system_clock::now().time_since_epoch()).count();
  }
}
