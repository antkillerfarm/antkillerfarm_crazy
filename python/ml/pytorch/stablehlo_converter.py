from pathlib import Path
from mlir import ir
from mlir.dialects import stablehlo

def convert_mlirbc_to_milr(module_name: str):
    module_path = Path(f"{module_name}.mlirbc")
    out_path = Path(f"{module_name}.mlir")
    with ir.Context() as context:
        stablehlo.register_dialect(context)
        with open(module_path, mode="rb") as f:
            stablehlo_module: ir.Module = stablehlo.deserialize_portable_artifact(
                context,
                f.read()
            )
            with open(out_path, mode="w") as f:
                stablehlo_asm = stablehlo_module.operation.get_asm(
                    large_elements_limit=20
                )
                f.write(stablehlo_asm)

if __name__ == "__main__":
    convert_mlirbc_to_milr("/home/vsi/aigc_profiler/output/profiles/text_encoder_1_77_256_spmd")
