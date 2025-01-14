// cltester definitions
#include <tester-base.hpp>

#define CATCH_CONFIG_RUNNER

#ifdef ENABLE_TESTING_BYPASS
TEST_CASE( "Init Smart Contract" ) {
  tester t;

  t.skip_to( "2023-01-01T07:00:00.000" );

  expect( t.alice.trace< edenproxy::actions::init >(),
          "Missing required authority" );

  t.eosproxyrwds.act< edenproxy::actions::init >();
  t.chain.start_block();

  expect( t.eosproxyrwds.trace< edenproxy::actions::init >(),
          "Contract is already initialized" );

  CHECK( t.get_account().balance == s2a( "0.0000 EOS" ) );
  CHECK(
      t.get_distribution().distribution_time ==
      eosio::time_point_sec( 1672642800 ) ); // == January 02, 2023 07:00 AM UTC
}

TEST_CASE( "Sign up and remove" ) {
  tester t;

  t.fund_accounts();

  t.eosproxyrwds.act< edenproxy::actions::init >();
  t.alice.act< token::actions::transfer >( "alice"_n,
                                           "eosproxyfund"_n,
                                           s2a( "500.0000 EOS" ),
                                           "donation" );

  // expect( t.alice.trace< edenproxy::actions::signup >( "alice"_n, "alice"_n ),
  //         "Need to delegate the vote to edensmartprx" );
  expect( t.bob.trace< edenproxy::actions::signup >( "alice"_n, "alice"_n ),
          "Missing required authority" );
  expect(
      t.alice.trace< edenproxy::actions::signup >( "alice"_n, "alice.fake"_n ),
      "Account does not exist" );

  t.alice.act< edenproxy::actions::signup >( "alice"_n, "alice"_n );
  t.chain.start_block();

  expect( t.alice.trace< edenproxy::actions::signup >( "alice"_n, "alice"_n ),
          "Voter already exist" );

  std::map< eosio::name, std::vector< uint64_t > > expected{
      { "alice"_n, { 0, 0, 0 } } };

  CHECK( t.get_voters() == expected );

  t.ahab.act< edenproxy::actions::signup >( "ahab"_n, "ahab"_n );
  t.bertie.act< edenproxy::actions::signup >( "bertie"_n, "bertie"_n );
  t.bob.act< edenproxy::actions::signup >( "bob"_n, "bob"_n );
  t.egeon.act< edenproxy::actions::signup >( "egeon"_n, "egeon"_n );
  t.pip.act< edenproxy::actions::signup >( "pip"_n, "pip"_n );

  expected.insert( { "ahab"_n, { 0, 0, 0 } } );
  expected.insert( { "alice"_n, { 0, 0, 0 } } );
  expected.insert( { "bertie"_n, { 0, 0, 0 } } );
  expected.insert( { "bob"_n, { 0, 0, 0 } } );
  expected.insert( { "egeon"_n, { 0, 0, 0 } } );
  expected.insert( { "pip"_n, { 0, 0, 0 } } );

  CHECK( t.get_voters() == expected );

  expect( t.alice.trace< edenproxy::actions::resign >( "ahab"_n ),
          "Missing required authority" );

  t.alice.act< edenproxy::actions::resign >( "alice"_n );

  expected.erase( "alice"_n );

  CHECK( t.get_voters() == expected );

  expect( t.fakeaccount.trace< edenproxy::actions::resign >( "account.fake"_n ),
          "Voter does not exist" );
}

TEST_CASE( "Change the Recipient" ) {
  tester t;

  t.alice.trace< edenproxy::actions::signup >( "alice"_n, "alice"_n );

  std::map< eosio::name, eosio::name > expected{ { "alice"_n, "alice"_n } };

  CHECK( t.get_recipients() == expected );

  expect( t.bob.trace< edenproxy::actions::changercpt >( "alice"_n,
                                                         "bob"_n,
                                                         false ),
          "Missing required authority" );
  expect( t.alice.trace< edenproxy::actions::changercpt >( "alice"_n,
                                                           "bob2"_n,
                                                           false ),
          "Recipient is not an account" );
  expect(
      t.bob.trace< edenproxy::actions::changercpt >( "bob"_n, "bob"_n, false ),
      "Voter does not exist" );

  t.alice.trace< edenproxy::actions::changercpt >( "alice"_n, "bob"_n, false );

  expected["alice"_n] = "bob"_n;

  CHECK( t.get_recipients() == expected );
}

