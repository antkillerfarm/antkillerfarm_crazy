import os
import re
import hashlib

MAX_NUM_WAVS_PER_CLASS = 2**27 - 1  # ~134M

def which_set(filename, validation_percentage, testing_percentage):
  """Determines which data partition the file should belong to.

  We want to keep files in the same training, validation, or testing sets even
  if new ones are added over time. This makes it less likely that testing
  samples will accidentally be reused in training when long runs are restarted
  for example. To keep this stability, a hash of the filename is taken and used
  to determine which set it should belong to. This determination only depends on
  the name and the set proportions, so it won't change as other files are added.

  It's also useful to associate particular files as related (for example words
  spoken by the same person), so anything after '_nohash_' in a filename is
  ignored for set determination. This ensures that 'bobby_nohash_0.wav' and
  'bobby_nohash_1.wav' are always in the same set, for example.

  Args:
    filename: File path of the data sample.
    validation_percentage: How much of the data set to use for validation.
    testing_percentage: How much of the data set to use for testing.

  Returns:
    String, one of 'training', 'validation', or 'testing'.
  """
  base_name = os.path.basename(filename)
  # We want to ignore anything after '_nohash_' in the file name when
  # deciding which set to put a wav in, so the data set creator has a way of
  # grouping wavs that are close variations of each other.
  hash_name = re.sub(r'_nohash_.*$', '', base_name)
  # This looks a bit magical, but we need to decide whether this file should
  # go into the training, testing, or validation sets, and we want to keep
  # existing files in the same set even if more files are subsequently
  # added.
  # To do that, we need a stable way of deciding based on just the file name
  # itself, so we do a hash of that and then use that to generate a
  # probability value that we use to assign it.
  hash_name_hashed = hashlib.sha1(hash_name.encode("utf-8")).hexdigest()
  percentage_hash = ((int(hash_name_hashed, 16) %
                      (MAX_NUM_WAVS_PER_CLASS + 1)) *
                     (100.0 / MAX_NUM_WAVS_PER_CLASS))
  if percentage_hash < validation_percentage:
    result = 'validation'
  elif percentage_hash < (testing_percentage + validation_percentage):
    result = 'testing'
  else:
    result = 'training'
  return result

def save_list(l, file_name):
    with open(file_name,'w') as f:
        s = '\n'.join(l)
        f.write(s)
        f.close()

#DATASET_PATH = '/home/data/my/open_source/dataset/speech_commands'
DATASET_PATH = '/home/ubuser/my/dataset/speech_commands'
training_files = []
testing_files = []
validation_files = []
for parent, dirnames, filenames in os.walk(DATASET_PATH):
    for filename in filenames:
        if ".wav" in filename:
            fullname = os.path.join(parent, filename)
            result = which_set(fullname, 10, 10)
            if "training" in result:
                training_files.append(fullname.replace(DATASET_PATH + "/",''))
            elif "testing" in result:
                testing_files.append(fullname.replace(DATASET_PATH + "/", ''))
            else:
                validation_files.append(fullname.replace(DATASET_PATH + "/", ''))

save_list(training_files, DATASET_PATH + "/" + 'training_files.txt')
save_list(testing_files, DATASET_PATH + "/" + 'testing_files.txt')
save_list(validation_files, DATASET_PATH + "/" + 'validation_files.txt')