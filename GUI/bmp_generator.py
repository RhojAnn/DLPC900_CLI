from PIL import Image

def generate_bmp(row,column,grid_size=10,grid_line_thickness=2):
    CANVAS_W = 2048
    CANVAS_H = 1200
    SQUARE = 1200
    FILENAME = "row_pattern/current.bmp"

    pad_x = (CANVAS_W - SQUARE) // 2

    N = grid_size  # square grid
    cell_size = SQUARE // N  # auto-fit

    grid_w = N * cell_size
    grid_h = N * cell_size

    # Center the grid inside the square
    offset_x = pad_x + (SQUARE - grid_w) // 2
    offset_y = (SQUARE - grid_h) // 2

    img = Image.new("1", (CANVAS_W, CANVAS_H), 1)  # start white
    pixels = img.load()

    for y in range(CANVAS_H):
        for x in range(CANVAS_W):

            # Black square
            if pad_x <= x < pad_x + SQUARE:
                pixels[x, y] = 0

                if offset_x <= x < offset_x + grid_w and offset_y <= y < offset_y + grid_h:
                    gx = (x - offset_x) // cell_size
                    gy = (y - offset_y) // cell_size

                    # White selected cell
                    if gx == column and gy == row:
                        pixels[x, y] = 1

                    # Gridlines
                    if (x - offset_x) % cell_size < grid_line_thickness:
                        pixels[x, y] = 1
                    if (y - offset_y) % cell_size < grid_line_thickness:
                        pixels[x, y] = 1
    
    img.save(FILENAME, "BMP")
    print(f"Saved BMP: {FILENAME}")

if __name__ == "__main__":
    generate_bmp(
        row=3,
        column=6,
        grid_size=20,
        grid_line_thickness=2,
    )