TEST_CASE( "Send tokens from fake contract and symbol" ) {
  tester t;

  t.fund_accounts();

  t.eosproxyrwds.act< edenproxy::actions::init >();
  t.alice.with_code( "fake.token"_n )
      .act< token::actions::transfer >( "alice"_n,
                                        edenproxy::DEFAULT_FUNDING_CONTRACT,
                                        s2a( "500.0000 EOS" ),
                                        "donation" );
  t.alice.with_code( "fake.token"_n )
      .act< token::actions::transfer >( "alice"_n,
                                        "eosproxyrwds"_n,
                                        s2a( "500.0000 EOS" ),
                                        "donation" );
  t.alice.act< token::actions::transfer >( "alice"_n,
                                           "eosproxyrwds"_n,
                                           s2a( "500.0000 OTHER" ),
                                           "donation" );

  expect( t.alice.trace< token::actions::transfer >(
              "alice"_n,
              edenproxy::DEFAULT_FUNDING_CONTRACT,
              s2a( "500.0000 OTHER" ),
              "donation" ),
          "Invalid token symbol" );

  CHECK( t.get_account().balance == s2a( "0.0000 EOS" ) );

  t.alice.act< token::actions::transfer >( "alice"_n,
                                           edenproxy::DEFAULT_FUNDING_CONTRACT,
                                           s2a( "500.0000 EOS" ),
                                           "donation" );
  t.chain.as( "eosio"_n )
      .act< token::actions::transfer >( "eosio"_n,
                                        edenproxy::DEFAULT_FUNDING_CONTRACT,
                                        s2a( "500.0000 EOS" ),
                                        "donation" );
  t.alice.act< token::actions::transfer >( "alice"_n,
                                           "eosproxyrwds"_n,
                                           s2a( "500.0000 EOS" ),
                                           "donation" );
  t.chain.as( "eosio"_n )
      .act< token::actions::transfer >( "eosio"_n,
                                        "eosproxyrwds"_n,
                                        s2a( "500.0000 EOS" ),
                                        "donation" );

  CHECK( t.get_account().balance == s2a( "1000.0000 EOS" ) );
}

TEST_CASE( "Receive the inflation amount" ) {
  tester t;

  t.fund_accounts();

  t.eosproxyrwds.act< edenproxy::actions::init >();
  t.alice.act< token::actions::transfer >( "alice"_n,
                                           "eosproxyfund"_n,
                                           s2a( "500.0000 EOS" ),
                                           "donation" );

  CHECK( t.get_account().balance == s2a( "500.0000 EOS" ) );

  t.alice.act< token::actions::transfer >( "alice"_n,
                                           "eosproxyfund"_n,
                                           s2a( "12.0000 EOS" ),
                                           "donation" );

  CHECK( t.get_account().balance == s2a( "512.0000 EOS" ) );
}

TEST_CASE( "Distribute" ) {
  tester t;

  t.fund_accounts();
  t.skip_to( "2023-01-01T07:00:00.000" );

  t.eosproxyrwds.act< edenproxy::actions::init >();
  t.alice.act< token::actions::transfer >( "alice"_n,
                                           "eosproxyfund"_n,
                                           s2a( "512.0000 EOS" ),
                                           "donation" );

  t.full_signup();
  t.skip_to( "2023-01-02T06:59:50.500" );

  expect( t.alice.trace< edenproxy::actions::distribute >( 100 ),
          "Nothing to do" );

  t.skip_to( "2023-01-02T07:00:00.000" );
  t.alice.act< edenproxy::actions::distribute >( 100 );
  t.chain.start_block();

  expect( t.alice.trace< edenproxy::actions::distribute >( 100 ),
          "Nothing to do" );

  std::map< eosio::name, std::vector< uint64_t > > expected{
      { "ahab"_n, { 500000, 0, 853333 } },
      { "alice"_n, { 500000, 0, 853333 } },
      { "bertie"_n, { 500000, 0, 853333 } },
      { "bob"_n, { 500000, 0, 853333 } },
      { "egeon"_n, { 500000, 0, 853333 } },
      { "pip"_n, { 500000, 0, 853333 } } };

  CHECK( t.get_voters() == expected );
  CHECK( t.get_account().balance == s2a( "0.0002 EOS" ) );

  expect( t.alice.trace< edenproxy::actions::claim >( "bob"_n ),
          "Missing required authority" );
  t.chain.start_block();

  t.alice.act< edenproxy::actions::claim >( "alice"_n );
  t.chain.start_block();

  expect( t.alice.trace< edenproxy::actions::claim >( "alice"_n ),
          "No funds to claim" );

  expected["alice"_n] = { 500000, 853333, 0 };

  CHECK( t.get_voters() == expected );

  t.alice.act< token::actions::transfer >( "alice"_n,
                                           "eosproxyfund"_n,
                                           s2a( "600.0000 EOS" ),
                                           "donation" );

  t.skip_to( "2023-01-03T06:59:59.500" );

  expect( t.alice.trace< edenproxy::actions::distribute >( 100 ),
          "Nothing to do" );

  CHECK( t.get_account().balance == s2a( "600.0002 EOS" ) );

  t.chain.start_block();
  t.alice.act< edenproxy::actions::distribute >( 100 );

  expected["ahab"_n] = { 500000, 0, 1853333 };
  expected["alice"_n] = { 500000, 853333, 1000000 };
  expected["bertie"_n] = { 500000, 0, 1853333 };
  expected["bob"_n] = { 500000, 0, 1853333 };
  expected["egeon"_n] = { 500000, 0, 1853333 };
  expected["pip"_n] = { 500000, 0, 1853333 };

  CHECK( t.get_voters() == expected );
  CHECK( t.get_account().balance == s2a( "0.0002 EOS" ) );
}

