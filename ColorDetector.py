import cv2
import numpy as np
import math

class ColorDetector:
    def __init__(self):
        self.last_green_center = None
        self.angle_history = []

    def create_mask(self, hsv, lower_bound, upper_bound):
        kernel = np.ones((5, 5), np.uint8)
        mask = cv2.inRange(hsv, lower_bound, upper_bound)
        return cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)

    def find_largest_contour(self, mask):
        contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        if contours:
            return max(contours, key=cv2.contourArea)
        return None

    def calculate_angle(self, line_vec, green_vec):
        angle = math.degrees(math.atan2(
            line_vec[0] * green_vec[1] - line_vec[1] * green_vec[0],
            line_vec[0] * green_vec[0] + line_vec[1] * green_vec[1]
        ))
        return angle

    def detect_colors(self, frame):
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

        lower_red, upper_red = np.array([150, 100, 100]), np.array([165, 255, 255])
        lower_blue, upper_blue = np.array([90, 100, 100]), np.array([130, 255, 255])
        lower_green, upper_green = np.array([40, 100, 100]), np.array([85, 255, 255])

        mask_red = self.create_mask(hsv, lower_red, upper_red)
        mask_blue = self.create_mask(hsv, lower_blue, upper_blue)
        mask_green = self.create_mask(hsv, lower_green, upper_green)

        red_contour = self.find_largest_contour(mask_red)
        blue_contour = self.find_largest_contour(mask_blue)
        green_contour = self.find_largest_contour(mask_green)

        min_area = 500
        if red_contour is not None and cv2.contourArea(red_contour) < min_area:
            red_contour = None
        if blue_contour is not None and cv2.contourArea(blue_contour) < min_area:
            blue_contour = None

        current_green_center = None
        if green_contour is not None:
            M = cv2.moments(green_contour)
            current_green_center = (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"]))
            self.last_green_center = current_green_center

        if red_contour is not None and blue_contour is not None:
            M_red = cv2.moments(red_contour)
            red_center = (int(M_red["m10"] / M_red["m00"]), int(M_red["m01"] / M_red["m00"]))

            M_blue = cv2.moments(blue_contour)
            blue_center = (int(M_blue["m10"] / M_blue["m00"]), int(M_blue["m01"] / M_blue["m00"]))

            cv2.line(frame, red_center, blue_center, (255, 255, 255), 2)

            green_center = current_green_center if current_green_center is not None else self.last_green_center
            if green_center is not None:
                line_vec = np.array([blue_center[0] - red_center[0], blue_center[1] - red_center[1]])
                green_vec = np.array([green_center[0] - red_center[0], green_center[1] - red_center[1]])

                angle = self.calculate_angle(line_vec, green_vec)

                self.angle_history.append(angle)
                if len(self.angle_history) > 5:
                    self.angle_history.pop(0)
                smooth_angle = np.median(self.angle_history)

                cv2.putText(frame, str(round(abs(smooth_angle),2)), (10, 30),
                            cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)

        if red_contour is not None:
            cv2.drawContours(frame, [red_contour], -1, (0, 0, 255), 2)
        if blue_contour is not None:
            cv2.drawContours(frame, [blue_contour], -1, (255, 0, 0), 2)
        if green_contour is not None:
            x, y, w, h = cv2.boundingRect(green_contour)
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)

        return frame

cap = cv2.VideoCapture("output_1080x1920.mp4")

detector = ColorDetector()

while True:
    ret, frame = cap.read()
    if not ret:
        break

    processed_frame = detector.detect_colors(frame)
    cv2.imshow("Color Detection", processed_frame)
    cv2.waitKey(1)

cap.release()
cv2.destroyAllWindows()
