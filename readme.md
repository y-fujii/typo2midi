# typo2midi (WIP)

typo2midi is a simple CLI program that reads Linux keyboard events and emit
the MIDI messages to JACK.

The main purpose of typo2midi is not to emulate a MIDI keyboard, but to
generate typing sounds for live codings, etc.

## Build and run

    ninja
    ./build/typo2midi

Note that input device files (`/dev/input/event*`) are usually unreadable by
the normal user.  `sudo typo2midi` is not a solution since JACK or PipeWire
clients must be running on the same user.  Adding the user to `input` group
solves it on Arch Linux.

typo2midi simply maps a key code 0 .. 127 to the MIDI note number.  See also
`/usr/include/linux/input-event-codes.h`.

`sfz/typewriter.sfz` is a example typewriter SFZ instrument, which use the
samples from <http://subtersonic.weebly.com/blog/july-21st-2012> (WIP).  Run
`cd sfz; ./retrieve_samples.sh` to setup the samples before use.  My
recommended SFZ samplers are [sfizz](https://github.com/sfztools/sfizz) and
[liquidsfz](https://github.com/swesterfeld/liquidsfz).  Do not use a Carla's
internal SFZ sampler, which currently does not support some opcodes required by
`typewriter.sfz`.
