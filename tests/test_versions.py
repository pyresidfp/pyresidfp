import pyresidfp
import pyresidfp._pyresidfp as _pyresidfp


def test_version_matches():
    assert pyresidfp.__version__ == _pyresidfp.__version__
