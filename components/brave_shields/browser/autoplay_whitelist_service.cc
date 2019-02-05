/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_shields/browser/autoplay_whitelist_service.h"

#include <algorithm>
#include <utility>

#include "base/base_paths.h"
#include "base/bind.h"
#include "base/logging.h"
#include "base/macros.h"
#include "base/memory/ptr_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/threading/thread_restrictions.h"
#include "brave/browser/brave_browser_process_impl.h"
#include "brave/components/brave_shields/browser/ad_block_service.h"
#include "brave/components/brave_shields/browser/autoplay_whitelist_parser.h"
#include "brave/components/brave_shields/browser/local_data_files_service.h"
#include "brave/components/brave_shields/browser/dat_file_util.h"

#include "base/logging.h"

#define DAT_FILE "AutoplayWhitelist.dat"
#define DAT_FILE_VERSION "1"

namespace brave_shields {

AutoplayWhitelistService::AutoplayWhitelistService()
    : autoplay_whitelist_client_(new AutoplayWhitelistParser()),
      weak_factory_(this) {
    // TODO(mpilgrim) begin temporary debug code
    autoplay_whitelist_client_.reset(new AutoplayWhitelistParser());
    LOG(ERROR) << "entering AutoplayWhitelistService\n";
    std::string kDebugWhitelist[] = {
        "example.com",
        "youtube.com",
        "khanacademy.org",
        "twitch.tv",
        "vimeo.com",
        "udemy.com",
        "duolingo.com",
        "giphy.com",
        "imgur.com",
        "netflix.com",
        "hulu.com",
        "primevideo.com",
        "dailymotion.com",
        "tv.com",
        "viewster.com",
        "metacafe.com",
        "d.tube",
        "spotify.com",
        "lynda.com",
        "soundcloud.com",
        "pandora.com",
        "periscope.tv",
        "pscp.tv",
        "hangouts.google.com",
        "meet.google.com",
    };
    for (const auto& host : kDebugWhitelist) {
        autoplay_whitelist_client_->addHost(host.c_str());
    }
    // TODO(mpilgrim) end temporary debug code
    // TODO(mpilgrim) remove above code when we're ready to actually read from
    // the DAT file

  DETACH_FROM_SEQUENCE(sequence_checker_);
}

AutoplayWhitelistService::~AutoplayWhitelistService() {
  autoplay_whitelist_client_.reset();
}

bool AutoplayWhitelistService::ShouldAllowAutoplay(const GURL& url) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  return autoplay_whitelist_client_->matchesHost(url.host().c_str());
}

void AutoplayWhitelistService::OnDATFileDataReady() {
  if (buffer_.empty()) {
    LOG(ERROR) << "Could not obtain autoplay whitelist data";
    return;
  }
  autoplay_whitelist_client_.reset(new AutoplayWhitelistParser());
  if (!autoplay_whitelist_client_->deserialize((char*)&buffer_.front())) {
    autoplay_whitelist_client_.reset();
    LOG(ERROR) << "Failed to deserialize autoplay whitelist data";
    return;
  }
}

void AutoplayWhitelistService::OnComponentReady(
    const std::string& component_id,
    const base::FilePath& install_dir,
    const std::string& manifest) {

  base::FilePath dat_file_path =
      install_dir.AppendASCII(DAT_FILE_VERSION).AppendASCII(DAT_FILE);

  GetTaskRunner()->PostTaskAndReply(
      FROM_HERE,
      base::Bind(&GetDATFileData, dat_file_path, &buffer_),
      base::Bind(&AutoplayWhitelistService::OnDATFileDataReady,
                 weak_factory_.GetWeakPtr()));
}

scoped_refptr<base::SequencedTaskRunner> AutoplayWhitelistService::GetTaskRunner() {
  // We share the same task runner as ad-block code
  return g_brave_browser_process->ad_block_service()->GetTaskRunner();
}

///////////////////////////////////////////////////////////////////////////////

// The tracking protection factory. Using the Brave Shields as a singleton
// is the job of the browser process.
std::unique_ptr<AutoplayWhitelistService> AutoplayWhitelistServiceFactory() {
  std::unique_ptr<AutoplayWhitelistService> service = std::make_unique<AutoplayWhitelistService>();
  g_brave_browser_process->local_data_files_service()->AddObserver(service.get());
  return service;
}

}  // namespace brave_shields
