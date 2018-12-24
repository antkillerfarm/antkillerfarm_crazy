import tensorflow as tf
from tensorflow.keras.layers import concatenate 


def U_Net(features, labels, mode) :
    # Input Layer 
    
    input_layer = tf.reshape(features['mag'],[-1,512,128,1])
    
    # Convolutional Layer 1
    conv1 = tf.layers.batch_normalization(tf.layers.conv2d(inputs = input_layer, filters=16, kernel_size=[5,5], 
                                                       strides=[2,2], padding="same", activation=tf.nn.leaky_relu))
    
    # Convolutional Layer 2 
    conv2 = tf.layers.batch_normalization(tf.layers.conv2d(inputs = conv1, filters = 32, kernel_size = [5,5], 
                                                       strides = [2,2], padding="same", activation = tf.nn.leaky_relu))

    # Convolutional Layer 3
    conv3 = tf.layers.batch_normalization(tf.layers.conv2d(inputs = conv2, filters = 64, kernel_size = [5,5], 
                                                       strides = [2,2], padding="same", activation = tf.nn.leaky_relu))

    # Convolutional Layer 4
    conv4 = tf.layers.batch_normalization(tf.layers.conv2d(inputs = conv3, filters = 128, kernel_size = [5,5], 
                                                       strides = [2,2], padding="same", activation = tf.nn.leaky_relu))

    # Convolutional Layer 5
    conv5 = tf.layers.batch_normalization(tf.layers.conv2d(inputs = conv4, filters = 256, kernel_size = [5,5], 
                                                       strides = [2,2], padding="same", activation = tf.nn.leaky_relu))

    # Convolutional Layer 6
    conv6 = tf.layers.batch_normalization(tf.layers.conv2d(inputs = conv5, filters = 512, kernel_size = [5,5], 
                                                       strides = [2,2], padding="same", activation = tf.nn.leaky_relu))

    # Deconvolutional Layer1 (dropout)
    deconv1 = tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = conv6, filters = 256, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = tf.nn.relu))
    dropout1 = tf.layers.dropout(inputs = deconv1, rate = 0.5, training=mode == tf.estimator.ModeKeys.TRAIN)
    
    # Deconvolutional Layer2 (dropout)
    deconv2 = tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = concatenate([dropout1,conv5],3), filters = 128, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = tf.nn.relu))
    dropout2 = tf.layers.dropout(inputs = deconv2, rate = 0.5, training=mode == tf.estimator.ModeKeys.TRAIN)
                                      
    # Deconvolutional Layer3 (dropout)
    deconv3 = tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = concatenate([dropout2,conv4],3), filters = 64, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = tf.nn.relu))
    dropout3 = tf.layers.dropout(inputs = deconv3, rate = 0.5, training=mode == tf.estimator.ModeKeys.TRAIN)
                                      
    # Deconvolutional Layer4 
    deconv4 = tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = concatenate([dropout3,conv3],3), filters = 32, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = tf.nn.relu))
 
    # Deconvolutional Layer5 
    deconv5 = tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = concatenate([deconv4,conv2],3), filters = 16, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = tf.nn.relu))
    # Deconvolutional Layer6
    deconv6 = tf.layers.conv2d_transpose(inputs = concatenate([deconv5,conv1],3), filters = 1, kernel_size = [5,5],
                                                                   strides = [2,2], padding="same", activation = tf.nn.relu)
    
    predictions = {'outputs': deconv6
                  }
    
    if mode == tf.estimator.ModeKeys.PREDICT:
        return tf.estimator.EstimatorSpec(mode=mode, predictions=predictions)
    
    loss = tf.losses.absolute_difference(labels,deconv6)
    
    if mode == tf.estimator.ModeKeys.TRAIN:
        optimizer = tf.train.AdamOptimizer(1e-4)
        train_op = optimizer.minimize(
            loss=loss,
            global_step=tf.train.get_global_step())
        return tf.estimator.EstimatorSpec(mode=mode, loss=loss, train_op=train_op)
    
    return tf.estimator.EstimatorSpec(
        mode=mode, loss=loss)

