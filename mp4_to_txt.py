import cv2
import argparse
from PIL import Image

def video2text(input_path, output_path, scale=0.1, max_frames=None, frame_interval=1):
    """
    将视频转换为文本格式（显示压缩后尺寸）
    
   参数：
    - input_path: 输入视频路径
    - output_path: 输出文本路径
    - scale: 压缩比例 (0-1)
    - max_frames: 最大提取帧数 (None表示无限制)
    - frame_interval: 抽帧间隔 (每N帧取1帧)
    """
    cap = cv2.VideoCapture(input_path)
    if not cap.isOpened():
        raise ValueError("无法打开视频文件")

    total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    fps = cap.get(cv2.CAP_PROP_FPS)
    processed = 0
    with open(output_path, 'w') as f:
        frame_idx = 0
        while cap.isOpened():
            ret, frame = cap.read()
            if not ret or (max_frames and processed >= max_frames):
                break

            if frame_idx % frame_interval == 0:
                # 转换为PIL格式并压缩
                pil_img = Image.fromarray(cv2.cvtColor(frame, cv2.COLOR_BGR2RGB))
                new_size = (int(pil_img.width * scale), int(pil_img.height * scale))
                pil_img = pil_img.resize(new_size, Image.Resampling.LANCZOS)
                
                # 写入帧头（包含压缩后尺寸）
                f.write(f"{max_frames} {frame_interval} {new_size[0]} {new_size[1]}\n")
                
                # 写入像素数据
                for y in range(new_size[1]):
                    for x in range(new_size[0]):
                        r, g, b = pil_img.getpixel((x, y))
                        f.write(f"{r:3} {g:3} {b:3} ")  # 对齐数据
                    f.write("\n")  # 每行像素换行
                
                print(f"已处理帧 {processed}: 压缩尺寸 {new_size[0]}x{new_size[1]}")
                processed += 1

            frame_idx += 1

    cap.release()
    print(f"转换完成！总帧数：{processed}/{total_frames}")

if __name__ == "__main__":
    # 示例参数（可修改）
    video2text(
        input_path= r"C:\Users\1\Desktop\videos_based_on_cmd\test.mp4" , 
        output_path= r"C:\Users\1\Desktop\videos_based_on_cmd\样例输入输出output.txt",
        scale=0.25,      # 压缩为原尺寸的20%
        max_frames=1393,  # 最多处理50帧
        frame_interval=3 # 每10帧取1帧
    )
