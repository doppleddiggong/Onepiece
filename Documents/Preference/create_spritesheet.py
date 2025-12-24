import os
from PIL import Image

# --- 설정값 ---
SOURCE_DIR = "."  # 현재 폴더
OUTPUT_NAME = "T_Character_SpriteSheet_Cropped.png"
CROP_SIZE = 720  # 중앙에서 720x720 크롭
GRID_SIZE = (8, 5)  # 8열 5행

def center_crop(img, crop_size):
    """이미지 중앙에서 crop_size x crop_size 영역 추출"""
    width, height = img.size
    left = (width - crop_size) // 2
    top = (height - crop_size) // 2
    right = left + crop_size
    bottom = top + crop_size
    return img.crop((left, top, right, bottom))

def main():
    # 1. 파일 목록 가져오기 및 정렬 (ezgif-frame 파일만 선택)
    all_files = os.listdir(SOURCE_DIR)
    files = sorted([f for f in all_files if f.startswith('ezgif-frame') and f.lower().endswith('.png')])[:40]

    print(f"처리할 이미지 파일 개수: {len(files)}")

    # 2. 결과물 캔버스 생성 (5760 x 3600)
    canvas_width = CROP_SIZE * GRID_SIZE[0]
    canvas_height = CROP_SIZE * GRID_SIZE[1]
    sheet = Image.new("RGB", (canvas_width, canvas_height), (255, 255, 255))

    for index, file_name in enumerate(files):
        img_path = os.path.join(SOURCE_DIR, file_name)
        print(f"처리 중: {file_name} ({index + 1}/{len(files)})")

        with Image.open(img_path) as img:
            # 중앙 720x720 크롭
            cropped_img = center_crop(img, CROP_SIZE)

            # 그리드 좌표 계산
            x = (index % GRID_SIZE[0]) * CROP_SIZE
            y = (index // GRID_SIZE[0]) * CROP_SIZE

            # 캔버스에 붙이기
            sheet.paste(cropped_img, (x, y))

    # 3. 최종 저장
    sheet.save(OUTPUT_NAME, "PNG")
    print(f"\n완료! 파일 저장됨: {OUTPUT_NAME}")
    print(f"크기: {canvas_width}x{canvas_height} (4096x2560)")

if __name__ == "__main__":
    main()
