from datetime import timedelta

from pyresidfp import SoundInterfaceDevice, Voice, ControlBits, Tone


def test_sample_length():
    """Emulate SID for 0.62 seconds, expect a corresponding PCM sample vector length"""
    # program SID
    sid = SoundInterfaceDevice()
    sid.Filter_Mode_Vol = 15  # Maximum volume
    sid.attack_decay(Voice.ONE, 190)  # 800 ms attack, 15 s decay
    sid.sustain_release(Voice.ONE, 248)  # sustain peak, 300 ms release
    sid.tone(Voice.ONE, Tone.C4)
    sid.control(Voice.ONE, ControlBits.TRIANGLE | ControlBits.GATE)

    # sample attack phase
    attack_phase = timedelta(seconds=0.32)
    raw_samples = sid.clock(attack_phase)

    # reprogram SID for release phase and sample
    release_phase = timedelta(seconds=0.3)
    sid.control(Voice.ONE, ControlBits.TRIANGLE)
    raw_samples.extend(sid.clock(release_phase))

    expected_vector_length = int(
        (attack_phase.total_seconds() + release_phase.total_seconds())
        * sid.sampling_frequency
    )
    error_margin = int(1 / 100 * expected_vector_length)
    assert (
        expected_vector_length - error_margin
        <= len(raw_samples)
        <= expected_vector_length + error_margin
    )
