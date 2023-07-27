// Copyright 2023 Kushview, LLC <info@kushview.net>
// SPDX-License-Identifier: GPL3-or-later

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef union _kv_packed_t
{
    int64_t packed;
    uint8_t data[4];
} kv_packed_t;

#ifdef __cplusplus
}
#endif
