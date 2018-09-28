import csv
import numpy as np
import librosa
import pickle

def save2pickle(fname, data1):
    f = open(fname, 'wb')
    pickle.dump(data1, f)

def loadpickle(fname):
    pkl_file = open(fname, 'rb')
    data1 = pickle.load(pkl_file)
    return data1

def loadfromecsv(fname):
    r = []
    with open(fname) as docfile:
        reader = csv.reader(docfile)
        for line in reader:
            r.append(line)
    return r

AUDIO_SR = 16000  # sampling rate

def tf_wave_to_stft(wave):
    sample_rate = 16000
    window_size = 0.02
    window_stride = 0.01
    window = 'hamming'
    normalize = True
    
    # y = librosa.core.load(wave, sr=sample_rate)[0]
    # print(len(y))
    
    n_fft = 320  # int(sample_rate * window_size)
    win_length = n_fft
    hop_length = 160  # int(sample_rate * window_stride)
    # STFT
    D = librosa.stft(wave, n_fft=n_fft, hop_length=hop_length,
                     win_length=win_length, window=window)
    
    spect, phase = librosa.magphase(D)
    # S = log(S+1)
    spect = np.log1p(spect)
    return spect

def _get_files_stft_librosa(wav_filenames):
    # print('Processing stft_librosa...')
    mfccs = []
    lens = []
    n_context = 0
    n_input = 161
    
    audio_fname = wav_filenames
    wave = librosa.core.load(audio_fname, sr=AUDIO_SR)[0]
    
    mfcc = tf_wave_to_stft(wave).T
    # print('max mfcc', mfcc.shape)
    
    feature_len = (len(mfcc) + 6 + 5) // 6 * 6
    
    if len(mfcc) < feature_len:
        needlen = feature_len - len(mfcc)
        INPUT_DIM = n_input
        a = np.array(([[0 for x in range(INPUT_DIM * (2 * n_context + 1))] for y in range(needlen)]))
        mfcc = np.concatenate((mfcc, a))
    mfccs.append(mfcc)
    lens.append(len(mfcc))
    a_mfccs = np.array(mfccs)
    a_lens = np.array(lens)
    return a_mfccs, a_lens



#######################################################################
if __name__ == '__main__':
    mfcc, mfcc_len = _get_files_stft_librosa('1188-133604-0025.flac.wav')
    #print('Done.')
