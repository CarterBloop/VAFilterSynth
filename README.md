# VAFilterSynth

A digital synthesizer plugin built with JUCE and C++. The purpose of this project is to experiment with digital models of analog circuits (virtual analog), commonly applied to electronic instruments and effect pedals. Features two sawtooth oscillators and a 4-pole diode-ladder VCF modeled on the EMS VCS3.

Main challenges were fighting aliasing from the digital waveform generation and latency from the CPU-hungry VCF emulation.

![VAFS Screenshot](/VAFS_Screenshot.png?raw=true "VAFS Screenshot")

## Features
- **Band-Limited**: Utilizes PolyBLEP to band-limit the digital oscillators and minimize aliasing.
- **Multi-Stage Oversampling**: Utilizes multi-stage oversampling to efficiently reduce aliasing.
- **Diode-Ladder VCF**: Nonlinear digital emulation of the EMS VCS3 filter.
  > Based on “A NONLINEAR DIGITAL MODEL OF THE EMS VCS3 VOLTAGE-CONTROLLED FILTER”
  > Marco Civolani & Federico Fontana ([link](https://www.researchgate.net/publication/224130819_Modeling_of_the_EMS_VCS3_Voltage-Controlled_Filter_as_a_Nonlinear_Filter_Network))

## VCF Discretization & Modeling Technique
The lowpass filter is modeled using the continuous-time diode-ladder equations from “A NONLINEAR DIGITAL MODEL OF THE EMS VCS3 VOLTAGE-CONTROLLED FILTER”, which are converted to a digital form using trapezoidal integration. This discretization method produces an implicit nonlinear system that must be solved every sample. The filter then uses a per-sample iterative solver to resolve the diode currents and ladder node voltages, with state variables representing the capacitor charges in each pole.

## Quick Start
1. Download VAFilterSynth.zip from [releases](https://github.com/CarterBloop/VAFilterSynth/releases/tag/0.1.0-alpha).
2. Zip contains VST3 and AU plugin formats.
4. Drop the resulting `.vst3`/`.component` into your host’s plugin folder.

## License

MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
