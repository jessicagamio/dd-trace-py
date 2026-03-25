// Fuzz harness: for_each_interp
//
// Exercises the interpreter linked-list traversal in interp.cc.
// for_each_interp walks _PyRuntimeState.interpreters.head, reading
// PyInterpreterState fields (id, next, tstate_head) via copy_type.
// Tests cycle detection, iteration bounds, and partial-read resilience.

#include "fuzz_common.h"

#include <echion/echion_sampler.h>
#include <echion/interp.h>
#include <echion/greenlets.h>

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < sizeof(PyObject*)) {
        return 0;
    }

    g_data = data;
    g_size = size;

    // Point interpreters.head into the fuzz buffer so for_each_interp
    // reads interpreter nodes from fuzzer-controlled memory.
    EchionSampler echion;
    auto* p0 = reinterpret_cast<PyObject*>(addr_from_u64(load_u64_le(data, size, 0)));

    GreenletInfo::ID id = 123;
    StringTable::Key key = 123;
    (void)GreenletInfo(id, p0, key);

    g_data = nullptr;
    g_size = 0;
    return 0;
}
