from pathlib import Path
from torch import nn
from transformers import CLIPTextModel
from diffusers import UNet2DConditionModel, AutoencoderKL


def CLIP_demo():
    input_tensor = torch.ones(size=(3, 7), dtype=torch.int32)
    inputs = {
        "input_ids": input_tensor,
    }

    model_config = CLIPTextModel.config_class.from_pretrained(
        Path("models/stable_diffusion_v1_5/text_encoder")
    )
    model = CLIPTextModel._from_config(config=model_config)
    with torch.inference_mode():
        outputs = model(**inputs)
        print(outputs.last_hidden_state.shape)


def Unet_demo():
    input_noise_img = torch.ones(size=(3, 4, 64, 64))  # RGBA img
    input_timestep = torch.ones(3)
    input_text = torch.ones(size=(3, 7, 768))
    inputs = {
        "sample": input_noise_img,
        "timestep": input_timestep,
        "encoder_hidden_states": input_text,
    }

    model_config = UNet2DConditionModel.load_config(
        Path("models/stable_diffusion_v1_5/unet")
    )
    model = UNet2DConditionModel.from_config(config=model_config)
    with torch.inference_mode():
        outputs = model(**inputs)
        print(outputs.sample)


def VAE_demo():
    input_latent_img = torch.ones(size=(3, 4, 64, 64))

    model_config = AutoencoderKL.load_config(Path("models/stable_diffusion_v1_5/vae"))
    model = AutoencoderKL.from_config(config=model_config)
    with torch.inference_mode():
        outputs = model.decoder(input_latent_img)
        print(outputs.sample)


if __name__ == "__main__":
    CLIP_demo()
