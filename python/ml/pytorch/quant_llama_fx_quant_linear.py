
import torch
from torch import nn
import torch_xla
import torch_xla.core.xla_model as xm

from torch.ao.quantization import get_default_qconfig
from torch.ao.quantization import QConfigMapping, QConfig, MinMaxObserver, default_observer, PerChannelMinMaxObserver, HistogramObserver
from torch.ao.quantization.quantize_fx import prepare_fx, convert_fx, _convert_fx

from typing import Any, Dict, Optional, Tuple, Union, Type, List

from transformers.utils.fx import (
    #symbolic_trace, 
    HFTracer, 
    get_concrete_args, 
    check_if_model_is_supported
)
from torch.fx import Node
from torch.ao.quantization.fx.custom_config import (
    PrepareCustomConfig,
)

from torch.ao.quantization.backend_config import (
    BackendConfig,
    get_tensorrt_backend_config,
)
from torch.ao.quantization.quantize_fx import (
    _swap_ff_with_fxff, 
    get_skipped_module_name_and_classes, 
    _attach_meta_to_node_if_not_exist, 
    attach_preserved_attrs_to_model,
)
from torch.ao.quantization.fx.prepare import prepare 
from torch.fx import GraphModule


MODELPATH = '/data/tianmu/models/llama-2-7b-hf'

def _prepare_fx(
    model: torch.nn.Module,
    qconfig_mapping: Union[QConfigMapping, Dict[str, Any]],
    is_qat: bool,
    example_inputs: Tuple[Any, ...],
    prepare_custom_config: Union[PrepareCustomConfig, Dict[str, Any], None] = None,
    _equalization_config: Optional[Union[QConfigMapping, Dict[str, Any]]] = None,
    backend_config: Union[BackendConfig, Dict[str, Any], None] = None,
    is_standalone_module: bool = False,
) -> GraphModule:
    r""" Internal helper function for prepare_fx
    Args:
      `model`, `qconfig_mapping`, `prepare_custom_config`, `_equalization_config`:
      see docs for :func:`~torch.ao.quantization.prepare_fx`
      `is_standalone_module`: a boolean flag indicates whether we are
      quantizing a standalone module or not, a standalone module
      is a submodule of the parent module that is not inlined in the
forward graph of the parent module,
      the way we quantize standalone module is described in:
      :func:`~torch.ao.quantization._prepare_standalone_module_fx`
    """
    if prepare_custom_config is None:
        prepare_custom_config = PrepareCustomConfig()
    if _equalization_config is None:
        _equalization_config = QConfigMapping()
        
    # swap FloatFunctional with FXFloatFunctional
    _swap_ff_with_fxff(model)

    skipped_module_names, skipped_module_classes = \
        get_skipped_module_name_and_classes(prepare_custom_config, is_standalone_module)
    preserved_attr_names = prepare_custom_config.preserved_attributes
    preserved_attrs = {attr: getattr(model, attr) for attr in preserved_attr_names if hasattr(model, attr)}
    # symbolically trace the model
    input_names = model.dummy_inputs.keys()
    input_names = list(input_names)
    concrete_args = get_concrete_args(model, input_names)
    tracer = HFTracer()
    # tracer = QuantizationTracer(skipped_module_names, skipped_module_classes)  # type: ignore[arg-type]
    graph_module = GraphModule(model, tracer.trace(model, concrete_args=concrete_args))
    # print("==============graph_module.graph")
    # print(graph_module.graph)
    # print("==============tracer.node_name_to_scope")
    # print(tracer.node_name_to_scope)
    _attach_meta_to_node_if_not_exist(graph_module)

    prepared = prepare(
        graph_module,
        qconfig_mapping,
        is_qat,
        tracer.node_name_to_scope,
        example_inputs=example_inputs,
        prepare_custom_config=prepare_custom_config,
        _equalization_config=_equalization_config,
        backend_config=backend_config,
        is_standalone_module=is_standalone_module,
    )  # type: ignore[operator]

    attach_preserved_attrs_to_model(prepared, preserved_attrs)
    return prepared

class CustomModel(torch.nn.Module):
    def __init__(self):
        super(CustomModel, self).__init__()
        self.layer0 = nn.Linear(2, 2, bias=False)
        self.layer1 = nn.Linear(2, 2, bias=False)

    def forward(self, input):
        x = self.layer0(input)
        x1 = self.layer1(x)
        return x1
    
class ModelQuant(torch.nn.Module):

    def __init__(self, model_path):
        super(ModelQuant, self).__init__()
        model = CustomModel()
        
        self.model = model
        
    def forward(self, input):
        outputs = self.model(input)
        
        return outputs
        
    def quantize(self, input):
        qconfig = get_default_qconfig("qnnpack")

        qconfig = QConfig(
            activation=HistogramObserver.with_args(dtype=torch.quint8),
            weight=PerChannelMinMaxObserver.with_args(dtype=torch.qint8, qscheme=torch.per_channel_symmetric))
        matmul_config = QConfig(
            activation=MinMaxObserver.with_args(dtype=torch.quint8),
            weight=MinMaxObserver.with_args(dtype=torch.quint8))
        print("=============qconfig:")
        print(qconfig)
        qconfig_mapping = QConfigMapping().set_object_type(torch.nn.Linear, qconfig).set_object_type(torch.matmul, matmul_config)
        def calibrate(model, data_loader):
            model.eval()
            with torch.no_grad():
                for image in data_loader:
                    model(image)
        prepared_model = _prepare_fx(self.model, qconfig_mapping=qconfig_mapping, example_inputs=input)
        print("calibrate...")
        calibrate(prepared_model, [input])
        print("quantize")
        quantized_model = _convert_fx(prepared_model, is_reference=False, is_decomposed=True)        
        
        print("quantized model fx graph: ")
        print(quantized_model.code)
        print(quantized_model.state_dict())
        self.model = quantized_model


def main():
    # device = xm.xla_device()
    device = torch.device("cpu")
    input = torch.rand((1,2,2))

    model_quant = ModelQuant(MODELPATH).to(device)
    print("outputs before quant on cpu: ", model_quant(input))
    model_quant.quantize(input)

    print(model_quant)
    # print("jit save")
    # jit_traced_model = torch.jit.trace(llama_quant, input_ids)
    # torch.jit.save(jit_traced_model, "./traced_model.pth")
    
    # print("inference")
    # load_model = torch.jit.load("./traced_model.pth")
    # outputs = load_model(input_ids)
    # print("outputs: ", outputs)
    output_cpu = model_quant(input)
    
    print("inference")
    device = xm.xla_device()
    model_quant = model_quant.to(device)
    outputs = model_quant(input.to(device))

    print("outputs: ", outputs)
    print("outputs on cpu: ", output_cpu)



if __name__ == '__main__':
    main()