TEST_CASE( "Slow distribution" ) {
  tester t;

  t.fund_accounts();
  t.skip_to( "2023-01-01T07:00:00.000" );

  t.eosproxyrwds.act< edenproxy::actions::init >();
  t.alice.act< token::actions::transfer >( "alice"_n,
                                           "eosproxyfund"_n,
                                           s2a( "512.0000 EOS" ),
                                           "donation" );

  t.full_signup();
  t.skip_to( "2023-01-02T06:59:50.500" );

  expect( t.alice.trace< edenproxy::actions::distribute >( 1 ),
          "Nothing to do" );

  t.skip_to( "2023-01-02T07:00:00.000" );

  // setup distribution
  t.alice.act< edenproxy::actions::distribute >( 1 );
  t.chain.start_block();

  // update voters
  for ( std::size_t i = 0; i < 6; ++i ) {
    t.alice.act< edenproxy::actions::distribute >( 1 );
    t.chain.start_block();
  }

  // distribute rewards
  for ( std::size_t i = 0; i < 6; ++i ) {
    t.alice.act< edenproxy::actions::distribute >( 1 );
    t.chain.start_block();
  }

  expect( t.alice.trace< edenproxy::actions::distribute >( 1 ),
          "Nothing to do" );

  CHECK( t.get_account().balance == s2a( "0.0002 EOS" ) );

  std::map< eosio::name, std::vector< uint64_t > > expected{
      { "ahab"_n, { 500000, 0, 853333 } },
      { "alice"_n, { 500000, 0, 853333 } },
      { "bertie"_n, { 500000, 0, 853333 } },
      { "bob"_n, { 500000, 0, 853333 } },
      { "egeon"_n, { 500000, 0, 853333 } },
      { "pip"_n, { 500000, 0, 853333 } } };

  CHECK( t.get_voters() == expected );
}

TEST_CASE( "Cannot resign with pending funds to claim" ) {
  tester t;

  t.fund_accounts();
  t.skip_to( "2023-01-01T07:00:00.000" );

  t.eosproxyrwds.act< edenproxy::actions::init >();
  t.alice.act< token::actions::transfer >( "alice"_n,
                                           "eosproxyfund"_n,
                                           s2a( "512.0000 EOS" ),
                                           "donation" );
  t.full_signup();
  t.skip_to( "2023-01-02T07:00:00.000" );
  t.alice.act< edenproxy::actions::distribute >( 100 );
  t.chain.start_block();

  expect( t.alice.trace< edenproxy::actions::resign >( "alice"_n ),
          "Need to claim the funds before to resign" );

  t.alice.act< edenproxy::actions::claim >( "alice"_n );
  t.alice.act< edenproxy::actions::resign >( "alice"_n );
}

