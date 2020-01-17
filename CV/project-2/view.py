from PyQt5.QtWidgets import QApplication
from PyQt5.QtWidgets import QMainWindow
from PyQt5.QtWidgets import QWidget, QLabel
from PyQt5.QtWidgets import QPushButton, QSlider, QLineEdit
from PyQt5.QtWidgets import QVBoxLayout, QHBoxLayout, QGridLayout
from PyQt5.QtCore import Qt, QRect
from PyQt5.QtGui import QPixmap, QImage
import numpy as np

class View(QMainWindow):

    def __init__(self, imagePath):
        super(View, self).__init__()
        self.imagePath = imagePath

        self.setWindowTitle('CV')
        self.setFixedSize(800, 600)

        self.generalLayout = QHBoxLayout()
        self._centralWidget = QWidget(self)
        self.setCentralWidget(self._centralWidget)
        self._centralWidget.setLayout(self.generalLayout)
        
        self._createControls()
        self._createImage()

    def _createControls(self):
        '''homework-1 ui'''
        self.hw1Controls = QVBoxLayout()
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

        self.hw1Controls.addWidget(self.robertsButton)
        self.hw1Controls.addWidget(self.prewittButton)
        self.hw1Controls.addWidget(self.sobelButton)
        self.hw1Controls.addWidget(self.sizeWidget)
        self.hw1Controls.addWidget(self.sigmaWidget)
        self.hw1Controls.addWidget(self.gaussianButton)
        self.hw1Controls.addWidget(self.meanButton)
        self.hw1Controls.addWidget(self.medianButton)

        self.generalLayout.addLayout(self.hw1Controls, stretch=2)

        '''homework-2 ui'''
        self.hw2Controls = QVBoxLayout()

        self.SESlider = QSlider(Qt.Horizontal)
        self.SESliderNum = QLabel('SE size: 2x2')
        self.SESlider.setMinimum(2)
        self.SESlider.setMaximum(7)
        self.SESlider.valueChanged.connect(lambda: self.SESliderNum.setText("SE size: "+str(self.SESlider.value())+"x"+str(self.SESlider.value())))
        self.SESliderLayout = QHBoxLayout()
        self.SESliderLayout.addWidget(self.SESlider)
        self.SESliderLayout.addWidget(self.SESliderNum)
        self.sizeWidget = QWidget()
        self.sizeWidget.setLayout(self.SESliderLayout)
        self.sizeWidget.setFixedHeight(40)

        self.dilateButton = QPushButton('dilate')
        self.erodeButton = QPushButton('erode')
        self.openButton = QPushButton('open')
        self.closeButton = QPushButton('close')
        self.MEDButton = QPushButton('morphological edge detection')
        self.MGButton = QPushButton('morphological gradient')

        self.hw2Controls.addWidget(self.sizeWidget)
        self.hw2Controls.addWidget(self.dilateButton)
        self.hw2Controls.addWidget(self.erodeButton)
        self.hw2Controls.addWidget(self.openButton)
        self.hw2Controls.addWidget(self.closeButton)
        self.hw2Controls.addWidget(self.MEDButton)
        self.hw2Controls.addWidget(self.MGButton)

        self.generalLayout.addLayout(self.hw2Controls, stretch=2)
        
    def _createImage(self):
        self.image = QLabel()

        self.image.setPixmap(QPixmap(self.imagePath))
        self.generalLayout.addWidget(self.image, stretch=2, alignment=Qt.AlignCenter)

    def setImage(self, img):
        ans = QImage(img.data, img.shape[1], img.shape[0], img.shape[1], QImage.Format_Grayscale8)
        self.image.setPixmap(QPixmap.fromImage(ans))