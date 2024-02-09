import re

import pyresidfp
import pyresidfp._pyresidfp as _pyresidfp


BASE_VERSION_REGEX = re.compile(r"^([0-9]+\.[0-9]+(\.[0-9]+)?)(\.[^.]*)*$")


def test_version_matches_closely():
    base_version = BASE_VERSION_REGEX.sub(r"\1", pyresidfp.__version__)
    assert base_version == _pyresidfp.__version__
