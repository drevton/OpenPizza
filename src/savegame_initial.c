// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#include "initial_data.h"
#include "savegame.h"
void pt_initial_data_apply(void) {
  pt_native_initial_data_apply();
  pt_saved_global_values_seed();
}
