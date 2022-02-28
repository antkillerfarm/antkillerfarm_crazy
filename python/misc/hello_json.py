import json

fpath = "resnet.json"

with open(fpath) as f:
    json_dom = json.load(f)

layers = json_dom["config"]["layers"]

class_count = {}
for l in layers:
    if l["class_name"] in class_count:
        class_count[l["class_name"]] = class_count[l["class_name"]] + 1
    else:
        class_count[l["class_name"]] = 1

print(class_count)