def U_Net2(features, labels, mode) :
    # Input Layer 
    
    input_layer = tf.reshape(features['mag'],[-1,512,128,1])
    
    # Convolutional Layer 1
    conv1 = tf.nn.leaky_relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = input_layer, filters=16, kernel_size=[5,5], 
                                                       strides=[2,2], padding="same", activation = None)))
    
    # Convolutional Layer 2 
    conv2 = tf.nn.leaky_relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = conv1, filters = 32, kernel_size = [5,5], 
                                                       strides = [2,2], padding="same", activation = None)))

    # Convolutional Layer 3
    conv3 = tf.nn.leaky_relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = conv2, filters = 64, kernel_size = [5,5], 
                                                       strides = [2,2], padding="same", activation = None)))

    # Convolutional Layer 4
    conv4 = tf.nn.leaky_relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = conv3, filters = 128, kernel_size = [5,5], 
                                                       strides = [2,2], padding="same", activation = None)))

    # Convolutional Layer 5
    conv5 = tf.nn.leaky_relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = conv4, filters = 256, kernel_size = [5,5], 
                                                       strides = [2,2], padding="same", activation = None)))

    # Convolutional Layer 6
    conv6 = tf.nn.leaky_relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = conv5, filters = 512, kernel_size = [5,5], 
                                                       strides = [2,2], padding="same", activation = None)))

    # Deconvolutional Layer1 (dropout)
    deconv1 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = conv6, filters = 256, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = None)))
    dropout1 = tf.layers.dropout(inputs = deconv1, rate = 0.5, training=mode == tf.estimator.ModeKeys.TRAIN)
    
    # Deconvolutional Layer2 (dropout)
    deconv2 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = concatenate([dropout1,conv5],3), filters = 128, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = None)))
    dropout2 = tf.layers.dropout(inputs = deconv2, rate = 0.5, training=mode == tf.estimator.ModeKeys.TRAIN)
                                      
    # Deconvolutional Layer3 (dropout)
    deconv3 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = concatenate([dropout2,conv4],3), filters = 64, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = None)))
    dropout3 = tf.layers.dropout(inputs = deconv3, rate = 0.5, training=mode == tf.estimator.ModeKeys.TRAIN)
                                      
    # Deconvolutional Layer4 
    deconv4 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = concatenate([dropout3,conv3],3), filters = 32, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = None)))
 
    # Deconvolutional Layer5 
    deconv5 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = concatenate([deconv4,conv2],3), filters = 16, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = None)))
    # Deconvolutional Layer6
    deconv6 = tf.layers.conv2d_transpose(inputs = concatenate([deconv5,conv1],3), filters = 1, kernel_size = [5,5],
                                                                   strides = [2,2], padding="same", activation = tf.nn.relu)
    
    predictions = {'outputs': deconv6
                  }
    
    if mode == tf.estimator.ModeKeys.PREDICT:
        return tf.estimator.EstimatorSpec(mode=mode, predictions=predictions)
    
    loss = tf.losses.absolute_difference(labels,deconv6)
    
    if mode == tf.estimator.ModeKeys.TRAIN:
        optimizer = tf.train.AdamOptimizer(1e-4)
        train_op = optimizer.minimize(
            loss=loss,
            global_step=tf.train.get_global_step())
        return tf.estimator.EstimatorSpec(mode=mode, loss=loss, train_op=train_op)
    
    return tf.estimator.EstimatorSpec(
        mode=mode, loss=loss)

