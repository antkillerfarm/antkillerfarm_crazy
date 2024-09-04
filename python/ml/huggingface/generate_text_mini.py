import transformers
import torch
import json
from transformers import (
    AutoTokenizer,
    AutoModelForCausalLM,
    LlamaForCausalLM,
    pipeline,
    TextStreamer,
)
import torch_xla
import torch_xla.core.xla_model as xm

MODELPATH = "./ModelFiles/llama2-7b"


def cut_model(model_path):
    model = LlamaForCausalLM.from_pretrained(model_path, trust_remote_code=True)
    print("### model ###", model)
    # part_model = model.model.layers[0:1]
    # part_model0 = torch.nn.Sequential(
    #         model.model.embed_tokens,
    #         model.model.layers[0:1],
    #         model.model.norm
    #     )
    # part_model1 = torch.nn.Sequential(
    #         part_model0,
    #         model.lm_head
    #     )
    # print("### part_model ###", part_model1)
    model.model.layers = model.model.layers[0:1]
    # torch.save(model, 'model.pth')
    torch.save(model.state_dict(), "pytorch_model.bin")


def main(prompt, model_path):
    device = xm.xla_device()
    # model = AutoModelForCausalLM.from_pretrained(model_path,
    #                                              device_map='auto',
    #                                              offload_folder='./ModelFiles/off_load')

    model = LlamaForCausalLM.from_pretrained(
        "./ModelFiles/llama2-7b_1", trust_remote_code=True
    )
    # model.model.layers = model.model.layers[0:1]
    model.eval()
    print("### model ###", model)
    model = model.half()
    print("### model to device ###")
    model.to(device)
    print("### model compile ###")
    #    model = torch.compile(model, backend='openxla')
    tokenizer = AutoTokenizer.from_pretrained(model_path)

    # system_prompt = 'You are a helpful aasistant that provides accurate and concise response'

    #    B_INST, E_INST = '### i:', '### can:'
    B_INST, E_INST = "### Human:", "### Assistant:"
    prompt = f"{B_INST} {prompt.strip()} {E_INST} \n\n"
    inputs = tokenizer([prompt], return_tensors="pt").to(device)
    print("intpus is : ", inputs)
    del inputs["token_type_ids"]
    streamer = TextStreamer(tokenizer)
    result = model.generate(**inputs, streamer=streamer, max_new_tokens=5)
    print("result is ", result)
    # res1 = tokenizer.convert_ids_to_tokens(result[0])
    # print("result text is ", res1)


if __name__ == "__main__":
    # prompt = 'Life is a movie beacuse'
    prompt = "hello"
    main(prompt, MODELPATH)
    # cut_model(MODELPATH)
