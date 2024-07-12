# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0

# DeepSpeed Team
import importlib
import inspect
import functools
import os
from .abstract_accelerator import DeepSpeedAccelerator
# During setup stage torch may not be installed, pass on no torch will
# allow op builder related API to be executed.
try:
    import torch
    import torch_xla
    import torch_xla.core.xla_model as xm
except ImportError:
    pass

class XLA_Accelerator(DeepSpeedAccelerator):

    def __init__(self):
        super().__init__()
        self._name = 'xla'
        self._communication_backend_name = 'xla'
        # dict that holds class name <--> class type mapping i.e.
        # 'AsyncIOBuilder': <class 'op_builder.async_io.AsyncIOBuilder'>
        # this dict will be filled at init stage
        self.class_dict = None
        self.device_id = 0
        try:
            import psutil
            mem = psutil.Process().memory_info().rss
            self.max_mem = mem
        except ImportError as e:
            self.max_mem = 0

    def is_synchronized_device(self):
        return True

    def use_host_timers(self):
        return self.is_synchronized_device()

    def resolves_data_dependency(self):
        return self.is_synchronized_device()

    def handles_memory_backpressure(self):
        return self.is_synchronized_device()

    # Device APIs
    def device_name(self, device_index=None):
        if device_index is None:
            return 'xla'
#        return 'xla:{}'.format(device_index)
        return 'xla:0'.format(device_index)

    def device(self, device_index=None):
        return xm.xla_device(device_index)

    def set_device(self, device_index):
        print("set device : ", device_index, os.getpid())
#        torch.cuda.set_device(device_index)
        return

    def current_device(self):
        print("current device : ", torch.cuda.current_device(), os.getpid())
#        return torch.device("xla", index=torch.cuda.current_device())
        return 'xla:0'

    def current_device_name(self):
        return '{}'.format(self.current_device())

    def device_count(self):
        return 2

    def synchronize(self, device_index=None):
        return

    # RNG APIs
    def random(self):
        return torch.random

    def set_rng_state(self, new_state, device_index=None):
        if device_index is None:
            return xm.set_rng_state(101)
        return xm.set_rng_state(101, device_index)

    def get_rng_state(self, device_index=None):
        return torch.get_rng_state()
#        return xm.get_rng_state(device_index)

    def manual_seed(self, seed):
        torch.manual_seed(seed)

    def manual_seed_all(self, seed):
        torch.manual_seed(seed)

    def initial_seed(self, seed):
         return torch.initial_seed(seed)

    def default_generator(self, device_index):
        return torch.default_generator

    # Streams/Events
    @property
    def Stream(self):
        return None

    def stream(self, stream):
        from deepspeed.runtime.utils import noop_context
        return noop_context()

    def current_stream(self, device_index=None):
        return torch.cpu.current_stream()

    def default_stream(self, device_index=None):
        return None

    @property
    def Event(self):
        return None

    # Memory management
    def empty_cache(self):
        return

    def reset_rss(self):
        import psutil
        mem = psutil.Process().memory_info().rss
        self.max_mem = mem
        return mem

    def get_rss(self):
        import psutil
        mem = psutil.Process().memory_info().rss
        if mem > self.max_mem:
            self.max_mem = mem
        return mem

    def memory_allocated(self, device_index=None):
        return self.get_rss()

    def max_memory_allocated(self, device_index=None):
        self.get_rss()
        return self.max_mem

    def reset_max_memory_allocated(self, device_index=None):
        self.reset_rss()
        return
    def memory_cached(self, device_index=None):
        return self.get_rss()

    def max_memory_cached(self, device_index=None):
        self.get_rss()
        return self.max_mem

    def reset_max_memory_cached(self, device_index=None):
        self.reset_rss()
        return

    def memory_stats(self, device_index=None):
        mem = self.get_rss()
        mem_stat = {}
        mem_stat['allocated_bytes.all.current'] = mem
        mem_stat['allocated_bytes.all.peak'] = self.max_mem
        return mem_stat

    def reset_peak_memory_stats(self, device_index=None):
        self.reset_rss()
        return

    def memory_reserved(self, device_index=None):
        return self.get_rss()

    def max_memory_reserved(self, device_index=None):
        self.get_rss()
        return self.max_mem

    def total_memory(self, device_index=None):
        import psutil
        return psutil.virtual_memory().total

    def available_memory(self, device_index=None):
        import psutil
        return psutil.virtual_memory().available

    # Data types
    def is_bf16_supported(self):
        return True

    def is_fp16_supported(self):
        return True

    def supported_dtypes(self):
        return [torch.float, torch.bfloat16, torch.float16]
    # Misc
    def amp(self):
        return None

    def is_available(self):
        return True

    def range_push(self, msg):
        return

    def range_pop(self):
        return

    def lazy_call(self, callback):
        return callback()

    def communication_backend_name(self):
        return self._communication_backend_name

    def is_triton_supported(self):
        return False

    # Graph operations
    def create_graph(self):
        return None

    def capture_to_graph(self, graph, pool=None, stream=None):
        from deepspeed.runtime.utils import noop_context
        return noop_context()

    def replay_graph(self, graph):
        return

    # Tensor operations

    @property
    def BFloat16Tensor(self):
        return functools.partial(torch.tensor, dtype=torch.bfloat16, device='xla')

    @property
    def ByteTensor(self):
        return functools.partial(torch.tensor, dtype=torch.uint8, device='xla')

    @property
    def DoubleTensor(self):
        return functools.partial(torch.tensor, dtype=torch.double, device='xla')

    @property
    def FloatTensor(self):
        return functools.partial(torch.tensor, dtype=torch.float, device='xla')

    @property
    def HalfTensor(self):
        return functools.partial(torch.tensor, dtype=torch.half, device='xla')

    @property
    def IntTensor(self):
        return functools.partial(torch.tensor, dtype=torch.int, device='xla')

    @property
    def LongTensor(self):
        return functools.partial(torch.tensor, dtype=torch.long, device='xla')

    def pin_memory(self, tensor, align_bytes=1):
        print(tensor)
        return tensor.pin_memory(self.device())

    def is_pinned(self, tensor):
        return tensor.is_pinned()

    def on_accelerator(self, tensor):
        device_str = str(tensor.device)
        if device_str.startswith('xla:'):
            return True
        else:
            return False

    def op_builder_dir(self):
        try:
            # is op_builder from deepspeed or a 3p version? this should only succeed if it's deepspeed
            # if successful this also means we're doing a local install and not JIT compile path
            from op_builder import __deepspeed__  # noqa: F401 # type: ignore
            return "op_builder"
        except ImportError:
            return "deepspeed.ops.op_builder"

    def _lazy_init_class_dict(self):
        if self.class_dict:
            return

        op_builder_module = importlib.import_module(self.op_builder_dir())

        # get op builder class from op_builder/npu/__init__.py
        self.class_dict = {}
        for class_name, class_obj in inspect.getmembers(op_builder_module, inspect.isclass):
            self.class_dict[class_name] = class_obj

    # create an instance of op builder and return, name specified by class_name
    def create_op_builder(self, class_name):
        builder_class = self.get_op_builder(class_name)
        return None if builder_class is None else builder_class()

    # return an op builder class, name specified by class_name
    def get_op_builder(self, class_name):
        self._lazy_init_class_dict()
        if class_name in self.class_dict:
            return self.class_dict[class_name]
        else:
            return self.class_dict['NotImplementedBuilder'] if 'NotImplementedBuilder' in self.class_dict else None

    def build_extension(self):
        from torch.utils.cpp_extension import BuildExtension
        return BuildExtension

    def export_envs(self):
        return []
