/*
  ==============================================================================

    PolyBlep.h
    Created: 11 Mar 2025 6:37:39pm
    Author:  Carter Lynch

  ==============================================================================
*/

#pragma once

// polyBLEP to reduce aliasing
inline float polyBlep(double t, double dt) {
    if (t < dt) { // 0 <= t < 1
        t /= dt;
        // 2 * (t - t^2/2 - 0.5)
        return t + t - t * t - 1.;
    } else if (t > 1. - dt) { // -1 < t < 0
        t = (t - 1.) / dt;
        // 2 * (t^2/2 + t + 0.5)
        return t * t + t + t + 1.;
    } else {
        return 0.;
    }
}
