#! /usr/bin/env python

import sys
import aubio 
import numpy as np


def get_spectrogram(filename, samplerate=0):
    win_s = 320  # fft window size
    hop_s = 160  # hop size
    fft_s = win_s // 2 + 1  # spectrum bins

    a = aubio.source(filename, samplerate, hop_s)  # source file
    if samplerate == 0: samplerate = a.samplerate
    pv = aubio.pvoc(win_s, hop_s)  # phase vocoder
    specgram = np.zeros([0, fft_s], dtype=aubio.float_type)  # numpy array to store spectrogram

    # analysis
    while True:
        samples, read = a()  # read file
        sp0 = pv(samples)
        specgram = np.vstack((specgram, sp0.norm))  # store new norm vector
        if read < a.hop_size: break
    np.savetxt("specgram.txt", specgram.flatten(), fmt='%f')

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: %s <filename>" % sys.argv[0])
    else:
        for soundfile in sys.argv[1:]:
            get_spectrogram(soundfile)



