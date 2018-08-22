from PIL import Image

im = Image.open("D:/my/work/0.jpg")
im1 = im.resize((224,224))
im1.save("D:/my/work/1.jpg")
