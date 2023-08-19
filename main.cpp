#include <iostream>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/param/param.h>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <future>
#include <memory>
#include <thread>

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

//std::shared_ptr<System> get_system(Mavsdk& mavsdk)
//{
//	auto prom = std::promise<std::shared_ptr<System>>{};
//	auto fut = prom.get_future();

//	// We wait for new systems to be discovered, once we find one that has an
//	// autopilot, we decide to use it.
//	Mavsdk::NewSystemHandle handle = mavsdk.subscribe_on_new_system([&mavsdk, &prom, &handle]() {
//		auto system = mavsdk.systems().back();

//		if (system->has_autopilot()) {

//			// Unsubscribe again as we only want to find one system.
//			mavsdk.unsubscribe_on_new_system(handle);
//			prom.set_value(system);
//		}
//	});

//	// We usually receive heartbeats at 1Hz, therefore we should find a
//	// system after around 3 seconds max, surely.
//	if (fut.wait_for(seconds(300)) == std::future_status::timeout) {
//		return {};
//	}

//	// Get discovered system now.
//	return fut.get();
//}

std::shared_ptr<System> get_system(Mavsdk& mavsdk)
{
	std::cout << "Waiting to discover system...\n";
	auto prom = std::promise<std::shared_ptr<System>>{};
	auto fut = prom.get_future();

	// We wait for new systems to be discovered, once we find one that has an
	// autopilot, we decide to use it.
	mavsdk.subscribe_on_new_system([&mavsdk, &prom]() {
		auto system = mavsdk.systems().back();

		if (system->has_autopilot()) {
			std::cout << "Discovered autopilot\n";

			// Unsubscribe again as we only want to find one system.
			mavsdk.subscribe_on_new_system(nullptr);
			prom.set_value(system);
		}
	});

	// We usually receive heartbeats at 1Hz, therefore we should find a
	// system after around 3 seconds max, surely.
	if (fut.wait_for(seconds(3)) == std::future_status::timeout) {
		std::cerr << "No autopilot found.\n";
		return {};
	}

	// Get discovered system now.
	return fut.get();
}

void print_health(Telemetry::Health health){
	std::cout << "Got Health:" << '\n';
	std::cout << "Gyro calibration:\t" << (health.is_gyrometer_calibration_ok ? "ok" : "not ok") << '\n';
	std::cout << "Accel calibration:\t" << (health.is_accelerometer_calibration_ok ? "ok" : "not ok") << '\n';
	std::cout << "Mag calibration:\t" << (health.is_magnetometer_calibration_ok ? "ok" : "not ok") << '\n';
	std::cout << "Local position: \t" << (health.is_local_position_ok ? "ok" : "not ok") << '\n';
	std::cout << "Global position:\t" << (health.is_global_position_ok ? "ok" : "not ok") << '\n';
	std::cout << "Home position:  \t" << (health.is_home_position_ok ? "ok" : "not ok") << '\n';
}

using namespace std;

int main()
{
	mavsdk::Mavsdk mavsdk;
	mavsdk::Mavsdk::Configuration configuration(mavsdk::Mavsdk::Configuration::UsageType::GroundStation);
	mavsdk.set_configuration(configuration);
	mavsdk.add_any_connection("udp://:14550");
	auto system = get_system(mavsdk);
	auto param = Param(system);
	auto telemetry = Telemetry(system);
	param.get_all_params();
	telemetry.subscribe_health([](Telemetry::Health health){ print_health(health); });
	while(!telemetry.health_all_ok()){
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}


