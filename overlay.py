from PIL import Image, ImageDraw

class Size(object):
    def __init__(self, bottom, height, width, color, middle = False):
        self.bottom = bottom
        self.height = height
        self.width = width
        self.color = color
        self.middle = middle

imageWidth = 1280
imageHeight = 720
lineWidth = 10
image = Image.new("RGBA", (imageWidth, imageHeight), (0, 0, 0, 0))

draw = ImageDraw.Draw(image)

colors = [
    (255, 0,   0, 255), # Red
    (255, 255, 0, 255), # Yellow
    (0,   255, 0, 255) # Green
]
stepfactor = 2
stepsize = imageHeight / (len(colors) * stepfactor)
sizes = []

for i in range(len(colors)):
    sizes.append(Size(
        imageHeight - lineWidth - stepsize * i * stepfactor, # bottom
        stepsize,                                            # height
        stepsize,                                            # width
        colors[i],                                           # color
        i == 0))                                             # middle

def path(points, fill, width):
    offset = (width - 1) / 2

    for i in range(len(points)-1):
        draw.line(
            (points[0 + i], points[1 + i]),
            fill = fill,
            width = width)

        if i > 0:
            draw.ellipse(
                (points[i][0] - offset,
                points[i][1] - offset,
                points[i][0] + offset,
                points[i][1] + offset),
                fill = fill)

offset = lineWidth / 2
for size in sizes:
    # left
    path(
        ((0 + offset, size.bottom),
        (0 + offset,  size.bottom - size.height),
        (size.width - 1 - offset,     size.bottom - size.height)),
        fill = size.color,
        width = lineWidth)

    # right
    path(
        ((imageWidth - offset,              size.bottom),
        (imageWidth - offset,              size.bottom - size.height),
        (imageWidth - size.width - offset, size.bottom - size.height)),
        fill = size.color,
        width = lineWidth)

    # middle
    if size.middle:
        # horizontal
        path(
            (((imageWidth - lineWidth - offset) / 2 + size.height / 2, size.bottom - size.height),
            ((imageWidth - lineWidth - offset) / 2 - size.height / 2, size.bottom - size.height)),
            fill = size.color,
            width = lineWidth)

        # vertical
        path(
            (((imageWidth - lineWidth - offset) / 2, size.bottom - size.height - size.height / 2),
            ((imageWidth - lineWidth - offset) / 2, size.bottom - size.height + size.height / 2)),
            fill = size.color,
            width = lineWidth)

image.save("overlay.png", "PNG")
