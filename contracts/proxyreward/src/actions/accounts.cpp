#include <accounts.hpp>
#include <reward.hpp>

namespace edenproxy {
  void reward::notify_transfer( eosio::name         from,
                                eosio::name         to,
                                const eosio::asset &quantity,
                                std::string         memo ) {
    // token symbol is already validated in the funds transfer contract, if the
    // notified transacion gets here, it's a valid transfer and comes from
    // a trusted contract only which is `default_funding_contract`

    accounts{ get_self() }.add_balance( quantity );
  }
} // namespace edenproxy