/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_REWARDS_LOGGING_H_
#define BAT_REWARDS_LOGGING_H_

#define BLOG(client, severity) \
  client->Log(__FILE__, __LINE__, severity)->stream()

#define BVLOG(client, severity) \
  client->LogVerbose(__FILE__, __LINE__, severity)->stream()

#endif  // BAT_REWARDS_LOGGING_H_

#if defined(NDEBUG)
#define DCHECK(x) (void)(x)
#else
#include "assert.h"
#define DCHECK(x) assert(x)
#endif
