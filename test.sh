#!/bin/bash

make

function test_hash() {
	echo -ne "[\033[32;1m$2\033[0m]\t" ; echo "Testing '$1'"
		local mine=`echo -ne "$1" | ./ft_ssl $2 -r | cut -d' ' -f1`
	local openssl=`echo -ne "$1" | openssl $2 -r | cut -d' ' -f1`
	local md5sum=`echo -ne "$1" | $2sum | cut -d' ' -f1`

	if [ "$mine" != "$openssl" ] || [ "$mine" != "$md5sum" ]; then
		echo -ne "\033[31;1mFAILED\033[0m: " ; echo "$1"
		echo "Mine: $mine"
		echo "OpenSSL: $openssl"
		echo "md5sum: $md5sum"
		exit 1
	fi
}

test_hash "Hello World" md5
test_hash "Hello World!" md5
test_hash "Hello World!\n" md5
test_hash "The quick brown fox jumps over the lazy dog" md5
test_hash "The quick brown fox jumps over the lazy dog." md5
test_hash "a" md5
test_hash "abc" md5
test_hash "message digest" md5
test_hash "abcdefghijklmnopqrstuvwxyz" md5
test_hash "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" md5
test_hash "1234567890123456789012345678901234567890\n1234567890123456789012345678901234567890" md5
echo
test_hash "Hello World" sha256
test_hash "Hello World!" sha256
test_hash "Hello World!\n" sha256
test_hash "The quick brown fox jumps over the lazy dog" sha256
test_hash "The quick brown fox jumps over the lazy dog." sha256
test_hash "a" sha256
test_hash "abc" sha256
test_hash "message digest" sha256
test_hash "abcdefghijklmnopqrstuvwxyz" sha256
test_hash "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" sha256
test_hash "1234567890123456789012345678901234567890\n1234567890123456789012345678901234567890" sha256
