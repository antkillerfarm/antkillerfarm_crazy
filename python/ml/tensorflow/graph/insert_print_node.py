#!/usr/bin/env python3
# encoding: utf-8
import sys, os
import numpy as np
import tensorflow as tf
from tensorflow.core.framework import node_def_pb2
from tensorflow.core.framework import attr_value_pb2

def create_node(op, name, inputs):
    new_node = node_def_pb2.NodeDef()
    new_node.op = op
    new_node.name = name
    for input_name in inputs:
        new_node.input.extend([input_name])
    return new_node

def set_attr_dtype(node, key, value):
    try:
        node.attr[key].CopyFrom(
            attr_value_pb2.AttrValue(type=value.as_datatype_enum))
    except KeyError:
        pass

def set_attr_dtype_list(node, key, value):
    list_value = attr_value_pb2.AttrValue.ListValue(type=[v.as_datatype_enum for v in value])
    try:
        node.attr[key].CopyFrom(attr_value_pb2.AttrValue(list=list_value))
    except KeyError:
        pass

def set_attr_int(node, key, value):
    try:
        node.attr[key].CopyFrom(attr_value_pb2.AttrValue(i=value))
    except KeyError:
        pass

def set_attr_string(node, key, value):
    try:
        node.attr[key].CopyFrom(attr_value_pb2.AttrValue(s=value))
    except KeyError:
        pass

def find_tensor_reference(graph, tensor_name):
    reference_nodes = []
    for i, node in enumerate(graph.node):
        for j, input in enumerate(node.input):
            if input == tensor_name:
                reference_nodes.append((i, j))
    return reference_nodes

def insert_print_node(graph, tensor_name, dtype=tf.float32, message='Tensor: '):
    tensor_name = tensor_name.replace(':0', '')
    reference_nodes = find_tensor_reference(graph, tensor_name)
    if len(reference_nodes) > 0:
        node_name = tensor_name.split(':')[0] + '_Shape'
        shape_node = create_node('Shape', node_name, [tensor_name])
        set_attr_dtype(shape_node, 'T', dtype)
        set_attr_dtype(shape_node, 'out_type', tf.int32)

        node_name = tensor_name.split(':')[0] + '_Print'
        print_node = create_node('Print', node_name, [tensor_name, shape_node.name, tensor_name])
        set_attr_dtype(print_node, 'T', dtype)
        set_attr_dtype_list(print_node, 'U', [tf.int32, dtype])
        set_attr_int(print_node, 'first_n', -1)
        set_attr_string(print_node, 'message', bytes(message, 'utf-8'))
        set_attr_int(print_node, 'summarize', 0x7FFFFFFF)

        for ref in reference_nodes:
            graph.node[ref[0]].input[ref[1]] = print_node.name

        graph.node.extend([print_node, shape_node])
    else:
        print('Warning: Invalid tensor! No node refers to tensor: {}, will not be printed.'.format(tensor_name))

    return graph

def parse_args():
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option('', '--pb', dest="pb", help="tensorflow .pb path")
    parser.add_option('', '--tensor', dest="tensor", help="tensorflow .pb path")
    parser.add_option('', '--dtype', dest="dtype", default='float32', help='dtype of target tensor, default float32')

    (options, args) = parser.parse_args()
    if options.pb and options.tensor:
        return options

    parser.print_help()
    sys.exit(-1)

if __name__ == '__main__':
    options = parse_args()
    if os.path.exists(options.pb):
        target_path = options.pb.replace('.pb', '.print.pb')
        with open(options.pb, 'rb') as fhndl:
            graph = tf.GraphDef()
            graph.ParseFromString(fhndl.read())
            
            dtype = getattr(tf, options.dtype)
            graph = insert_print_node(graph, options.tensor, dtype, message=options.tensor + ': ')
            tf.train.write_graph(graph, os.path.dirname(target_path), os.path.basename(target_path), as_text=False)
