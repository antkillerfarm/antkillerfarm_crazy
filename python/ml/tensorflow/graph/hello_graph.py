# coding: utf-8
import tensorflow as tf

def load_graph(frozen_graph_filename):
    print('loading', frozen_graph_filename)
    # We parse the graph_def file
    with tf.Session() as sess:
        with tf.gfile.GFile(frozen_graph_filename, "rb") as f:
            graph_def = tf.GraphDef()
            graph_def.ParseFromString(f.read())

        sess.graph.as_graph_def()
        # We load the graph_def in the default graph
        with tf.Graph().as_default() as graph:
            tf.import_graph_def(
                graph_def,
                name="prefix",
            )
    return graph

graph = load_graph(savedmodel_tf_fname_pb)
for op in graph.get_operations():
    print(op.name, op.values())
    pass

x = self.graph.get_tensor_by_name('prefix/x:0')
sess = tf.Session(graph=graph)
