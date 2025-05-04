
'''
　androidのcliで/dev/video0からカメラの映像を取得する。これにより、androidにssh接続することで、安定した制御ができる。
'''


import cv2

cap = cv2.VideoCapture(0)

if not cap.isOpened():
    print("カメラが開けません")
    exit()

ret, frame = cap.read()

if ret:
    cv2.imwrite("captured_image.jpg", frame)
    print("画像を保存しました")

cap.release()

