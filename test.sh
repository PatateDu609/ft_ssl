#!/bin/bash

make

function test_md5() {
	echo "Testing \`$1'"
	local mine=`echo -ne "$1" | ./ft_ssl md5 -r | cut -d' ' -f1`
	local openssl=`echo -ne "$1" | openssl md5 -r | cut -d' ' -f1`
	local md5sum=`echo -ne "$1" | md5sum | cut -d' ' -f1`

	if [ "$mine" != "$openssl" ] || [ "$mine" != "$md5sum" ]; then
		echo "\033[31;1mFAILED\033[0m: $1"
		echo "Mine: $mine"
		echo "OpenSSL: $openssl"
		echo "md5sum: $md5sum"
		exit 1
	fi
}

test_md5 ""
test_md5 "Hello World"
test_md5 "Hello World!"
test_md5 "Hello World!\n"
test_md5 "The quick brown fox jumps over the lazy dog"
test_md5 "The quick brown fox jumps over the lazy dog."
test_md5 "a"
test_md5 "abc"
test_md5 "message digest"
test_md5 "abcdefghijklmnopqrstuvwxyz"
test_md5 "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
test_md5 "1234567890123456789012345678901234567890\n1234567890123456789012345678901234567890"
