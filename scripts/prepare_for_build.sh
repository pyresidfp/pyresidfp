#! /usr/bin/env bash

set -Eeu -o pipefail

echo "Removing previous _skbuild dir"
rm -rf _skbuild || :
