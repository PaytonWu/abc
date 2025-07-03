// Copyright(c) 2025 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_ASYNC_QUEUE_FWD_DECL
#define ABC_INCLUDE_ABC_ASYNC_QUEUE_FWD_DECL

#pragma once

#include <stdexec/execution.hpp>

namespace abc::async
{

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
class Queue;

}

#endif // ABC_INCLUDE_ABC_ASYNC_QUEUE_FWD_DECL
