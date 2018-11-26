rm -rf bitcoin-cliapi-output/
mkdir -p bitcoin-cliapi-output/univalue/.libs/
cp univalue/.libs/libunivalue.a  bitcoin-cliapi-output/univalue/.libs/
cp bitcoin-cliapi.h  libbitcoin_cliapi.a  test.cpp  bitcoin-cliapi-output
cp make_test.sh bitcoin-cliapi-output
