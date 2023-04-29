#include <snowplow/configuration/emitter_configuration.hpp>
#include <snowplow/detail/utils/utils.hpp>
#include <snowplow/emitter/emitter.hpp>
#include <snowplow/snowplow.hpp>

#include <cstdlib>

int main() {
  auto uuid = snowplow::Utils::get_uuid4();
  auto payload = snowplow::Utils::deserialize_json_str("");
  snowplow::NetworkConfiguration network_config("");
  snowplow::Emitter emitter(network_config, snowplow::EmitterConfiguration(""));

  return EXIT_SUCCESS;
}
