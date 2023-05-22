#! /usr/bin/env bash

set -Eeu -o pipefail

SCRIPTDIR=$(dirname "$(readlink -f "$0")")
TOPDIR=$(dirname "${SCRIPTDIR}")
TMPFOLDER=$(mktemp --tmpdir -d "pyresidfp-dev.XXXXXXXX")

cleanup() {
    rm -r "${TMPFOLDER}" || :
    exit
}

trap cleanup INT TERM EXIT

python -m venv "${TMPFOLDER}/venv"

. "${TMPFOLDER}/venv/bin/activate"

python -m pip install -r "${SCRIPTDIR}/requirements-dev.txt"

python -m pip install "${TOPDIR}" 

black --check "${TOPDIR}/src/pyresidfp"

mypy -p pyresidfp

cleanup
