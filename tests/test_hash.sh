#!/bin/bash

EXIT_CODE=0

function test_hash() {
	local mine
	local openssl
	local sum_command
	local sum
	local output
	local output_sum

	echo -ne "[\033[32;1m$2\033[0m]\t" ; echo "Testing '$1'"


	mine=$(echo -ne "$1" | $MY_COMMAND "$2" -q)
	openssl=$(echo -ne "$1" | openssl "$2" -r | cut -d' ' -f1)

	# check if command $2sum exists
	sum_command=$(which "$2"sum 2> /dev/null)
	if [ -x "$sum_command" ]; then
		sum=$(echo -ne "$1" | "$2"sum | cut -d' ' -f1)
	fi

	if [ "$mine" != "$openssl" ] || { [ -x "$sum_command" ] && [ "$mine" != "$sum" ]; }; then
		echo -ne "\033[31;1mFAILED\033[0m: " ; echo "$1"
		output=$(printf "\tMine\t%s\n\tOpenSSL\t%s" "$mine" "$openssl")

		if [ -x "$sum_command" ]; then
			output_sum=$( printf "\t%ssum\t%s" "$2" "$sum" )
			output="$output|$output_sum"
		fi
		echo "$output" | tr '|' '\n' | column -t -s '\t'
		EXIT_CODE=1
	fi
}

function test_hash_fn() {
	test_hash "" "$1"
	test_hash "Hello World" "$1"
	test_hash "Hello World!" "$1"
	test_hash "Hello World!\n" "$1"
	test_hash "The quick brown fox jumps over the lazy dog" "$1"
	test_hash "The quick brown fox jumps over the lazy dog." "$1"
	test_hash "a" "$1"
	test_hash "abc" "$1"
	test_hash "message digest" "$1"
	test_hash "abcdefghijklmnopqrstuvwxyz" "$1"
	test_hash "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" "$1"
	test_hash "1234567890123456789012345678901234567890\n1234567890123456789012345678901234567890" "$1"
}

to_test="md5 sha224 sha256 sha384 sha512 sha512-224 sha512-256"


for i in $to_test; do
	test_hash_fn "$i"
	echo
done

exit $EXIT_CODE
