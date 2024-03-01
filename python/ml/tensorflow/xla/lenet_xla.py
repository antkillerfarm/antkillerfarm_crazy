"""
the demo which come from:
https://www.jianshu.com/p/6beedc7f83da
"""

import os
# input("pid: " + str(os.getpid()) +", press enter after attached")
import numpy as np
import tensorflow as tf
import argparse
from datetime import datetime
# from tensorflow.keras.mixed_precision import experimental as mixed_precision
# policy = mixed_precision.Policy('mixed_bfloat16')
# mixed_precision.set_policy(policy)
print("tf verison: " + tf.__version__)
# input("pid: " + str(os.getpid()) +", press enter after set breakpoints")
tf.keras.backend.clear_session()
tf.config.optimizer.set_jit(True) # Start with XLA disabled.
tf.debugging.set_log_device_placement(True)

parser = argparse.ArgumentParser()
parser.add_argument('--network', type=str,
    help='where to save model', default='lenet.json')
parser.add_argument('--weight', type=str,
    help='where to save weight', default='lenet.h5')
parser.add_argument('--clear', action='store_true',
    help='remove saved model before training')
parser.add_argument('--batch_size', type=int,
    help='set training bacth size', default=16)
parser.add_argument('--epochs', type=int,
    help='set training epochs number', default=3)
parser.add_argument('--dump', type=str,
    help='where to save dumped tensor', default='npu')
parser.add_argument('--full_test', action='store_true',
    help='determine enable full test or not')
parser.add_argument('--multi_dev', action='store_true',
    help='determine enable multi device or not')
args = parser.parse_args()

MODEL_FILE = args.network
MODEL_DATA_FILE = args.weight

if args.clear and os.path.exists(MODEL_FILE):
  print("Clear previous model at: %s" %MODEL_FILE)
  os.remove(MODEL_FILE)
  os.remove(MODEL_DATA_FILE)

FULL_TEST=args.full_test
ENABLE_MULTI_DEVICE = args.multi_dev

BATCH_SIZE = args.batch_size
DUMP_DIR = args.dump

# strategy = tf.distribute.MirroredStrategy(cross_device_ops=tf.distribute.ReductionToOneDevice())
strategy = tf.distribute.MirroredStrategy()

def load_data():
  (x_train, y_train), (x_test, y_test) = tf.keras.datasets.mnist.load_data()
  x_train = x_train.reshape(x_train.shape[0], 28, 28, 1)
  x_test = x_test.reshape(x_test.shape[0], 28, 28, 1)
  x_train = x_train.astype('float32') / 255
  x_test = x_test.astype('float32') / 255

  # Convert class vectors to binary class matrices.
  y_train = tf.keras.utils.to_categorical(y_train, num_classes=10)
  y_test = tf.keras.utils.to_categorical(y_test, num_classes=10)
  return ((x_train, y_train), (x_test, y_test))

(x_train, y_train), (x_test, y_test) = load_data()

if FULL_TEST:
  EPOCHS = args.epochs
else:
  EPOCHS = 1
  TRAIN_SIZE = BATCH_SIZE * 2
  x_train = x_train[0:TRAIN_SIZE, :, :, :]
  y_train = y_train[0:TRAIN_SIZE, :]

  x_test = x_test[0:2,:,:,:]
  y_test = y_test[0:2,:]

def generate_model():
  return tf.keras.models.Sequential([
    tf.keras.layers.Conv2D(1, kernel_size=(5, 5), activation='relu',
      input_shape=x_train.shape[1:]),
    # tf.keras.layers.MaxPooling2D(pool_size=(2, 2)),
    # tf.keras.layers.Conv2D(16, kernel_size=(5, 5), activation='relu'),
    # tf.keras.layers.MaxPooling2D(pool_size=(2, 2)),
    tf.keras.layers.Flatten(),
    # tf.keras.layers.Dense(120, activation='relu'),
    # tf.keras.layers.Dense(84, activation='relu'),
    tf.keras.layers.Dense(10, activation='softmax', dtype='float32'),
  ])

def compile_model(model):
  opt = tf.keras.optimizers.SGD(lr=0.0001)
  # opt = tf.keras.optimizers.SGD(lr=0.0001, momentum=0.9)
  # opt = tf.keras.optimizers.Adam(learning_rate=0.0001)
  # opt = tf.keras.optimizers.RMSprop(lr=0.0001, decay=1e-6)
  model.compile(loss='categorical_crossentropy',
                optimizer=opt,
                # run_eagerly=True,
                metrics=['accuracy'])
  return model

