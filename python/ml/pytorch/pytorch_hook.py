import torch
import torchvision
from torchinfo import summary

def hook_forward_fn(module, input, output):
    print(f"module: {module}")
    # print(f"input: {input}")
    # print(f"output: {output}")

# model = torchvision.models.quantization.mobilenet_v2(pretrained=True, quantize=True)
model = torchvision.models.mobilenet_v2(pretrained=True)
dummy_input = torch.rand(1, 3, 224, 224)
# summary(model, input_size=(1, 3, 224, 224))
# print(model)
layer_name = 'features.1.conv.0.0'
for (name, module) in model.named_modules():
    # print("Layer Name: " + name)
    if name == layer_name:
        module.register_forward_hook(hook=hook_forward_fn)

output = model(dummy_input)
# print(output)

children = model.children()
l = list(children)
# print(len(l))
model_1 = l[0]
children = model_1.children()
model_1 = list(children)[0]
# print(model_1)

output = model_1(dummy_input)
# print(output)

model = torchvision.models.quantization.mobilenet_v2(pretrained=True, quantize=True)
print(model)
children = model.children()
l = list(children)
children = l[0].children()
l_cut_model = [l[2], list(children)[0], l[3]]
model_1 = torch.nn.Sequential(*l_cut_model)
print(model_1)
output = model_1(dummy_input)
print("OK")
