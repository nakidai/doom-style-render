from PIL import Image, ImageFont, ImageDraw

CHAR_SIZE = 16

image = Image.new("RGBA", (CHAR_SIZE * 256, CHAR_SIZE), (0, 0, 0))

chars = "".join([chr(i) for i in range(0, 255)])
font = ImageFont.truetype("./res/Sans.ttf", CHAR_SIZE)

for i, char in enumerate(chars):
    char_img = Image.new("RGBA", (CHAR_SIZE, CHAR_SIZE), (0, 0, 0))

    d_usr = ImageDraw.Draw(char_img)
    d_usr = d_usr.text((2, -2), char, (245, 245, 245), font=font, spacing=0)

    image.paste(char_img, [i * CHAR_SIZE, 0])

image.save("res/chars.bmp")