def train_model(model, x_train, y_train, x_test,
  y_test, epochs=1, batch_size=1):
  # Create a TensorBoard callback
  logs = "logs/" + datetime.now().strftime("%Y%m%d-%H%M%S")

  # profile_batch – It sets the batch or batches to be profiled,
  # the default value is 2, meaning the second batch will be profiled.
  # To disable profiling, set the value to zero, profile_batch can only
  # be a positive integer or a range let’s say (2,6) this will profile
  # batches from 2 to 6.
  tboard_callback = tf.keras.callbacks.TensorBoard(log_dir = logs,
                                                  profile_batch = 2)
  model.fit(x_train, y_train, batch_size=batch_size, epochs=epochs,
    validation_data=(x_test, y_test), shuffle=False, callbacks = [tboard_callback])

def warmup(model, x_train, y_train, x_test, y_test):
  # Warm up the JIT, we do not wish to measure the compilation time.
  initial_weights = model.get_weights()
  train_model(model, x_train, y_train, x_test, y_test, epochs=1)
  model.set_weights(initial_weights)

def load_model():
  if os.path.exists(MODEL_FILE):
    print("Load model from file: %s" %MODEL_FILE)
    json_string = open(MODEL_FILE, 'r').read() 
    model = tf.keras.models.model_from_json(json_string)
    model.load_weights(MODEL_DATA_FILE)
  else:
    model = generate_model()
  return model

if ENABLE_MULTI_DEVICE:
  with strategy.scope():
    model = load_model()
else:
  model = load_model()

model = compile_model(model)
model.summary()

def dump_tensors(tensors, prefix, tensors_name=None):
  if not os.path.isdir(DUMP_DIR): os.makedirs(DUMP_DIR)
  if not os.path.isdir(DUMP_DIR + "/" + prefix):
    os.makedirs(DUMP_DIR + "/" + prefix)
  if (tensors_name == None):
    ts_zip = zip(tensors, tensors)
  else:
    ts_zip = zip(tensors, tensors_name)
  for tensor in ts_zip:
    file_name = DUMP_DIR + "/" + prefix + "/" + \
      tensor[1].name.replace("/", "_").replace(":", "_") + ".txt"
    print(tensor[1].name, tensor[1].shape, " saved in: ", file_name)
    if hasattr(tensor[0], "numpy"):
      num = tensor[0].numpy()
    else:
      num = tensor[0]
    np.savetxt(file_name, num.flatten(), fmt='%.8f')

weights = [weight for layer in model.layers for weight in layer.weights]
dump_tensors(weights, "before")

def get_weight_grad(model, inputs, outputs):
  with tf.GradientTape() as tape:
    pred = model(inputs)
    loss = model.compiled_loss(tf.convert_to_tensor(outputs), pred, None,
                                   regularization_losses=model.losses)
  grad = tape.gradient(loss, model.trainable_weights)
  return grad

# warmup(model, x_train, y_train, x_test, y_test)
# train_model(model, x_train, y_train, x_test, y_test,
#   epochs=EPOCHS, batch_size=BATCH_SIZE)
# print("RRR 1")

if FULL_TEST:
  for i in range(EPOCHS):
    print("Epoch X: {}/{}".format(i + 1, EPOCHS))
    train_model(model, x_train, y_train, x_test, y_test,
      epochs=1, batch_size=BATCH_SIZE)
    dump_tensors(weights, "after/{}/".format(i + 1))
    model.save_weights(DUMP_DIR + "/after/{}/".format(i + 1) + MODEL_DATA_FILE)
else:
  train_model(model, x_train, y_train, x_test, y_test,
  epochs=1, batch_size=BATCH_SIZE)
  # weight_grads = get_weight_grad(model, x_train, y_train)
  # dump_tensors(weight_grads, "grads", model.trainable_weights)
  dump_tensors(weights, "after")
  model.save_weights(DUMP_DIR + "/after/" + MODEL_DATA_FILE)

if not os.path.exists(MODEL_FILE):
  json_string = model.to_json()
  open(MODEL_FILE, 'w').write(json_string) 
  model.save_weights(MODEL_DATA_FILE)
  print("RRR : save model.")

# outputs = [layer.output for layer in model.layers][1:]
# # all layer outputs except first (input) layer
# functor = tf.keras.backend.function([model.input], outputs)
# layer_outs = functor([x_train])
# dump_tensors(layer_outs, "output", outputs)

# print(model.predict(x_test, batch_size=BATCH_SIZE))

print("RRR : job finish.")
