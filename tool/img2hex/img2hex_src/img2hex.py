import sys
from PyQt5.QtWidgets import (QApplication, QMainWindow, QPushButton, QColorDialog, QGraphicsScene, QGraphicsView,
                             QGraphicsRectItem, QFileDialog, QVBoxLayout, QWidget, QMessageBox, QLabel, QDialog,
                             QSpinBox, QScrollArea, QCheckBox, QHBoxLayout, QShortcut)
from PyQt5.QtCore import Qt, QPoint, QEvent
from PyQt5.QtGui import QBrush, QColor, QImage, QPainter, QPixmap, QKeySequence

class PreviewDialog(QDialog):
    def __init__(self, image):
        super().__init__()
        self.setWindowTitle("Xem trước hình ảnh")
        self.setFixedSize(800, 600) 

        self.image = image
        self.scale_factor = 1.0
        self.last_mouse_position = QPoint(0, 0)
        self.is_dragging = False
        self.image_label = QLabel(self)
        self.image_label.setAlignment(Qt.AlignCenter)
        self.image_label.setPixmap(QPixmap.fromImage(image))

        layout = QVBoxLayout()
        layout.addWidget(self.image_label)
        self.setLayout(layout)

    def wheelEvent(self, event):

        if event.angleDelta().y() > 0:  
            self.scale_factor *= 1.1
        else: 
            self.scale_factor /= 1.1

        self.update_image_label()

    def update_image_label(self):

        scaled_image = self.image.scaled(self.image.size() * self.scale_factor, Qt.KeepAspectRatio, Qt.SmoothTransformation)
        self.image_label.setPixmap(QPixmap.fromImage(scaled_image))
        self.image_label.resize(scaled_image.size())

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.is_dragging = True
            self.last_mouse_position = event.pos()

    def mouseMoveEvent(self, event):
        if self.is_dragging:
            delta = event.pos() - self.last_mouse_position
            self.image_label.move(self.image_label.pos() + delta)
            self.last_mouse_position = event.pos()

    def mouseReleaseEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.is_dragging = False

