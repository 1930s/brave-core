/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/app/brave_command_line_helper.h"

#include <set>
#include <string>
#include <vector>

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/strings/string_split.h"
#include "base/strings/utf_string_conversions.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace {

int CountA(const base::CommandLine::StringVector& sv) {
  int count = 0;
  for (const auto& argv : sv) {
    std::string value;
#if defined(OS_WIN)
    value = base::UTF16ToASCII(argv);
#else
    value = argv;
#endif
    if (value == "--a")
      count++;
  }
  return count;
}

std::set<std::string> FeaturesToSet(const std::string& features) {
  std::set<std::string> result;
  std::vector<std::string> values = base::SplitString(
      features, ",", base::TRIM_WHITESPACE, base::SPLIT_WANT_NONEMPTY);
  result.insert(std::make_move_iterator(values.begin()),
                std::make_move_iterator(values.end()));
  return result;
}

}  // namespace

TEST(BraveCommandLineHelperUnitTest, TestAppendSwitch) {
  base::CommandLine& command_line = *base::CommandLine::ForCurrentProcess();
  BraveCommandLineHelper helper(&command_line);
  // Test that append switch works.
  helper.AppendSwitch("a");
  ASSERT_TRUE(command_line.HasSwitch("a"));
  ASSERT_EQ(1, CountA(command_line.argv()));

  // Chromium's AppendSwtich always adds to argv even if the switch
  // has already been added.
  command_line.AppendSwitch("a");
  ASSERT_EQ(2, CountA(command_line.argv()));

  // Test that multiple attempts to append the same switch doesn't append
  // more than once.
  helper.AppendSwitch("a");
  ASSERT_EQ(2, CountA(command_line.argv()));
}

TEST(BraveCommandLineHelperUnitTest, TestFeatures) {
  base::CommandLine& command_line = *base::CommandLine::ForCurrentProcess();
  BraveCommandLineHelper helper(&command_line);
  // Test enabled features: none on command line.
  helper.AppendEnabledFeatures({"a"});
  ASSERT_EQ(command_line.GetSwitchValueASCII(switches::kEnableFeatures), "a");
  // Test enabled features: merge with existing.
  helper.AppendEnabledFeatures({"a", "b", "c"});
  ASSERT_EQ(FeaturesToSet(
                command_line.GetSwitchValueASCII(switches::kEnableFeatures)),
            FeaturesToSet("a,b,c"));
  // Test disabled features: none on command line.
  helper.AppendDisabledFeatures({"d", "e"});
  ASSERT_EQ(FeaturesToSet(
                command_line.GetSwitchValueASCII(switches::kDisableFeatures)),
            FeaturesToSet("d,e"));
  // Test disabled features: merge with existing.
  helper.AppendDisabledFeatures({"d", "e", "f", "g"});
  ASSERT_EQ(FeaturesToSet(
                command_line.GetSwitchValueASCII(switches::kDisableFeatures)),
            FeaturesToSet("d,e,f,g"));
  // Append non-intersecting to enabled and disabled.
  helper.AppendEnabledFeatures({"v", "w"});
  helper.AppendDisabledFeatures({"v", "w"});
  ASSERT_EQ(FeaturesToSet(
                command_line.GetSwitchValueASCII(switches::kEnableFeatures)),
            FeaturesToSet("a,b,c,v,w"));
  ASSERT_EQ(FeaturesToSet(
                command_line.GetSwitchValueASCII(switches::kDisableFeatures)),
            FeaturesToSet("d,e,f,g,v,w"));
}
