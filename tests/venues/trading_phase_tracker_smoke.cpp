#include "marketdata/venues/trading_phase_tracker.hpp"

#include <cassert>

int main() {
  md::venues::TradingPhaseTracker tracker;
  trading::events::Status status{};
  status.status_type = trading::core::StatusType::TradingPhase;
  status.trading_phase = trading::core::TradingPhase::Continuous;
  status.header.exchange_seq = 100;
  assert(tracker.apply(status) == md::venues::PhaseApplyStatus::Applied);
  assert(tracker.phase() == trading::core::TradingPhase::Continuous);
  assert(tracker.apply(status) == md::venues::PhaseApplyStatus::Duplicate);
  status.header.exchange_seq = 99;
  assert(tracker.apply(status) == md::venues::PhaseApplyStatus::Regression);
  return 0;
}
