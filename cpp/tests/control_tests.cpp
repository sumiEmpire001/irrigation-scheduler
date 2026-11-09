#include <cassert>
#include <limits>
#include "IrrigationScheduler.h"
int main() {
  using namespace irrigation;
  Controller c(Settings(30,55,10,5));
  Report start=c.ingest(0,20,true);
  assert(start.valid && start.pump_on && start.reason==Reason::Watering);
  assert(c.ingest(4,40,true).pump_on);
  Report stop=c.ingest(10,40,true);
  assert(stop.valid && !stop.pump_on && stop.reason==Reason::MaxRuntime);
  assert(c.ingest(11,10,true).reason==Reason::Cooldown);
  assert(c.ingest(15,10,true).pump_on);
  assert(c.ingest(16,56,true).reason==Reason::TargetReached);
  assert(!c.ingest(17,10,false).pump_on);
  Controller invalid;
  assert(!invalid.ingest(0,-1,true).valid);
  assert(invalid.ingest(1,20,true).pump_on);
  Report nan=invalid.ingest(2,std::numeric_limits<double>::quiet_NaN(),true);
  assert(!nan.valid && !nan.pump_on);
  assert(!invalid.ingest(1,20,true).valid);
  Controller bad(Settings(60,55));
  assert(!bad.ingest(0,20,true).valid);
}
