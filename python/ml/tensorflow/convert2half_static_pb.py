import argparse
import tensorflow as tf
from tensorflow.python.framework.convert_to_constants import convert_variables_to_constants_v2
from resnet50 import get_resnet50, preprocess
from tensorflow.keras import mixed_precision
from labels.imagenet_1k import image_net_1k
import tf2onnx

SAVES_MODEL = False
SAVE_ONNX = False

def check_graph(model_path, summary=False):
    graph = tf.compat.v1.get_default_graph()
    graph_def = graph.as_graph_def()
    graph_def.ParseFromString(tf.compat.v1.gfile.FastGFile(model_path, 'rb').read())
    tf.import_graph_def(graph_def, name='resnet')
    if not summary:
        return graph
    with tf.compat.v1.Graph().as_default():
        summary = tf.compat.v1.summary.FileWriter('./log', graph)

def keras2frozen(keras_model, save_dir, pb_name, batch_size=1):
    # Convert Keras model to ConcreteFunction
    # Keyword arguments: {'Input': TensorSpec(shape=(1, 224, 224, 3), dtype=tf.float16, name='Input')}
    # full_model = tf.function(lambda Input: keras_model(Input=Input))
    full_model = tf.function(lambda input: keras_model(input))
    # tensorspec = tf.TensorSpec((batch_size,) + (224, 224, 3), tf.float16)
    tensorspec = tf.TensorSpec((batch_size,) + (224, 224, 3), tf.float32)
    print(tensorspec)
    # full_model = full_model.get_concrete_function(Input=tensorspec)
    full_model = full_model.get_concrete_function(tensorspec)
    
    # Get frozen ConcreteFunction
    # convert variables to constants
    frozen_func = convert_variables_to_constants_v2(full_model)
    frozen_func.graph.as_graph_def()

    # print input/output signature
    layers = [op.name for op in frozen_func.graph.get_operations()]
    print("-" * 100)
    print("Frozen model layers: ")
    for layer in layers:
        print(layer)
    print("-" * 100)
    print("Frozen model inputs: ")
    print(frozen_func.inputs)
    print("Frozen model outputs: ")
    print(frozen_func.outputs)

    # wirte frozen graph into file
    tf.io.write_graph(frozen_func.graph,
                      save_dir,
                      pb_name,
                      False)


if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--image_path", default='./images/panda.jpg')
    parser.add_argument("--summary", action='store_true')
    args = parser.parse_args()
    
    policy = mixed_precision.Policy('float16')  # compute and variable type: float16
    # policy = mixed_precision.Policy('mixed_float16')
    mixed_precision.set_global_policy(policy)
    print('compute  type: ', policy.compute_dtype)
    print('variable type: ', policy.variable_dtype)

    resnet50 = get_resnet50()
    # resnet50 = tf.saved_model.load('./saved_model_fused_conv_bn')

    if SAVE_ONNX:
        tf2onnx.convert.from_keras(resnet50,
                                   input_signature=[tf.TensorSpec((1, 224, 224, 3),
                                                                  tf.float32, 'Input')],
                                   output_path='./saved_model_onnx/resnet_fp16.onnx',
                                   opset=11)

    if SAVES_MODEL:
        keras2frozen(resnet50, './saved_static_pb', 'resnet_static.pb')
    
    graph = check_graph('./saved_static_pb/fused_resnet_static.pb', summary=args.summary)

    if graph:

        tf.compat.v1.disable_eager_execution()

        input = graph.get_tensor_by_name('input:0')
        output = graph.get_tensor_by_name('Identity:0')

        input_array = preprocess(args.image_path)

        with tf.compat.v1.Session() as sess:
            res = sess.run(output, feed_dict={input: input_array})
        
            topk = tf.math.top_k(res).indices.eval(session=sess)
            print(topk)
            for i, top in enumerate(topk):
                print('The {}th batch ----> top1 is {} and classification is [{}] and confidence is [{}]'.format(i, top, image_net_1k[top[0]], res[i][top[0]]))
