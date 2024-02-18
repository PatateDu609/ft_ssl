#!/usr/bin/env bash

make

export MY_COMMAND="./ft_ssl"
#export MY_COMMAND="valgrind --leak-check=full --show-leak-kinds=all -s ./ft_ssl"

#echo "Launching hash tests"
#bash tests/test_hash.sh

#echo "Launching base64 tests"
#bash tests/test_base64.sh

echo "Launching enc tests"
bash tests/test_enc.sh
