This module is meant to be used to measure the mean ADC value of peaks coming from tracks.

Uses an embarassingly simple peak finding algorithm to find hits. The peak ADC values of those hits are stored, along with the wire number on which it was found.

The idea is to be able to do a simple calibration of the MIP peak ADC value, allowing one to determine a gaussian smearing function that can improve data/MC agreemenet.

