import numpy as np

class Controller:

    def __init__(self, view, model):
        self._view = view
        self._model = model
        self._connectSignals()

    def _setImage(self, mode):
        if mode == 'roberts':
            imgf = self._model.roberts(self._view.imagePath) * 255
        elif mode == 'prewitt':
            imgf = self._model.prewitt(self._view.imagePath) * 255
        elif mode == 'sobel':   
            imgf = self._model.sobel(self._view.imagePath) * 255
        elif mode == 'gaussian':
            size = self._view.sizeSlider.value()
            sigma = self._view.sigmaSlider.value() / 10.
            imgf = self._model.gaussian(self._view.imagePath, size, sigma)
        elif mode == 'mean':
            size = self._view.sizeSlider.value()
            imgf = self._model.mean(self._view.imagePath, size)
        elif mode == 'median':
            size = self._view.sizeSlider.value()
            imgf = self._model.median(self._view.imagePath, size)
        
        img = np.zeros(imgf.shape, dtype=np.uint8)
        img[...] = imgf[...]

        self._view.setImage(img)

    def _connectSignals(self):
        self._view.robertsButton.clicked.connect(lambda : self._setImage('roberts'))
        self._view.prewittButton.clicked.connect(lambda : self._setImage('prewitt'))
        self._view.sobelButton.clicked.connect(lambda: self._setImage('sobel'))
        self._view.gaussianButton.clicked.connect(lambda: self._setImage('gaussian'))
        self._view.meanButton.clicked.connect(lambda: self._setImage('mean'))
        self._view.medianButton.clicked.connect(lambda: self._setImage('median'))
        
