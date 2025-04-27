// Copyright (C) hedge-dev 2025, Licensed via GPL3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html).
#pragma once
#include "recompiled/ppc/ppc_context.h"

inline thread_local PPCContext* GLOBAL_CPU_CONTEXT;

inline PPCContext* GetPPCContext()
{
    return GLOBAL_CPU_CONTEXT;
}

inline void SetPPCContext(PPCContext& ctx)
{
    GLOBAL_CPU_CONTEXT = &ctx;
}
