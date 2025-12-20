from ultralytics import YOLO
import cv2

# Load model
model = YOLO("yolov8n.pt")

# Buka webcam
cap = cv2.VideoCapture(0)

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    # Deteksi objek
    results = model(frame, conf=0.5)

    for r in results:
        boxes = r.boxes

        if boxes is not None:
            for box in boxes:
                # Koordinat bounding box
                x1, y1, x2, y2 = map(int, box.xyxy[0])

                # Confidence & class
                conf = float(box.conf[0])
                cls = int(box.cls[0])
                label = f"{model.names[cls]} {conf:.2f}"

                # Gambar bounding box
                cv2.rectangle(frame, (x1, y1), (x2, y2),
                              (0, 255, 0), 2)

                # Tampilkan label
                cv2.putText(frame, label, (x1, y1 - 10),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.5,
                            (0, 255, 0), 2)

    cv2.imshow("YOLOv8 Real-Time Detection", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
