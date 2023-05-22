#! /usr/bin/env bash

set -Eeu -o pipefail

SCRIPTDIR=$(dirname "$(readlink -f "$0")")
TOPDIR=$(dirname "${SCRIPTDIR}")
TMPFOLDER=$(mktemp --tmpdir -d "pyresidfp-dev.XXXXXXXX")

cleanup() {
    rm -r "${TMPFOLDER}" || :
    exit
}

trap cleanup EXIT INT TERM

python -m venv "${TMPFOLDER}/venv"

. "${TMPFOLDER}/venv/bin/activate"

python -m pip install -r "${SCRIPTDIR}/requirements-dev.txt"

python -m pip install "${TOPDIR}" 

pybind11-stubgen -o "${TOPDIR}/src" --no-setup-py --root-module-suffix "" pyresidfp._pyresidfp

black "${TOPDIR}/src/pyresidfp"

cleanup
