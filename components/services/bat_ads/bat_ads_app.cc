/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/services/bat_ads/bat_ads_app.h"

#include "brave/components/services/bat_ads/bat_ads_service_impl.h"
#include "mojo/public/cpp/bindings/strong_binding.h"

namespace bat_ads {

BatAdsApp::BatAdsApp(service_manager::mojom::ServiceRequest request) :
    service_binding_(this, std::move(request)),
    service_keepalive_(&service_binding_, base::TimeDelta()) {
}

BatAdsApp::~BatAdsApp() {}

void BatAdsApp::OnStart() {}

void BatAdsApp::OnBindInterface(
    const service_manager::BindSourceInfo& source_info,
    const std::string& interface_name,
    mojo::ScopedMessagePipeHandle interface_pipe) {
  registry_.BindInterface(interface_name, std::move(interface_pipe));
}

}  // namespace bat_ads
