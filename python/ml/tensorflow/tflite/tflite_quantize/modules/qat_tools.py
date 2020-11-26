import tensorflow_model_optimization as tfmot
import tensorflow as tf
""" 
By default the QAT performs per channel quantization. However it can be changed to per-tensor. See per_axis setting. 
In order to do that, we have to provide own variant of QunatizeConfig template class. 
We cannot use the default  tfmot.quantization.keras.quantize_model, but in fact it is a shortcut for :
- quantize_annotate_model()
- quantize_apply()
In order to quantize the whole model: 
1. By using MyQuantizeConfig instance annotate layers which are known to perform per-channel quant (e.g. Convolutions)
annotate these layers with settings the per_axis=False
2. Annotate the remainig layer by quantize_annotate_model() call
3. Apply quantization with quantize_apply.  
"""
LastValueQuantizer = tfmot.quantization.keras.quantizers.LastValueQuantizer
MovingAverageQuantizer = tfmot.quantization.keras.quantizers.MovingAverageQuantizer

class PerTensor8bitQuantizeConfig(tfmot.quantization.keras.QuantizeConfig):
    '''
    Quantization configuration for quantization per tensor
    '''
    def get_weights_and_quantizers(self, layer):
        print("Get_weights_and_quantizers: ", layer.name)
        if hasattr(layer, 'kernel'):
            return [(layer.kernel, LastValueQuantizer(num_bits=8, per_axis=False, symmetric=True, narrow_range=True))]
        else:
            return super().get_weights_and_quantizers( layer)


    def get_activations_and_quantizers(self, layer):
        print("Get_activations_and_quantizers: ", layer.name)
        if hasattr(layer, 'activation'):
            return [(layer.activation, MovingAverageQuantizer(num_bits=8, per_axis=False, symmetric=False, narrow_range=False))]
        else:
            return super().get_activations_and_quantizers(layer)

    def set_quantize_weights(self, layer, quantize_weights):
        layer.kernel = quantize_weights[0]

    def set_quantize_activations(self, layer, quantize_activations):
        layer.activation = quantize_activations[0]

    def get_output_quantizers(self, layer):
        # Does not quantize output, since we return an empty list.
        return []

    def get_config(self):
        return {}

def apply_quantization_per_tensor(layer):
    if isinstance(layer, tf.keras.layers.Conv2D) or isinstance(layer, tf.keras.layers.DepthwiseConv2D):
        print('Appling custom quantization ', layer.name)
        return tfmot.quantization.keras.quantize_annotate_layer(layer, PerTensor8bitQuantizeConfig())
    print('Skiping layer ', layer.name)
    return layer

def check_per_channel(model):
    '''
    Check if model layers are set to per-tensor quantization.
    Args:
        model: model to check

    Returns:
        - False if any layer is set for per_asis quantization.
        - set of per_channel_nodes
    '''
    if model is None:
        raise ValueError('`model` cannot be None')
    if not isinstance(model, tf.keras.Model):
        raise ValueError('`model` can only be a tf.keras.Model instance. You passed an instance of type {input}.'.format(
            model.__class__.__name__))

    per_channel_nodes = set()
    for i in range(len(model.layers)):
        if not hasattr(model.layers[i], 'quantize_config'):
            print("Node {}, {}: Skipping".format(i, model.layers[i].name))
            continue
        if model.layers[i].quantize_config.activation_quantizer.per_axis == True:
            per_channel_nodes.add(model.layers[i])
            print("Node {}, {}: activation_quantizer: is set to per-channel quantization".format(i, model.layers[i].name))
        if model.layers[i].quantize_config.weight_quantizer.per_axis == True:
            per_channel_nodes.add(model.layers[i])
            print("Node {}, {}: weight_quantizer: is set to per-channel quantization".format(i, model.layers[i].name))

    return (len(per_channel_nodes) == 0), per_channel_nodes

def quantize_model(to_quantize):
    """Quantize a `tf.keras` model with the 8bit per-tensor quantization implementation.

      Quantization constructs a model which emulates quantization during training.
      This allows the model to learn parameters robust to quantization loss, and
      also model the accuracy of a quantized model.

      Implementation firstly annotate the selected layers of the to_quantize model with  PerTensor8bitQuantizeConfig.
      Next annotate the rest of the model with default implementation in tenrosflow_model_optimization

      For more information, see
      https://www.tensorflow.org/model_optimization/guide/quantization/training

      Quantize a model:

      ```python
      # Quantize sequential model
      model = quantize_model(
          keras.Sequential([
              layers.Dense(10, activation='relu', input_shape=(100,)),
              layers.Dense(2, activation='sigmoid')
          ]))

      # Quantize functional model
      in = tf.keras.Input((3,))
      out = tf.keras.Dense(2)(in)
      model = tf.keras.Model(in, out)

      quantized_model = quantize_model(model)
      ```

      Note that this function removes the optimizer from the original model.

      The returned model copies over weights from the original model. So while
      it preserves the original weights, training it will not modify the weights
      of the original model.

      Args:
        to_quantize: tf.keras model to be quantized. It can have pre-trained
          weights.

      Returns:
        Returns a new `tf.keras` model prepared for quantization.
      """
    if to_quantize is None:
        raise ValueError('`to_quantize` cannot be None')

    # Partially annotate selected model layers with PerTensor8bitQuantizeConfig
    annotated_model = tf.keras.models.clone_model(
        to_quantize,
        clone_function=apply_quantization_per_tensor,
    )
    # Finish annotation with remaining layers using tfmot defaults
    annotated_model = tfmot.quantization.keras.quantize_annotate_model(annotated_model)

    with tfmot.quantization.keras.quantize_scope(
            {'PerTensor8bitQuantizeConfig': PerTensor8bitQuantizeConfig}):
        q_aware_model = tfmot.quantization.keras.quantize_apply(annotated_model)
    return q_aware_model



