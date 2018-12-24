import tensorflow as tf

from U_net import *
from util import *
from config import *


def train() : 
    print("LoadSpectrogram...")
    X_list, Y_list = LoadSpectrogram()# Mix spectrogram
    print("Magnitude_phase...")
    X_mag,X_phase = Magnitude_phase(X_list)
    Y_mag,_ = Magnitude_phase(Y_list)
    print("Training...")
    deep_u_net = tf.estimator.Estimator(model_fn=U_Net,model_dir="./model")
    
    for e in range(EPOCH) :
        # Random sampling for training
        print("EPOCH:{}".format(e))
        X,y = sampling(X_mag,Y_mag)
        train_input_fn = tf.estimator.inputs.numpy_input_fn(x = {"mag":X},y = y,batch_size = BATCH,num_epochs = 1,shuffle = False)
        deep_u_net.train(input_fn= train_input_fn)

def train2(model_name, model_dir) : 
    deep_u_net = tf.estimator.Estimator(model_fn = model_name, model_dir = model_dir)
    
    for e in range(EPOCH) :
        # Random sampling for training
        print("EPOCH:{}".format(e))
        print("LoadSpectrogram...")
        batch_list = LoadSpectrogram_batch()
        for i in range(len(batch_list)) :
            X_list, Y_list = LoadSpectrogram_batch_items(batch_list[i])# Mix spectrogram
            print("Magnitude_phase...")
            X_mag,X_phase = Magnitude_phase(X_list)
            Y_mag,_ = Magnitude_phase(Y_list)
            print("Training...")
            X,y = sampling(X_mag,Y_mag)
            train_input_fn = tf.estimator.inputs.numpy_input_fn(x = {"mag":X},y = y,batch_size = BATCH,num_epochs = 1,shuffle = False)
            deep_u_net.train(input_fn= train_input_fn)

if __name__ == '__main__' :
    train2(U_Net5, './pool_3x3_model')
    print("Training Complete!!")
