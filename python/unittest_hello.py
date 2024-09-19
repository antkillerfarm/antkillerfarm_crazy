import sys
import unittest
from unittest.mock import patch


class MyTestCase(unittest.TestCase):
    def setUp(self):
        # 初始化测试环境
        self.data = [1, 2, 3, 4, 5]

    def tearDown(self):
        # 清理测试资源
        del self.data

    def test_addition(self):
        result = sum(self.data)
        self.assertEqual(result, 15)

    def test_empty_list(self):
        self.data = []
        result = sum(self.data)
        self.assertEqual(result, 0)

    @patch('sys.argv', ['my_cli_tool.py', 'arg1', 'arg2'])
    def test_argv(self):
        print(sys.argv)

if __name__ == '__main__':
    unittest.main()