class PixelArtDialog(QDialog):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.setWindowTitle('Pixel Art Editor')
        self.setGeometry(100, 100, 500, 400)
        self.setWindowFlags(self.windowFlags() | Qt.WindowMinMaxButtonsHint)
        self.history = []
        undo_shortcut = QShortcut(QKeySequence("Ctrl+Z"), self)
        undo_shortcut.activated.connect(self.undo)
        self.pixel_size = 10  
        self.grid_width = 50  
        self.grid_height = 50  
        self.is_painting = False  
        self.erase_mode = False  
        self.width_input = QSpinBox(self)
        self.width_input.setRange(1, 1000)
        self.width_input.setValue(self.grid_width)
        self.width_input.setPrefix('Width: ')

        self.height_input = QSpinBox(self)
        self.height_input.setRange(1, 1000)
        self.height_input.setValue(self.grid_height)
        self.height_input.setPrefix('Height: ')

        self.apply_size_button = QPushButton('Áp dụng kích thước', self)
        self.apply_size_button.clicked.connect(self.apply_size)

        self.open_image_button = QPushButton('Mở hình ảnh', self)
        self.open_image_button.clicked.connect(self.open_image)
        
        button_config_layout = QHBoxLayout()
        button_config_layout.addWidget(self.apply_size_button)
        button_config_layout.addWidget(self.open_image_button)


        self.scene = QGraphicsScene()
        self.pixel_matrix = []
        self.create_pixel_grid()

        self.view = QGraphicsView(self.scene)
        self.view.setFixedSize(self.grid_width * self.pixel_size + 1, self.grid_height * self.pixel_size + 1)

      
        self.scroll_area = QScrollArea(self)
        self.scroll_area.setWidget(self.view)
        self.scroll_area.setWidgetResizable(True)
        self.undo_button = QPushButton('Undo', self)
        self.undo_button.clicked.connect(self.undo)
        self.color_button = QPushButton('Chọn màu', self)
        self.color_button.clicked.connect(self.choose_color)

        self.save_design_button = QPushButton('Lưu thiết kế', self)
        self.save_design_button.clicked.connect(self.save_pixel_design)

        self.zoom_in_button = QPushButton('Phóng to', self)
        self.zoom_in_button.clicked.connect(self.zoom_in)

        self.zoom_out_button = QPushButton('Thu nhỏ', self)
        self.zoom_out_button.clicked.connect(self.zoom_out)

        self.preview_button = QPushButton("Xem trước")
        self.preview_button.clicked.connect(self.preview_image)


        
        self.coordinate_checkbox = QCheckBox('Xem tọa độ', self)
        self.coordinate_checkbox.setChecked(False)

        button_layout = QHBoxLayout()

        button_layout.addWidget(self.color_button)
        button_layout.addWidget(self.undo_button)
        button_layout.addWidget(self.save_design_button)
        button_layout.addWidget(self.zoom_in_button)
        button_layout.addWidget(self.zoom_out_button)
        button_layout.addWidget(self.preview_button)

        layout = QVBoxLayout()
        layout.addWidget(self.width_input)
        layout.addWidget(self.height_input)
        layout.addLayout(button_config_layout) 
        #layout.addWidget(self.apply_size_button)
        layout.addWidget(self.scroll_area)
        layout.addLayout(button_layout) 
        layout.addWidget(self.coordinate_checkbox)
        self.setLayout(layout)
        self.selected_color = QColor(255, 0, 0)

        self.view.viewport().installEventFilter(self)

        self.coordinates_label = QLabel(self)
        layout.addWidget(self.coordinates_label)
        self.coordinates_label.setAlignment(Qt.AlignBottom | Qt.AlignLeft)

    def create_pixel_grid(self):
        self.pixel_matrix = []
        self.scene.clear()
        for y in range(self.grid_height):
            row = []
            for x in range(self.grid_width):
                rect = QGraphicsRectItem(x * self.pixel_size, y * self.pixel_size, self.pixel_size, self.pixel_size)
                rect.setBrush(QBrush(Qt.white))
                rect.setPen(Qt.black)
                rect.setFlag(QGraphicsRectItem.ItemIsSelectable)
                self.scene.addItem(rect)
                row.append(rect)
            self.pixel_matrix.append(row)

    def open_image(self):
        file_path, _ = QFileDialog.getOpenFileName(self, 'Chọn hình ảnh', '', 'Images (*.png *.xpm *.jpg *.bmp)')
        if file_path:
            image = QImage(file_path)
            if image.isNull():
                QMessageBox.warning(self, 'Lỗi', 'Không thể mở hình ảnh.')
                return
    
            self.pixel_size = 1 
            self.grid_width = image.width() // self.pixel_size
            self.grid_height = image.height() // self.pixel_size
            self.width_input.setValue(self.grid_width)
            self.height_input.setValue(self.grid_height)
            self.apply_size() 

    

            self.create_pixel_grid()
    
            for y in range(min(self.grid_height, image.height())):
                for x in range(min(self.grid_width, image.width())):
                    color = image.pixelColor(x * self.pixel_size, y * self.pixel_size)
                    self.pixel_matrix[y][x].setBrush(QBrush(color))
    
            self.view.update() 




    def apply_size(self):
        self.grid_width = self.width_input.value()
        self.grid_height = self.height_input.value()
        self.view.setFixedSize(self.grid_width * self.pixel_size + 2, self.grid_height * self.pixel_size + 2)
        self.create_pixel_grid()

    def choose_color(self):
        color = QColorDialog.getColor()
        if color.isValid():
            self.selected_color = color

    def eventFilter(self, source, event):
        if event.type() == event.Wheel:
            if event.modifiers() == Qt.ControlModifier:
                if event.angleDelta().y() > 0:
                    self.zoom_in()
                else:
                    self.zoom_out()
                return True  
    
        elif event.type() == QEvent.MouseButtonPress and source is self.view.viewport():
            pos = self.view.mapToScene(event.pos())
            self.save_state_to_history()
    
            self.is_drawing = True
            
            for y, row in enumerate(self.pixel_matrix):
                for x, rect in enumerate(row):
                    if rect.contains(pos):
                        if event.button() == Qt.LeftButton:
                            if not self.coordinate_checkbox.isChecked():
                                rect.setBrush(QBrush(self.selected_color))
                        elif event.button() == Qt.RightButton:
                            rect.setBrush(QBrush(Qt.white))
                        
                        if self.coordinate_checkbox.isChecked():
                            self.coordinates_label.setText(f'Tọa độ: ({x}, {y})')
            
            return True  
    
        elif event.type() == QEvent.MouseMove and self.is_drawing:
            pos = self.view.mapToScene(event.pos())
            
            for y, row in enumerate(self.pixel_matrix):
                for x, rect in enumerate(row):
                    if rect.contains(pos):
                        if event.buttons() == Qt.LeftButton:
                            if not self.coordinate_checkbox.isChecked():
                                rect.setBrush(QBrush(self.selected_color))
                        elif event.buttons() == Qt.RightButton:
                            rect.setBrush(QBrush(Qt.white))
                            
                        if self.coordinate_checkbox.isChecked():
                            self.coordinates_label.setText(f'Tọa độ: ({x}, {y})')
            
            return True  
    
        elif event.type() == QEvent.MouseButtonRelease:
            self.is_drawing = False
    
        return False


    def save_state_to_history(self):
        state = []
        for y in range(self.grid_height):
            row = []
            for x in range(self.grid_width):
                color = self.pixel_matrix[y][x].brush().color()
                row.append(color)
            state.append(row)
        self.history.append(state)

    def undo(self):
        if self.history:
      
            last_state = self.history.pop()
            
        
            for y in range(self.grid_height):
                for x in range(self.grid_width):
                    color = last_state[y][x]
                    self.pixel_matrix[y][x].setBrush(QBrush(color))
            
            self.view.update() 
    def zoom_in(self):
        self.pixel_size += 1  
        self.update_pixel_sizes() 
    
    def zoom_out(self):
        if self.pixel_size > 1:
            self.pixel_size -= 1  
            self.update_pixel_sizes()
    def update_pixel_sizes(self):
        for y in range(self.grid_height):
            for x in range(self.grid_width):
                rect = self.pixel_matrix[y][x]
                if rect:  
                    rect.setRect(x * self.pixel_size, y * self.pixel_size, self.pixel_size, self.pixel_size)
    
       
        self.view.setFixedSize(self.grid_width * self.pixel_size + 1, self.grid_height * self.pixel_size + 1)



    def save_pixel_design(self):
        save_path, _ = QFileDialog.getSaveFileName(self, 'Lưu thiết kế pixel', '', 'PNG Files (*.png);;Hex Files (*.txt)')
        if save_path:
            if save_path.endswith('.png'):
                self.save_as_image(save_path)
            elif save_path.endswith('.txt'):
                self.save_as_hex(save_path)

    def save_as_image(self, path):
        image = QImage(self.grid_width, self.grid_height, QImage.Format_RGB32)
        painter = QPainter(image)

        for y in range(self.grid_height):
            for x in range(self.grid_width):
                rect = self.pixel_matrix[y][x]
                color = rect.brush().color()
                painter.fillRect(x , y , self.pixel_size, self.pixel_size, color)

        painter.end()
        image.save(path)
        QMessageBox.information(self, 'Thông báo', f'Thiết kế đã được lưu thành hình ảnh: {path}')

    def save_as_hex(self, path):
        hex_array = []
        for y in range(self.grid_height):
            for x in range(self.grid_width):
                rect = self.pixel_matrix[y][x]
                color = rect.brush().color()
                r, g, b = color.red(), color.green(), color.blue()
                rgb_565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
                hex_array.append(f"0x{rgb_565:04X}")

        with open(path, 'w') as file:
            for i in range(0, len(hex_array), self.grid_width):
                file.write(", ".join(hex_array[i:i + self.grid_width]) + ",\n")

        QMessageBox.information(self, 'Thông báo', f'Thiết kế đã được lưu thành mã hex: {path}')
    

    def preview_image(self):
        image = QImage(self.grid_width, self.grid_height, QImage.Format_RGB32)
        painter = QPainter(image)

        for y in range(self.grid_height):
            for x in range(self.grid_width):
                rect = self.pixel_matrix[y][x]
                color = rect.brush().color() 
                painter.fillRect(x, y, self.pixel_size, self.pixel_size, color)

        painter.end()

        preview_dialog = PreviewDialog(image)
        preview_dialog.exec_()
