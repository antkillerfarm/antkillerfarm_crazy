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
  print('filter_size:{}'.format(filter_size))

  kernel = np.zeros([filter_size,
                    filter_size,
                    number_of_classes,
                    number_of_classes], dtype=np.float32)
    
  upsample_kernel = upsample_filt(filter_size)

  for i in range(number_of_classes):
    kernel[:, :, i, i] = upsample_kernel

  kernel1 = np.transpose(kernel, (3, 2, 1, 0))
  np.savetxt("output_deconv_bilinear_kernel.tensor", kernel1.flatten(), fmt='%f')

  return kernel

def run_deconv_nearest_neighbor(np_image, im_width, im_height):
  with tf.Graph().as_default() as graph:
    placeholder = tf.placeholder(tf.float32, shape=[1, im_height, im_width, 3],
                                 name='input_image')
    kernel = generate_upsample_kernel(2, 3)
    upsampled_shape = [1, 2*im_height, 2*im_width, 3]
    output = tf.nn.conv2d_transpose(
        placeholder, kernel, upsampled_shape, [1, 2, 2, 1], padding='SAME')

    with tf.Session() as sess:
      result = sess.run(output, feed_dict={placeholder: np_image})

    return result

def run_deconv_bilinear(np_image, im_width, im_height):
  with tf.Graph().as_default() as graph:
    placeholder = tf.placeholder(tf.float32, shape=[1, im_height, im_width, 3],
                                 name='input_image')
    kernel = generate_bilinear_upsample_kernel(2, 3)
    upsampled_shape = [1, 2*im_height, 2*im_width, 3]
    output = tf.nn.conv2d_transpose(
        placeholder, kernel, upsampled_shape, [1, 2, 2, 1], padding='SAME')
    output = tf.slice(output, [0, 64, 64, 0], [1, 2*im_height - 128, 2 * im_width - 128, 3])

    with tf.Session() as sess:
      result = sess.run(output, feed_dict={placeholder: np_image})

    return result

def run_resize_nearest_neighbor(np_image, im_width, im_height):
  with tf.Graph().as_default() as graph:
    placeholder = tf.placeholder(tf.float32, shape=[1, im_height, im_width, 3],
                                 name='input_image')
    upsampled_shape = [2*im_height, 2*im_width]
    output = tf.image.resize_nearest_neighbor(placeholder, upsampled_shape)

    with tf.Session() as sess:
      result = sess.run(output, feed_dict={placeholder: np_image})

    return result

def run_resize_bilinear(np_image, im_width, im_height):
  with tf.Graph().as_default() as graph:
    placeholder = tf.placeholder(tf.float32, shape=[1, im_height, im_width, 3],
                                 name='input_image')
    upsampled_shape = [2*im_height, 2*im_width]
    output = tf.image.resize_bilinear(placeholder, upsampled_shape)
    output = tf.slice(output, [0, 64, 64, 0], [1, 2*im_height - 128, 2 * im_width - 128, 3])

    with tf.Session() as sess:
      result = sess.run(output, feed_dict={placeholder: np_image})

    return result

def main(_):
  image = Image.open(FLAGS.image_path)
  (im_width, im_height) = image.size
  np_image = np.array(image.getdata()).reshape(
      (im_height, im_width, 3)).astype(np.uint8)
  np_image = np.expand_dims(np_image, axis=0)
  np.savetxt("input.tensor", np_image.flatten(), fmt='%d')

  #result = run_deconv_nearest_neighbor(np_image, im_width, im_height)
  #np.savetxt("output_deconv_nearest_neighbor.tensor", result.flatten(), fmt='%f')
  result = run_deconv_bilinear(np_image, im_width, im_height)
  np.savetxt("output_deconv_bilinear.tensor", result.flatten(), fmt='%f')
  #result = run_resize_nearest_neighbor(np_image, im_width, im_height)
  #np.savetxt("output_resize_nearest_neighbor.tensor", result.flatten(), fmt='%f')
  result = run_resize_bilinear(np_image, im_width, im_height)
  np.savetxt("output_resize_bilinear.tensor", result.flatten(), fmt='%f')

  #result = Image.fromarray(np.squeeze(result).astype(np.uint8))
  #result.save(FLAGS.out_path)

if __name__ == '__main__':
  tf.app.run()
