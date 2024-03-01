import argparse
import os
import sys
from torch_xla import runtime as xr

import os
import shutil
import sys
import numpy as np
import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from torchvision import datasets, transforms
import torch_xla
import torch_xla.debug.metrics as met
import torch_xla.distributed.parallel_loader as pl
import torch_xla.utils.utils as xu
import torch_xla.core.xla_model as xm
from torch.testing._internal.common_utils import (
    TestCase, run_tests,
)

BPU_DEVICE = xm.xla_device()
CPU_DEVICE = torch.device("cpu")

class MNIST(nn.Module):

  def __init__(self):
    super(MNIST, self).__init__()
    # self.conv1 = nn.Conv2d(10, 20, kernel_size=3)
    self.conv1 =nn.Conv2d(256, 512, kernel_size=(1, 1), stride=(2, 2), bias=False)
    self.bn1 = nn.BatchNorm2d(256, eps=1e-05, momentum=0.1, affine=True, track_running_stats=True)
    self.conv2 = nn.Conv2d(256, 256, kernel_size=(3, 3), stride=(2, 2), padding=(1, 1), bias=False)
    self.bn2 = nn.BatchNorm2d(256, eps=1e-05, momentum=0.1, affine=True, track_running_stats=True)
    self.conv3 = nn.Conv2d(256, 1024, kernel_size=(1, 1), stride=(1, 1), bias=False)
    self.bn3 = nn.BatchNorm2d(1024, eps=1e-05, momentum=0.1, affine=True, track_running_stats=True)
    self.relu = nn.ReLU(inplace=True)

  def forward(self, x):
    x = self.conv1(x)
    # x = self.bn1(x)
    # x = self.conv2(x)
    # x = self.bn2(x)
    # x = self.conv3(x)
    # x = self.bn3(x)
    # x = self.relu(x)
    return x

class TestModule(TestCase):
    def test_mnist(self):
        x_cpu = torch.randn(1, 256, 28, 28, device=CPU_DEVICE, dtype=torch.float)
        print("input is : ", x_cpu)
        model_cpu = MNIST().to(CPU_DEVICE)
        model_cpu.eval()
        output_cpu = model_cpu.forward(x_cpu)
        print("cpu resulut is : ", output_cpu)
        model_cpu = model_cpu.half()
        x_boss = x_cpu.detach().clone().to(device=BPU_DEVICE).to(torch.float16)
        model_boss = model_cpu.to(BPU_DEVICE)
        # model_boss.weight.data = model_cpu.weight.data.detach().clone().to(BPU_DEVICE)
        # model_boss.bias.data = model_cpu.bias.data.detach().clone().to(BPU_DEVICE)
        model_boss.eval()
        output_boss = model_boss.forward(x_boss)
        print("boss resulut is : ", output_boss)
        self.assertEqual(output_cpu, output_boss.to(torch.float), rtol=1e-01, atol=1e-03)
    # def test_mnist_dataset(self):
    #     test_dataset = datasets.MNIST(
    #     os.path.join(flags.datadir, str(xm.get_ordinal())),
    #     train=False,
    #     download=True,
    #     transform=transforms.Compose(
    #         [transforms.ToTensor(),
    #          transforms.Normalize((0.1307,), (0.3081,))]))
if __name__ == '__main__':
    run_tests()