class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.setWindowTitle('Image to Hex Converter')
        self.setGeometry(100, 100, 400, 300)

        self.label = QLabel('Chọn hình ảnh để chuyển sang mã hex:', self)
        self.label.move(20, 20)

        self.open_button = QPushButton('Chọn hình ảnh', self)
        self.open_button.move(20, 60)
        self.open_button.clicked.connect(self.open_image)

        self.save_button = QPushButton('Lưu mã hex', self)
        self.save_button.move(150, 60)
        self.save_button.clicked.connect(self.save_hex)

        self.pixel_editor_button = QPushButton('Chọn pixel', self)
        self.pixel_editor_button.move(20, 120)
        self.pixel_editor_button.clicked.connect(self.open_pixel_editor)

        self.setCentralWidget(QWidget(self))
        self.layout = QVBoxLayout()
        self.centralWidget().setLayout(self.layout)

        self.layout.addWidget(self.label)
        self.layout.addWidget(self.open_button)
        self.layout.addWidget(self.save_button)
        self.layout.addWidget(self.pixel_editor_button)

        self.image = None
        self.hex_data = []

    def open_image(self):
        file_name, _ = QFileDialog.getOpenFileName(self, 'Chọn hình ảnh', '', 'Image Files (*.png *.jpg *.bmp)')
        if file_name:
            self.image = QPixmap(file_name)
            self.label.setPixmap(self.image)  

    def convert_image_to_hex(self):
        if self.image is not None:
            self.hex_data = []
            for y in range(self.image.height()):
                row = []
                for x in range(self.image.width()):
                    color = self.image.toImage().pixelColor(x, y) 
                    r, g, b = color.red(), color.green(), color.blue()
                    rgb_565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
                    row.append(f"0x{rgb_565:04X}")
                self.hex_data.append(row)

            QMessageBox.information(self, 'Thông báo', 'Hình ảnh đã được chuyển thành mã hex.')

    def save_hex(self):
        self.convert_image_to_hex()
        if not self.hex_data:
            QMessageBox.warning(self, 'Cảnh báo', 'Chưa có dữ liệu mã hex để lưu.')
            return

        save_path, _ = QFileDialog.getSaveFileName(self, 'Lưu mã hex', '', 'Hex Files (*.txt)')

        if save_path:
            
            with open(save_path, 'w') as file:
                file.write("const uint16_t epd_bitmap_images [] PROGMEM = {")
                for row in self.hex_data:
                    file.write(", ".join(row) + ",\n")
                file.write("};")
            QMessageBox.information(self, 'Thông báo', f'Mã hex đã được lưu thành: {save_path}')

    def open_pixel_editor(self):
        self.pixel_art_dialog = PixelArtDialog()
        self.pixel_art_dialog.show()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    mainWin = MainWindow()
    mainWin.show()
    sys.exit(app.exec_())
