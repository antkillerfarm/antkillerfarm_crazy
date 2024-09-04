from pathlib import Path
from transformers import CLIPTextModel
from torch import nn


def CLIP_demo():
    input_tensor = torch.ones(size=(3, 7), dtype=torch.int32)
    inputs = {
        "input_ids": input_tensor,
    }

    model_config = CLIPTextModel.config_class.from_pretrained(
        Path("models/stable_diffusion_v1_5/text_encoder")
    )
    model = CLIPTextModel._from_config(config=model_config)
    with torch.no_grad():
        outputs = model(**inputs)
        print(outputs.last_hidden_state.shape)


if __name__ == "__main__":
    CLIP_demo()
