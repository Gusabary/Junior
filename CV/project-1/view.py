from PyQt5.QtWidgets import QApplication
from PyQt5.QtWidgets import QMainWindow
from PyQt5.QtWidgets import QWidget, QLabel
from PyQt5.QtWidgets import QPushButton, QSlider
from PyQt5.QtWidgets import QVBoxLayout, QHBoxLayout
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QPixmap, QImage
import numpy as np

class View(QMainWindow):

    def __init__(self, imagePath):
        super(View, self).__init__()
        self.imagePath = imagePath

        self.setWindowTitle('CV')
        self.setFixedSize(600, 600)

        self.generalLayout = QHBoxLayout()
        self._centralWidget = QWidget(self)
        self.setCentralWidget(self._centralWidget)
        self._centralWidget.setLayout(self.generalLayout)
        
        self._createControls()
        self._createImage()

    def _createControls(self):
        self.controls = QVBoxLayout()
        self.robertsButton = QPushButton('roberts kernel convolution')
        self.prewittButton = QPushButton('prewitt kernel convolution')
        self.sobelButton = QPushButton('sobel kernel convolution')

        self.sizeSlider = QSlider(Qt.Horizontal)
        self.sizeSliderNum = QLabel('size: 2x2')
        self.sizeSlider.setMinimum(2)
        self.sizeSlider.setMaximum(7)
        self.sizeSlider.valueChanged.connect(lambda: self.sizeSliderNum.setText("size: "+str(self.sizeSlider.value())+"x"+str(self.sizeSlider.value())))
        self.sizeSliderLayout = QHBoxLayout()
        self.sizeSliderLayout.addWidget(self.sizeSlider)
        self.sizeSliderLayout.addWidget(self.sizeSliderNum)
        self.sizeWidget = QWidget()
        self.sizeWidget.setLayout(self.sizeSliderLayout)
        self.sizeWidget.setFixedHeight(40)

        self.sigmaSlider = QSlider(Qt.Horizontal)
        self.sigmaSliderNum = QLabel('sigma: 0.5')
        self.sigmaSlider.setMinimum(5)
        self.sigmaSlider.setMaximum(15)
        self.sigmaSlider.valueChanged.connect(lambda: self.sigmaSliderNum.setText("sigma: "+str(self.sigmaSlider.value()/10.)))
        self.sigmaSliderLayout = QHBoxLayout()
        self.sigmaSliderLayout.addWidget(self.sigmaSlider)
        self.sigmaSliderLayout.addWidget(self.sigmaSliderNum)
        self.sigmaWidget = QWidget()
        self.sigmaWidget.setLayout(self.sigmaSliderLayout)
        self.sigmaWidget.setFixedHeight(40)

        self.gaussianButton = QPushButton('gaussian filter')
        self.meanButton = QPushButton('mean filter')
        self.medianButton = QPushButton('median filter')

        self.controls.addWidget(self.robertsButton)
        self.controls.addWidget(self.prewittButton)
        self.controls.addWidget(self.sobelButton)
        self.controls.addWidget(self.sizeWidget)
        self.controls.addWidget(self.sigmaWidget)
        self.controls.addWidget(self.gaussianButton)
        self.controls.addWidget(self.meanButton)
        self.controls.addWidget(self.medianButton)

        self.generalLayout.addLayout(self.controls, stretch=2)
        
    def _createImage(self):
        self.image = QLabel()

        self.image.setPixmap(QPixmap(self.imagePath))
        self.generalLayout.addWidget(self.image, stretch=3, alignment=Qt.AlignCenter)

    def setImage(self, img):
        ans = QImage(img.data, img.shape[1], img.shape[0], img.shape[1], QImage.Format_Grayscale8)
        self.image.setPixmap(QPixmap.fromImage(ans))