def U_Net3(features, labels, mode) :
    # Input Layer 
    
    input_layer = tf.reshape(features['mag'],[-1,512,128,1])
    
    # Convolutional Layer 1
    conv1 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = input_layer, filters=16, kernel_size=[5,5], 
                                                       strides=[2,2], padding="same", activation = None)))
    
    # Convolutional Layer 2 
    conv2 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = conv1, filters = 32, kernel_size = [5,5], 
                                                       strides = [2,2], padding="same", activation = None)))

    # Convolutional Layer 3
    conv3 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = conv2, filters = 64, kernel_size = [5,5], 
                                                       strides = [2,2], padding="same", activation = None)))

    # Convolutional Layer 4
    conv4 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = conv3, filters = 128, kernel_size = [5,5], 
                                                       strides = [2,2], padding="same", activation = None)))

    # Convolutional Layer 5
    conv5 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = conv4, filters = 256, kernel_size = [5,5], 
                                                       strides = [2,2], padding="same", activation = None)))

    # Convolutional Layer 6
    conv6 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = conv5, filters = 512, kernel_size = [5,5], 
                                                       strides = [2,2], padding="same", activation = None)))

    # Deconvolutional Layer1 (dropout)
    deconv1 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = conv6, filters = 256, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = None)))
    dropout1 = tf.layers.dropout(inputs = deconv1, rate = 0.5, training=mode == tf.estimator.ModeKeys.TRAIN)
    
    # Deconvolutional Layer2 (dropout)
    deconv2 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = concatenate([dropout1,conv5],3), filters = 128, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = None)))
    dropout2 = tf.layers.dropout(inputs = deconv2, rate = 0.5, training=mode == tf.estimator.ModeKeys.TRAIN)
                                      
    # Deconvolutional Layer3 (dropout)
    deconv3 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = concatenate([dropout2,conv4],3), filters = 64, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = None)))
    dropout3 = tf.layers.dropout(inputs = deconv3, rate = 0.5, training=mode == tf.estimator.ModeKeys.TRAIN)
                                      
    # Deconvolutional Layer4 
    deconv4 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = concatenate([dropout3,conv3],3), filters = 32, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = None)))
 
    # Deconvolutional Layer5 
    deconv5 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = concatenate([deconv4,conv2],3), filters = 16, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = None)))
    # Deconvolutional Layer6
    deconv6 = tf.layers.conv2d_transpose(inputs = concatenate([deconv5,conv1],3), filters = 1, kernel_size = [5,5],
                                                                   strides = [2,2], padding="same", activation = tf.nn.relu)
    
    predictions = {'outputs': deconv6
                  }
    
    if mode == tf.estimator.ModeKeys.PREDICT:
        return tf.estimator.EstimatorSpec(mode=mode, predictions=predictions)
    
    loss = tf.losses.absolute_difference(labels,deconv6)
    
    if mode == tf.estimator.ModeKeys.TRAIN:
        optimizer = tf.train.AdamOptimizer(1e-4)
        train_op = optimizer.minimize(
            loss=loss,
            global_step=tf.train.get_global_step())
        return tf.estimator.EstimatorSpec(mode=mode, loss=loss, train_op=train_op)
    
    return tf.estimator.EstimatorSpec(
        mode=mode, loss=loss)

