from tensorflow.python.framework import graph_util
import tensorflow as tf
import sys
import os

MainFolder = sys.argv[1]
MetaFileName = sys.argv[2]
OutputPBFileName = sys.argv[3]

ckpt = tf.train.get_checkpoint_state(MainFolder)
# print(ckpt.model_checkpoint_path)

# print(os.path.join(MainFolder,MetaFileName))
saver = tf.train.import_meta_graph(os.path.join(MainFolder,MetaFileName))
# print(saver)
sess = tf.Session()
saver.restore(sess, ckpt.model_checkpoint_path)

# print(sess.graph_def)
if len(sys.argv) > 4:
    output = list()
    for id in range(4, len(sys.argv)):
        output.append(sys.argv[id])
        frozen_graph = graph_util.convert_variables_to_constants(sess, sess.graph_def, output)
        tf.train.write_graph(frozen_graph, MainFolder, OutputPBFileName, as_text=False)
else:
    tf.train.write_graph(sess.graph_def, MainFolder, OutputPBFileName, as_text=False)