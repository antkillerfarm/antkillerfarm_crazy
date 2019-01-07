import numpy as np
from PIL import Image
import tensorflow as tf

flags = tf.app.flags

flags.DEFINE_string('image_path', '', 'Path to image')
flags.DEFINE_string('out_path', '', 'Path to image')
FLAGS = flags.FLAGS

def generate_upsample_kernel(factor, number_of_classes):
  # Nearest neighbor interpolation
  kernel = np.zeros([factor, factor, number_of_classes , number_of_classes], dtype=np.float32)
  for i in range(number_of_classes):
    kernel[:,:,i,i] = 1
  return kernel

def get_kernel_size(factor):
  """
  Find the kernel size given the desired factor of upsampling.
  """
  return 2 * factor - factor % 2

def upsample_filt(size):
  """
  Make a 2D bilinear kernel suitable for upsampling of the given (h, w) size.
  """
  factor = (size + 1) // 2
  if size % 2 == 1:
    center = factor - 1
  else:
    center = factor - 0.5
  og = np.ogrid[:size, :size]
  return (1 - abs(og[0] - center) / factor) * \
    (1 - abs(og[1] - center) / factor)


def generate_bilinear_upsample_kernel(factor, number_of_classes):
  """
  Create weights matrix for transposed convolution with bilinear filter
  initialization.
  """

  filter_size = get_kernel_size(factor)

  kernel = np.zeros([filter_size,
                    filter_size,
                    number_of_classes,
                    number_of_classes], dtype=np.float32)
    
  upsample_kernel = upsample_filt(filter_size)

  for i in range(number_of_classes):
    kernel[:, :, i, i] = upsample_kernel

  return kernel

def main(_):
  image = Image.open(FLAGS.image_path)
  (im_width, im_height) = image.size
  np_image = np.array(image.getdata()).reshape(
      (im_height, im_width, 3)).astype(np.uint8)
  np_image = np.expand_dims(np_image, axis=0)

  with tf.Graph().as_default() as graph:
    placeholder = tf.placeholder(tf.float32, shape=[1, im_height, im_width, 3],
                                 name='input_image')
    #kernel = generate_upsample_kernel(2, 3)
    kernel = generate_bilinear_upsample_kernel(2, 3)
    upsampled_shape = [1, 2*im_height, 2*im_width, 3]
    output = tf.nn.conv2d_transpose(
        placeholder, kernel, upsampled_shape, [1, 2, 2, 1], padding='SAME')

    with tf.Session() as sess:
      result = sess.run(output, feed_dict={placeholder: np_image})

  result = Image.fromarray(np.squeeze(result).astype(np.uint8))
  result.save(FLAGS.out_path)

if __name__ == '__main__':
  tf.app.run()
