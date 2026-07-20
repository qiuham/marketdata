#include "marketdata/providers/cn/stock/csmar/mapper.hpp"

#include <cassert>

namespace cs = md::providers::csmar;
namespace tc = trading::core;
namespace te = trading::events;

int main() {
  cs::CsmarMapper mapper({.venue_id = 1,
                          .instrument_id = 600730,
                          .trading_day = 20260417,
                          .market = cs::Market::Shanghai});
  assert(mapper.establish_baseline(2, 109));

  cs::OrderMapOutput output{};
  auto result = mapper.map(
      cs::OrderRow{.unix_time_ms = 1'776'388'440'620ULL,
                   .record_id = 110,
                   .set_id = 2,
                   .order_type = 'S',
                   .trading_phase = tc::TradingPhase::OpeningCall},
      output);
  assert(result.publishable());
  assert(output.event_kind == tc::EventKind::TradingPhaseUpdate);
  assert(output.phase.header.exchange_ts_ns == 33'240'620'000'000ULL);

  result = mapper.map(
      cs::OrderRow{.unix_time_ms = 1'776'388'501'440ULL,
                   .record_id = 74153,
                   .set_id = 2,
                   .order_id = 3674,
                   .price = 91'900,
                   .quantity = 1'000,
                   .side = tc::Side::Sell,
                   .order_type = 'A'},
      output);
  assert(result.publishable());
  assert(output.order.action == tc::OrderAction::Add);
  assert(output.order.order_id == 3674);
  assert(output.order.header.exchange_seq == 74153);

  result = mapper.map(
      cs::OrderRow{.unix_time_ms = 1'776'388'513'350ULL,
                   .record_id = 74320,
                   .set_id = 2,
                   .order_id = 118500,
                   .price = 90'500,
                   .quantity = 500,
                   .side = tc::Side::Sell,
                   .order_type = 'D'},
      output);
  assert(result.publishable());
  assert(output.order.action == tc::OrderAction::Cancel);
  assert(output.order.order_id == 118500);
  assert(output.order.quantity == 500);

  te::BookTrade trade{};
  result = mapper.map(
      cs::TradeRow{.unix_time_ms = 1'776'389'100'250ULL,
                   .record_id = 74584,
                   .channel = 2,
                   .trade_id = 74584,
                   .buy_order_id = 281305,
                   .sell_order_id = 337667,
                   .price = 87'500,
                   .quantity = 100,
                   .aggressor_side = tc::AggressorSide::Unknown},
      trade);
  assert(result.publishable());
  assert(trade.buy_order_id == 281305);
  assert(trade.sell_order_id == 337667);
  assert(trade.aggressor_side == tc::AggressorSide::Unknown);

  result = mapper.map(
      cs::TradeRow{.unix_time_ms = 1'776'389'400'250ULL,
                   .record_id = 74585,
                   .channel = 2,
                   .trade_id = 74585,
                   .buy_order_id = 500001,
                   .sell_order_id = 500002,
                   .price = 87'500,
                   .quantity = 100,
                   .aggressor_side = tc::AggressorSide::Buy},
      trade);
  assert(result.publishable());
  assert(trade.buy_order_id == 0);
  assert(trade.sell_order_id == 0);
  assert(trade.resting_order_id == 500002);
  assert(trade.aggressor_side == tc::AggressorSide::Buy);

  result = mapper.map(
      cs::TradeRow{.record_id = 74585, .channel = 2, .trade_id = 74585},
      trade);
  assert(result.continuity.event == md::mappers::ContinuityEvent::Duplicate);
  assert(mapper.sequence_stats().gaps == 0);
  return 0;
}