TEST_CASE( "Cannot resign while a distributing" ) {
  tester t;

  t.fund_accounts();
  t.skip_to( "2023-01-01T07:00:00.000" );

  t.eosproxyrwds.act< edenproxy::actions::init >();
  t.alice.act< token::actions::transfer >( "alice"_n,
                                           "eosproxyfund"_n,
                                           s2a( "512.0000 EOS" ),
                                           "donation" );
  t.full_signup();
  t.skip_to( "2023-01-02T07:00:00.000" );
  t.alice.act< edenproxy::actions::distribute >( 1 );
  t.chain.start_block();

  expect( t.alice.trace< edenproxy::actions::resign >( "alice"_n ),
          "Cannot resign while a distribution is in progress" );

  t.alice.act< edenproxy::actions::distribute >( 100 );
  t.chain.start_block();
  t.alice.act< edenproxy::actions::claim >( "alice"_n );
  t.alice.act< edenproxy::actions::resign >( "alice"_n );
}

TEST_CASE( "Use default recipient if no one is set" ) {
  tester t;

  t.fund_accounts();
  t.skip_to( "2023-01-01T07:00:00.000" );

  t.eosproxyrwds.act< edenproxy::actions::init >();
  t.alice.act< token::actions::transfer >( "alice"_n,
                                           "eosproxyfund"_n,
                                           s2a( "512.0000 EOS" ),
                                           "donation" );

  t.alice.act< edenproxy::actions::signup >( "alice"_n, ""_n );

  std::map< eosio::name, eosio::name > expected{
      { "alice"_n, edenproxy::DEFAULT_FUNDING_CONTRACT } };

  CHECK( t.get_recipients() == expected );
}

TEST_CASE( "Disable claim funds for an account" ) {
  tester t;

  t.fund_accounts();
  t.skip_to( "2023-01-01T07:00:00.000" );

  t.eosproxyrwds.act< edenproxy::actions::init >();
  t.alice.act< token::actions::transfer >( "alice"_n,
                                           "eosproxyfund"_n,
                                           s2a( "512.0000 EOS" ),
                                           "donation" );

  t.alice.act< edenproxy::actions::signup >( "alice"_n, "alice"_n );
  t.skip_to( "2023-01-02T07:00:00.000" );

  std::map< eosio::name, eosio::name > expected{ { "alice"_n, "alice"_n } };

  CHECK( t.get_recipients() == expected );

  t.alice.act< edenproxy::actions::distribute >( 100 );
  t.chain.start_block();

  expect(
      t.bob.trace< edenproxy::actions::changercpt >( "alice"_n, ""_n, false ),
      "Missing required authority" );

  expect( t.eosproxyrwds.trace< edenproxy::actions::changercpt >( "alice"_n,
                                                                  ""_n,
                                                                  false ),
          "Missing required authority" );
  expect(
      t.alice.trace< edenproxy::actions::changercpt >( "alice"_n, ""_n, true ),
      "Missing required authority" );
  expect( t.eosproxyrwds.trace< edenproxy::actions::changercpt >( "alice"_n,
                                                                  "bob"_n,
                                                                  true ),
          "Admin can only ban" );

  t.eosproxyrwds.act< edenproxy::actions::changercpt >( "alice"_n, ""_n, true );

  expect( t.alice.trace< edenproxy::actions::changercpt >( "alice"_n,
                                                           "alice"_n,
                                                           false ),
          "Your account has been block by admins" );

  expected["alice"_n] = eosio::name{};

  CHECK( t.get_recipients() == expected );

  expect( t.alice.trace< edenproxy::actions::claim >( "alice"_n ),
          "Claiming has been blocked for alice" );
}

TEST_CASE( "Skip distribution if there are no accounts" ) {
  tester t;

  t.fund_accounts();
  t.skip_to( "2023-01-01T07:00:00.000" );

  t.eosproxyrwds.act< edenproxy::actions::init >();
  t.alice.act< token::actions::transfer >( "alice"_n,
                                           "eosproxyfund"_n,
                                           s2a( "512.0000 EOS" ),
                                           "donation" );

  t.skip_to( "2023-01-02T07:00:00.000" );
  t.alice.act< edenproxy::actions::distribute >( 1 );
  t.chain.start_block();

  CHECK(
      t.get_distribution().distribution_time ==
      eosio::time_point_sec( 1672729200 ) ); // == January 03, 2023 07:00 AM UTC

  t.skip_to( "2023-01-03T07:00:00.000" );
  t.alice.act< edenproxy::actions::distribute >( 1 );
  t.chain.start_block();

  CHECK(
      t.get_distribution().distribution_time ==
      eosio::time_point_sec( 1672815600 ) ); // == January 04, 2023 07:00 AM UTC
}

// TODO: test scenarios where the voter is not active
// create an special action of settable only available for the testing
#endif