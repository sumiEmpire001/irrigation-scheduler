#ifndef LORD_SUMI_IRRIGATION_SCHEDULER_H
#define LORD_SUMI_IRRIGATION_SCHEDULER_H
#include <math.h>
namespace irrigation {
enum class Reason { Invalid, Idle, TankEmpty, Cooldown, Watering, MaxRuntime, TargetReached };
struct Settings {
  double start_below, stop_above, max_run_seconds, cooldown_seconds;
  Settings(double start=30.0,double stop=55.0,double max_run=600.0,double cooldown=300.0)
    :start_below(start),stop_above(stop),max_run_seconds(max_run),cooldown_seconds(cooldown){}
};
struct Report {
  bool valid, pump_on;
  Reason reason;
  Report(bool v,bool p,Reason r):valid(v),pump_on(p),reason(r){}
};
inline bool valid_settings(const Settings& s) {
  return isfinite(s.start_below) && isfinite(s.stop_above) &&
    isfinite(s.max_run_seconds) && isfinite(s.cooldown_seconds) &&
    s.start_below>=0 && s.start_below<s.stop_above && s.stop_above<=100 &&
    s.max_run_seconds>0 && s.cooldown_seconds>=0;
}
class Controller {
 public:
  explicit Controller(Settings settings=Settings()): settings_(settings),running_(false),
    have_time_(false),have_stop_(false),last_timestamp_(0),running_since_(0),last_stop_(0){}
  Report ingest(double timestamp_seconds,double moisture_percent,bool tank_available) {
    if (!valid_settings(settings_) || !isfinite(timestamp_seconds) || timestamp_seconds<0 ||
        (have_time_ && timestamp_seconds<=last_timestamp_) || !isfinite(moisture_percent) ||
        moisture_percent<0 || moisture_percent>100) {
      running_=false;
      if (have_time_) { have_stop_=true; last_stop_=last_timestamp_; }
      return Report(false,false,Reason::Invalid);
    }
    Reason reason=Reason::Idle;
    if (running_) {
      if (!tank_available) reason=Reason::TankEmpty;
      else if (timestamp_seconds-running_since_>=settings_.max_run_seconds) reason=Reason::MaxRuntime;
      else if (moisture_percent>=settings_.stop_above) reason=Reason::TargetReached;
      else reason=Reason::Watering;
      if (reason!=Reason::Watering) {
        running_=false;
        have_stop_=true;
        last_stop_=timestamp_seconds;
      }
    } else if (!tank_available) reason=Reason::TankEmpty;
    else if (have_stop_ && timestamp_seconds-last_stop_<settings_.cooldown_seconds)
      reason=Reason::Cooldown;
    else if (moisture_percent<settings_.start_below) {
      running_=true;
      running_since_=timestamp_seconds;
      reason=Reason::Watering;
    }
    have_time_=true;
    last_timestamp_=timestamp_seconds;
    return Report(true,running_,reason);
  }
 private:
  Settings settings_;
  bool running_,have_time_,have_stop_;
  double last_timestamp_,running_since_,last_stop_;
};
} // namespace irrigation
#endif
