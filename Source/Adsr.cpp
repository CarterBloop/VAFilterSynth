/*
  ==============================================================================

    Adsr.cpp
    Created: 18 Mar 2025 1:56:35pm
    Author:  Carter Lynch

  ==============================================================================
*/

#include "Adsr.h"

void Adsr::update(const float attack, const float decay, const float sustain,
                  const float release) {
    adsr.attack = attack;
    adsr.decay = decay;
    adsr.sustain = sustain;
    adsr.release = release;

    setParameters(adsr);
}
