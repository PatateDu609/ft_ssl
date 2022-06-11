#!/bin/bash

make

function test_hash() {
	echo -ne "[\033[32;1m$2\033[0m]\t" ; echo "Testing '$1'"

	local mine=`echo -ne "$1" | ./ft_ssl $2 -q`
	local openssl=`echo -ne "$1" | openssl $2 -r | cut -d' ' -f1`

	# check if command $2sum exists
	local sum_command=`which $2sum`
	if [ -x "$sum_command" ]; then
		local sum=`echo -ne "$1" | $2sum | cut -d' ' -f1`
	fi

	if [ "$mine" != "$openssl" ] || ( [ -x "$sum_command" ] && [ "$mine" != "$sum" ] ); then
		echo -ne "\033[31;1mFAILED\033[0m: " ; echo "$1"
		(echo "Mine\t$mine" ; echo "OpenSSL\t$openssl") | column -t -s '\t'

		if [ -x "$sum_command" ]; then
			echo "$2sum: $md5sum"
		fi
		exit 1
	fi
}

function test() {
	test_hash "" $1
	test_hash "Hello World" $1
	test_hash "Hello World!" $1
	test_hash "Hello World!\n" $1
	test_hash "The quick brown fox jumps over the lazy dog" $1
	test_hash "The quick brown fox jumps over the lazy dog." $1
	test_hash "a" $1
	test_hash "abc" $1
	test_hash "message digest" $1
	test_hash "abcdefghijklmnopqrstuvwxyz" $1
	test_hash "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" $1
	test_hash "1234567890123456789012345678901234567890\n1234567890123456789012345678901234567890" $1
}

to_test="md5 sha224 sha256 sha384 sha512 sha512-224 sha512-256"

for i in $to_test; do
	test $i
	echo
done
