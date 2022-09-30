// This file is part of the AliceVision project.
// Copyright (c) 2022 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "ParallelFor.hpp"
#include "ParallelismBackendOpenMP.hpp"

#if ALICEVISION_HAVE_TBB
#include "ParallelismBackendTBB.hpp"
#endif

namespace aliceVision {
namespace system {

IParallelLoopManager::~IParallelLoopManager() = default;

ParallelLoopManagerSingleThread::ParallelLoopManagerSingleThread() = default;
ParallelLoopManagerSingleThread::~ParallelLoopManagerSingleThread() = default;

void ParallelLoopManagerSingleThread::submit(const std::function<void()>& callback)
{
    callback();
}

IParallelismBackend::~IParallelismBackend() = default;

bool s_getCurrentParallelismBackendCalled = false;
IParallelismBackend* s_parallelismBackendOverride = nullptr;

static IParallelismBackend& getDefaultParallelismBackend()
{
#if ALICEVISION_HAVE_TBB
    static tbb::task_arena taskArena;
    static ParallelismBackendTBB backend{taskArena};
    return backend;
#else
    static ParallelismBackendOpenMP backend;
    return backend;
#endif
}

IParallelismBackend& getCurrentParallelismBackend()
{
    s_getCurrentParallelismBackendCalled = true;
    if (s_parallelismBackendOverride != nullptr)
    {
        return *s_parallelismBackendOverride;
    }
    return getDefaultParallelismBackend();
}

void setCurrentParallelistBackend(IParallelismBackend& backend)
{
    if (s_getCurrentParallelismBackendCalled)
    {
        throw std::runtime_error("setCurrentParallelistBackend must be called before any other "
                                 "parallelism operations");
    }
    s_parallelismBackendOverride = &backend;
}

} // namespace system
} // namespace aliceVision
