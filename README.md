
# Tổng hợp mã nguồn cho con robot trồng cây.

## Rule
- Không tự tiện sửa code của người khác
- Nếu có thắc mắc, hỏi trực tiếp người viết hoặc báo với người viết về chỉnh sửa nếu gấp quá.
- Không đùa giỡn, quậy phá.
- Phải chú thích kỹ và viết code clear cho người khác đọc.
- Chịu trực tiếp trách nhiệm trước hành động của mình nên suy nghĩ và tra cứu kỹ trước khi làm gì.

## How it work
- Thư mục robot brain là hàm chính, nếu muốn thêm feather vào thì nhớ tạo một file mới hoặc tạo một brand mới và ko merge với brand chính.
- Cắm các dây theo thứ tự và nạp code vào chạy.
- Lưu ý : Nhớ rút cổng nhận ps2 trên board khi nạp code cũng như khi reset code.

## Librarys need:
- https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library ( Adafruit PCA9685 ).
- https://github.com/makerviet/Arduino-PS2X-ESP32-Makerbot (PS2)
- https://github.com/cvmanjoo/RTC (time)
- https://github.com/adafruit/Adafruit_BusIO (Adafruit BusIO)
