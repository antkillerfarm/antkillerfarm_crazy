import google.protobuf
from test_pb2 import TestMsg
import time

test = TestMsg()
test.id=1
test.time=int(time.time())
test.string="asdftest"
print test
test_str = test.SerializeToString()
print test_str

test1 = TestMsg()
test1.ParseFromString(test_str)
print test1