def U_Net4(features, labels, mode) :
    # Input Layer 

    input_layer = tf.reshape(features['mag'],[-1,512,128,1])

    # Convolutional Layer 1
    pool1 = tf.nn.max_pool(input_layer, ksize = [1, 2, 2, 1], strides = [1, 2, 2, 1], padding = 'SAME')
    conv1 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = pool1, filters=16, kernel_size=[5,5], 
                                                       strides=[1,1], padding="same", activation = None)))

    # Convolutional Layer 2 
    pool2 = tf.nn.max_pool(conv1, ksize = [1, 2, 2, 1], strides = [1, 2, 2, 1], padding = 'SAME')
    conv2 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = pool2, filters = 32, kernel_size = [5,5], 
                                                       strides = [1,1], padding="same", activation = None)))

    # Convolutional Layer 3
    pool3 = tf.nn.max_pool(conv2, ksize = [1, 2, 2, 1], strides = [1, 2, 2, 1], padding = 'SAME')
    conv3 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = pool3, filters = 64, kernel_size = [5,5], 
                                                       strides = [1,1], padding="same", activation = None)))

    # Convolutional Layer 4
    pool4 = tf.nn.max_pool(conv3, ksize = [1, 2, 2, 1], strides = [1, 2, 2, 1], padding = 'SAME')
    conv4 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = pool4, filters = 128, kernel_size = [5,5], 
                                                       strides = [1,1], padding="same", activation = None)))

    # Convolutional Layer 5
    pool5 = tf.nn.max_pool(conv4, ksize = [1, 2, 2, 1], strides = [1, 2, 2, 1], padding = 'SAME')
    conv5 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = pool5, filters = 256, kernel_size = [5,5], 
                                                       strides = [1,1], padding="same", activation = None)))

    # Convolutional Layer 6
    pool6 = tf.nn.max_pool(conv5, ksize = [1, 2, 2, 1], strides = [1, 2, 2, 1], padding = 'SAME')
    conv6 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = pool6, filters = 512, kernel_size = [5,5], 
                                                       strides = [1,1], padding="same", activation = None)))

    # Deconvolutional Layer1 (dropout)
    deconv1 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = conv6, filters = 256, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = None)))
    dropout1 = tf.layers.dropout(inputs = deconv1, rate = 0.5, training=mode == tf.estimator.ModeKeys.TRAIN)
    
    # Deconvolutional Layer2 (dropout)
    deconv2 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = concatenate([dropout1,conv5],3), filters = 128, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = None)))
    dropout2 = tf.layers.dropout(inputs = deconv2, rate = 0.5, training=mode == tf.estimator.ModeKeys.TRAIN)
                                      
    # Deconvolutional Layer3 (dropout)
    deconv3 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = concatenate([dropout2,conv4],3), filters = 64, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = None)))
    dropout3 = tf.layers.dropout(inputs = deconv3, rate = 0.5, training=mode == tf.estimator.ModeKeys.TRAIN)
                                      
    # Deconvolutional Layer4 
    deconv4 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = concatenate([dropout3,conv3],3), filters = 32, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = None)))
 
    # Deconvolutional Layer5 
    deconv5 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = concatenate([deconv4,conv2],3), filters = 16, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = None)))
    # Deconvolutional Layer6
    deconv6 = tf.layers.conv2d_transpose(inputs = concatenate([deconv5,conv1],3), filters = 1, kernel_size = [5,5],
                                                                   strides = [2,2], padding="same", activation = tf.nn.relu)
    
    predictions = {'outputs': deconv6
                  }
    
    if mode == tf.estimator.ModeKeys.PREDICT:
        return tf.estimator.EstimatorSpec(mode=mode, predictions=predictions)
    
    loss = tf.losses.absolute_difference(labels,deconv6)
    
    if mode == tf.estimator.ModeKeys.TRAIN:
        optimizer = tf.train.AdamOptimizer(1e-4)
        train_op = optimizer.minimize(
            loss=loss,
            global_step=tf.train.get_global_step())
        return tf.estimator.EstimatorSpec(mode=mode, loss=loss, train_op=train_op)
    
    return tf.estimator.EstimatorSpec(
        mode=mode, loss=loss)

