#!/bin/bash

EXIT_CODE=0

mkdir -p .tmp

function test_base64() {
	local test_mode
	local test_type
	local tested_string

	test_mode="$1"
	test_type="$2"
	tested_string="$3"

	local mine
	local openssl
	local output
	local test_label
	local test_label_color_code
	local test_option
	local label_tested_string

	if [ ${#tested_string} -gt 50 ]; then
		label_tested_string="${tested_string:0:15} ... ${tested_string: -15}"
	elif [ ${#tested_string} -ne 0 ]; then
		label_tested_string="$tested_string"
	else
		label_tested_string="''"
	fi

	if [ "$test_mode" = "enc" ]; then
		test_label_color_code="160"
		test_label="encrypt"
		test_option="-e"
	elif [ "$test_mode" = "dec" ]; then
		test_label_color_code="119"
		test_label="decrypt"
		test_option="-d"
	else
		echo "Unknown test mode: $test_mode"
		exit 1
	fi

	if [ "$test_type" = "str" ]; then
		printf "[\033[32;1mbase64\033[0m \033[38;5;%sm%s\033[0m]\t" "$test_label_color_code" "$test_label"
		echo "Testing $label_tested_string"

		mine=$(echo -ne "$tested_string" | ./ft_ssl base64 "$test_option")
		openssl=$(echo -ne "$tested_string" | openssl base64 "$test_option")
	elif [ "$test_type" = "file" ]; then
		printf "[\033[32;1mbase64\033[0m \033[38;5;%sm%s\033[0m]\t" "$test_label_color_code" "$test_label"
		echo "Testing file $label_tested_string"

		mine=$(./ft_ssl base64 "$test_option" -i "$tested_string")
		openssl=$(openssl base64 "$test_option" -in "$tested_string")
	else
		echo "Unknown test type: $test_type"
		exit 1
	fi

	if [ "$mine" != "$openssl" ]; then
		echo -ne "\033[31;1mFAILED\033[0m: "
		echo "$label_tested_string (type=$test_type)"
		echo "$mine" >.tmp/mine.txt
		echo "$openssl" >.tmp/openssl.txt
		output=$(
			echo "FILE1=openssl        FILE2=mine"
			diff .tmp/openssl.txt .tmp/mine.txt
		)

		echo "$output"
		EXIT_CODE=1
	fi
}

test_base64 enc str "oisvjoij"
test_base64 dec str "$(base64 Makefile)"
test_base64 enc str ""
test_base64 dec str "$(echo -n "" | base64)"

echo
echo -n "oisvjoij" >.tmp/basic.txt
touch .tmp/empty.txt

base64 .tmp/basic.txt >.tmp/basic.b64
base64 Makefile >.tmp/Makefile.b64
base64 .tmp/empty.txt >.tmp/empty.b64

test_base64 enc file .tmp/basic.txt
test_base64 enc file Makefile
test_base64 enc file .tmp/empty.txt

test_base64 dec file .tmp/basic.b64
test_base64 dec file .tmp/Makefile.b64
test_base64 dec file .tmp/empty.b64

rm -rf .tmp

exit $EXIT_CODE
