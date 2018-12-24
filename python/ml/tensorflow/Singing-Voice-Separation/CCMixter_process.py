import librosa
from librosa.util import find_files
from librosa import load
import os
import util


# Save Spectrogram 
def CCMixter() : 
    '''
    mix : original wav file
    source_1 : inst wav file 
    source_2 : vocal wac file 
    '''
    Audiolist = os.listdir('./data')
    for audio in Audiolist :
        try :
            audio_path = os.path.join('./data/'+audio)
            print("Song : %s" % audio_path)
            if os.path.exists(os.path.join('./Spectrogram',audio+'.npz')) :
                print("Already exist!! Skip....")
                continue
            aud = find_files(audio_path,ext="wav")
            #print(aud[0])
            #print(aud[1])
            #print(aud[2])
            mix,_ = load(aud[0], sr=None)
            inst,_ = load(aud[1], sr = None)
            vocal,_ = load(aud[2], sr=None)
            print("Saving...")
    
            util.SaveSpectrogram(mix, inst, vocal, audio)
        except IndexError as e :
            print("Wrong Directory")
            pass

if __name__ == '__main__' :
    CCMixter()
    print("Complete!!!!")