from argparse import ArgumentParser
from pathlib import Path

import torch

from transformers import BertConfig, BertModel, BertTokenizer

import onnx
import onnxsim


def get_args():
    parser = ArgumentParser()
    parser.add_argument(
        "--num_layers",
        type=int,
        default=1,
    )
    parser.add_argument(
        "--batch_size",
        type=int,
        default=1,
    )
    parser.add_argument(
        "--seq_len",
        type=int,
        default=128,
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=Path("bert-base.onnx"),
    )
    return parser.parse_args()


if __name__ == "__main__":
    args = get_args()
    num_layers: int = args.num_layers
    batch_size: int = args.batch_size
    seq_len: int = args.seq_len
    export_path: Path = args.output
    export_path.parent.mkdir(parents=True, exist_ok=True)

    bert_config = BertConfig(
        num_hidden_layers=num_layers
    )
    # tokenizer = BertTokenizer.from_pretrained("google-bert/bert-base-uncased")
    bert_model = BertModel(bert_config).eval()

    # dummy inputs.
    input_ids = torch.randint(
        low=0,
        high=bert_config.vocab_size,
        size=(batch_size, seq_len),
        dtype=torch.int64,
    )
    attention_mask = torch.ones(
        size=(batch_size, seq_len),
        dtype=torch.float32,
    )

    # export
    with torch.no_grad():
        torch.onnx.export(
            model=bert_model,
            args=(input_ids, attention_mask),
            f=str(export_path),
            input_names=["input_ids", "attention_mask"],
            output_names=["last_hidden_state", "logits"],
            dynamic_axes={
                "input_ids": {
                    0: "batch_size",
                    1: "sequence",
                },
                "attention_mask": {
                    0: "batch_size",
                    1: "sequence",
                },
                "last_hidden_state": {
                    0: "batch_size",
                    1: "sequence",
                },
                "logits": {
                    0: "batch_size",
                }
            },
            do_constant_folding=True,
            opset_version=17,
        )
    bert_onnx = onnx.load(export_path)

    # ONNX simplify.
    bert_onnx_simplified, is_ok = onnxsim.simplify(
        model=bert_onnx,
        overwrite_input_shapes={
            "input_ids": (batch_size, seq_len),
            "attention_mask": (batch_size, seq_len),
        }
    )
    assert is_ok, "Failed to simplify ONNX model."

    onnx.save(proto=bert_onnx_simplified, f=export_path)
    print(f"Exported ONNX model saved to {export_path}.")