def U_Net5(features, labels, mode) :
    # Input Layer 

    input_layer = tf.reshape(features['mag'],[-1,512,128,1])

    # Convolutional Layer 1
    pool1 = tf.nn.max_pool(input_layer, ksize = [1, 2, 2, 1], strides = [1, 2, 2, 1], padding = 'SAME')
    conv1 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = pool1, filters=16, kernel_size=[3,3], 
                                                       strides=[1,1], padding="same", activation = None)))

    # Convolutional Layer 2 
    pool2 = tf.nn.max_pool(conv1, ksize = [1, 2, 2, 1], strides = [1, 2, 2, 1], padding = 'SAME')
    conv2 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = pool2, filters = 32, kernel_size = [3,3], 
                                                       strides = [1,1], padding="same", activation = None)))

    # Convolutional Layer 3
    pool3 = tf.nn.max_pool(conv2, ksize = [1, 2, 2, 1], strides = [1, 2, 2, 1], padding = 'SAME')
    conv3 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = pool3, filters = 64, kernel_size = [3,3], 
                                                       strides = [1,1], padding="same", activation = None)))

    # Convolutional Layer 4
    pool4 = tf.nn.max_pool(conv3, ksize = [1, 2, 2, 1], strides = [1, 2, 2, 1], padding = 'SAME')
    conv4 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = pool4, filters = 128, kernel_size = [3,3], 
                                                       strides = [1,1], padding="same", activation = None)))

    # Convolutional Layer 5
    pool5 = tf.nn.max_pool(conv4, ksize = [1, 2, 2, 1], strides = [1, 2, 2, 1], padding = 'SAME')
    conv5 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = pool5, filters = 256, kernel_size = [3,3], 
                                                       strides = [1,1], padding="same", activation = None)))

    # Convolutional Layer 6
    pool6 = tf.nn.max_pool(conv5, ksize = [1, 2, 2, 1], strides = [1, 2, 2, 1], padding = 'SAME')
    conv6 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d(inputs = pool6, filters = 512, kernel_size = [3,3], 
                                                       strides = [1,1], padding="same", activation = None)))

    # Deconvolutional Layer1 (dropout)
    deconv1 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = conv6, filters = 256, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = None)))
    dropout1 = tf.layers.dropout(inputs = deconv1, rate = 0.5, training=mode == tf.estimator.ModeKeys.TRAIN)
    
    # Deconvolutional Layer2 (dropout)
    deconv2 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = concatenate([dropout1,conv5],3), filters = 128, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = None)))
    dropout2 = tf.layers.dropout(inputs = deconv2, rate = 0.5, training=mode == tf.estimator.ModeKeys.TRAIN)
                                      
    # Deconvolutional Layer3 (dropout)
    deconv3 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = concatenate([dropout2,conv4],3), filters = 64, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = None)))
    dropout3 = tf.layers.dropout(inputs = deconv3, rate = 0.5, training=mode == tf.estimator.ModeKeys.TRAIN)
                                      
    # Deconvolutional Layer4 
    deconv4 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = concatenate([dropout3,conv3],3), filters = 32, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = None)))
 
    # Deconvolutional Layer5 
    deconv5 = tf.nn.relu(tf.layers.batch_normalization(tf.layers.conv2d_transpose(inputs = concatenate([deconv4,conv2],3), filters = 16, kernel_size = [5,5], 
                                                                 strides = [2,2], padding="same", activation = None)))
    # Deconvolutional Layer6
    deconv6 = tf.layers.conv2d_transpose(inputs = concatenate([deconv5,conv1],3), filters = 1, kernel_size = [5,5],
                                                                   strides = [2,2], padding="same", activation = tf.nn.relu)
    
    predictions = {'outputs': deconv6
                  }
    
    if mode == tf.estimator.ModeKeys.PREDICT:
        return tf.estimator.EstimatorSpec(mode=mode, predictions=predictions)
    
    loss = tf.losses.absolute_difference(labels,deconv6)
    
    if mode == tf.estimator.ModeKeys.TRAIN:
        optimizer = tf.train.AdamOptimizer(1e-4)
        train_op = optimizer.minimize(
            loss=loss,
            global_step=tf.train.get_global_step())
        return tf.estimator.EstimatorSpec(mode=mode, loss=loss, train_op=train_op)
    
    return tf.estimator.EstimatorSpec(
        mode=mode, loss=loss)
