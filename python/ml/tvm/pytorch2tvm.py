import torch
import torch.nn as nn
import torchvision
import tvm
from tvm import relay
import tflite

class CNN(nn.Module):
    def __init__(self):
        super(CNN, self).__init__()
        self.conv1 = nn.Sequential(  # (1,28,28)
            nn.Conv2d(in_channels=1, out_channels=16, kernel_size=5,
                      stride=1, padding=2),  # (16,28,28)
            nn.ReLU(),
            nn.MaxPool2d(kernel_size=2)  # (16,14,14)
        )
        self.conv2 = nn.Sequential(  # (16,14,14)
            nn.Conv2d(16, 32, 5, 1, 2),  # (32,14,14)
            nn.ReLU(),
            nn.MaxPool2d(2)  # (32,7,7)
        )
        self.out = nn.Linear(32*7*7, 10)

    def forward(self, x):
        x = self.conv1(x)
        x = self.conv2(x)
        x = x.view(x.size(0), -1)
        output = self.out(x)
        return output

is_pytorch = True
if is_pytorch:
    model_quantized = torchvision.models.quantization.mobilenet_v2(pretrained=True, quantize=True)
    dummy_input = torch.rand(1, 3, 224, 224)
    # model_quantized = CNN()
    # dummy_input = torch.rand(1, 1, 28, 28)
    scripted_model = torch.jit.trace(model_quantized, dummy_input)
    # scripted_model.save("mobilenet_v2.pth")
    scripted_model = scripted_model.eval()
    # print(scripted_model)
    mod, params = relay.frontend.from_pytorch(scripted_model, [('input', dummy_input.shape)])
else:
    tflite_model_buf = open("./mobilenet_v1_0.25_224_quant.tflite", "rb").read()
    model = tflite.Model.GetRootAsModel(tflite_model_buf, 0)
    mod, params = relay.frontend.from_tflite(
        model, shape_dict={"input": (1, 224, 224, 3)}
    )

print(mod.astext())

target_string = "llvm -mtriple=x86_64-linux-gnu"              # linux host-triple
with tvm.transform.PassContext(opt_level=3, disabled_pass=None):
    lib = relay.build(mod, target_string, params=params)      # no error #

is_local = True
if is_local:
    ctx = tvm.cpu()
    mod = tvm.contrib.graph_executor.GraphModule(lib["default"](ctx))
else:
    # run `python3 -m tvm.exec.rpc_server --host 0.0.0.0 --port=9090` on another term before this step.
    lib_path = 'deploy_test.so'
    lib.export_library(lib_path)
    RPC_HOST = "127.0.0.1"
    RPC_PORT = 9090
    remote = tvm.rpc.connect(RPC_HOST, RPC_PORT)
    remote.upload(lib_path)
    lib = remote.load_module(lib_path)
    ctx = remote.cpu()
    mod = tvm.contrib.graph_executor.GraphModule(lib["default"](ctx))

mod.set_input('input', tvm.nd.array(dummy_input))
mod.run()
ref_out = mod.get_output(0)
print(ref_out.asnumpy())

print("OK")
