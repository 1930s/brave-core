/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/app/brave_command_line_helper.h"

#include <sstream>
#include <vector>

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/strings/string_split.h"

BraveCommandLineHelper::BraveCommandLineHelper(base::CommandLine* command_line)
    : command_line_(*command_line) {
  Parse();
}

void BraveCommandLineHelper::ParseCSV(
    const std::string& value,
    std::unordered_set<std::string>* dest) const {
  DCHECK(dest);
  if (value.empty())
    return;
  std::vector<std::string> values = base::SplitString(
      value, ",", base::TRIM_WHITESPACE, base::SPLIT_WANT_NONEMPTY);
  dest->insert(std::make_move_iterator(values.begin()),
               std::make_move_iterator(values.end()));
}

void BraveCommandLineHelper::Parse() {
  ParseCSV(command_line_.GetSwitchValueASCII(switches::kEnableFeatures),
           &enabled_features_);
  ParseCSV(command_line_.GetSwitchValueASCII(switches::kDisableFeatures),
           &disabled_features_);
}

void BraveCommandLineHelper::AppendSwitch(const char* switch_key) {
  if (!command_line_.HasSwitch(switch_key))
    command_line_.AppendSwitch(switch_key);
}

void BraveCommandLineHelper::AppendEnabledFeatures(
    const std::unordered_set<const char*>& enabled) {
  // Combine user and programmatically enabled features.
  for (auto it = enabled.cbegin(); it != enabled.cend(); ++it)
      enabled_features_.insert(*it);
  AppendCSV(switches::kEnableFeatures, enabled_features_);
}

void BraveCommandLineHelper::AppendDisabledFeatures(
    const std::unordered_set<const char*>& disabled) {
  // Combine user and programmatically disabled features.
  for (auto it = disabled.cbegin(); it != disabled.cend(); ++it)
    disabled_features_.insert(*it);
  AppendCSV(switches::kDisableFeatures, disabled_features_);
}

void BraveCommandLineHelper::AppendCSV(
    const char* switch_key,
    const std::unordered_set<std::string>& values) {
  std::stringstream ss;
  for (auto it = values.cbegin(); it != values.cend(); ++it) {
    if (it != values.cbegin())
      ss << ",";
    ss << *it;
  }
  command_line_.AppendSwitchASCII(switch_key, ss.str());
}
