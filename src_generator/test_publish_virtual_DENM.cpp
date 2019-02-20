#include "SafeStrip.hpp"
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

int
main() {

  MQTT_SafeStrip_publisher  mqtt_publisher("publisher");

  cout << "Connect ... \n";
  mqtt_publisher.connect( "127.0.0.1", 1883, 60 );

  cout << "Starting ... \n";
  for ( int j = 0; j < 1000; j++) {

    cout << "j = " << j << '\n';

    // trigger mosquitto callbacks
    mqtt_publisher.loop(1, 1);

    // Sending
    DENM_message denm;
    
    denm.v2x_header_msgID                                = j;
    denm.v2x_header_originationStationID                 = 100;
    denm.v2x_denm_sequenceNumber                         = j;
    denm.v2x_denm_detectionTime                          = 103921391;
    denm.v2x_denm_eventPosition_latitude                 = 49819426;
    denm.v2x_denm_eventPosition_longitude                = 18276841;
    denm.v2x_denm_ConfidenceEllipse_semiMajor            = 2;
    denm.v2x_denm_ConfidenceEllipse_semiMinor            = 1;
    denm.v2x_denm_ConfidenceEllipse_semiMajorOrientation = 21;
    denm.v2x_denm_eventPosition_altitude                 = 340;
    denm.v2x_denm_relevanceDistance                      = 20;
    denm.v2x_denm_relevanceTrafficDirection              = 1;
    denm.v2x_denm_validityDuration                       = 10;
    denm.v2x_denm_stationType                            = 15;
    denm.v2x_denm_eventType_CauseCode                    = 9;
    denm.v2x_denm_eventType_SubCauseCode                 = 1;
    denm.v2x_denm_frictionCoefficient_value              = 222;
    denm.v2x_denm_frictionCoefficient_confidence         = 50;

    mqtt_publisher.publish( denm );
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  cout << "> Clean exit\n";
  return 0;
}