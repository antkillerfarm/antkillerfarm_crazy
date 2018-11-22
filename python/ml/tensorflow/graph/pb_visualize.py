#!/usr/bin/env python3

import tensorflow as tf
import os
import shutil
import signal
import tempfile
import sys

flags = tf.app.flags
flags.DEFINE_string('tf_pb', '', 'Tensorflow PB file path')
FLAGS = flags.FLAGS

tmp_graph_dir = tempfile.mkdtemp()
print(sys.path[0], tmp_graph_dir)

def exit(signum, frame):
    if os.path.exists(tmp_graph_dir):
        shutil.rmtree(tmp_graph_dir)
        print(tmp_graph_dir, 'Removed')
def main(_):
    with tf.Graph().as_default():
        import_graph_def = tf.GraphDef()
        with tf.gfile.GFile(FLAGS.tf_pb, "rb") as f:
            import_graph_def.ParseFromString(f.read())
            tf.import_graph_def(import_graph_def, name="")

        with tf.Session() as sess:
            tf.global_variables_initializer().run()
            train_writer = tf.summary.FileWriter(logdir=tmp_graph_dir, graph=sess.graph)
            train_writer.close()

    tensorboard_cmd = 'tensorboard --logdir ' + tmp_graph_dir
    tensorboard_cmd += ' --port ' + '0'
    os.system(tensorboard_cmd)

if __name__ == '__main__':
    signal.signal(signal.SIGINT, exit)
    signal.signal(signal.SIGTERM, exit)
    tf.app.run()
