# Info
In addition to original cloudlogue-reverb, the pan/balance setting of stereo source signal can be used as send level.
Disadvantage is that pan left-right is (almost) not possible anymore, the raw audio will be either mono-centered (pan = L64 = minimum value), either sent to effect.
This effect is useful for external audio input.
Playing internal synth sound will therefore be fixed to 50% dry/wet ratio, since internal synth has centered pan.
So in addition to "clouds" plugin, there are 2 more flavours:
- cloudsP  : as described above (P from pan)
- cloudsPH : similar to cloudsP, but with attempt to allow hard pan left-right (PH from pan hard left/right). For e.g. at full level, if pan level is at boundaries (approx L64-L61 or R61-R64), the audio will be hard panned left, without pass through reverb. It works only if audio input snippets (drums) are panned left/right alternatively.

# Installation

## OS X

Requires:

```sh
# repo dependency
brew install jq

# get this repo
git clone https://github.com/peterall/cloudlogue-reverb.git
cd cloudlogue-reverb

# get submodules
git submodule update --init

# install logue sdk dependencies
cd logue-sdk
git submodule update --init
tools/gcc/get_gcc_osx.sh

# install eurorack dependencies
cd ../eurorack
git submodule update --init
```

# Compiling

Once you've gone through [Installation](#installation) above, run `make` from the repo root to build a `cloudlogue-reverb-x.x.x.zip` artifact.
