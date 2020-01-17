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
        elif mode == 'dilate':
            size = self._view.SESlider.value()
            imgf = self._model.dilate(self._view.imagePath, size)
        elif mode == 'erode':
            size = self._view.SESlider.value()
            imgf = self._model.erode(self._view.imagePath, size)
        elif mode == 'open':
            size = self._view.SESlider.value()
            imgf = self._model.erode(self._view.imagePath, size)
            img = np.zeros(imgf.shape, dtype=np.uint8)
            img[...] = imgf[...]
            imgf = self._model.dilate(None, size, rawImage=img)
        elif mode == 'close':
            size = self._view.SESlider.value()
            imgf = self._model.dilate(self._view.imagePath, size)
            img = np.zeros(imgf.shape, dtype=np.uint8)
            img[...] = imgf[...]
            imgf = self._model.erode(None, size, rawImage=img)
        elif mode == 'MED':
            size = self._view.SESlider.value()
            imgfd = self._model.dilate(self._view.imagePath, size)
            imgfe = self._model.erode(self._view.imagePath, size)
            imgf = imgfd - imgfe
        elif mode == 'MG':
            size = self._view.SESlider.value()
            imgfd = self._model.dilate(self._view.imagePath, size)
            imgfe = self._model.erode(self._view.imagePath, size)
            imgf = (imgfd - imgfe) * 0.5
        
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
        self._view.dilateButton.clicked.connect(lambda: self._setImage('dilate'))
        self._view.erodeButton.clicked.connect(lambda: self._setImage('erode'))
        self._view.openButton.clicked.connect(lambda: self._setImage('open'))
        self._view.closeButton.clicked.connect(lambda: self._setImage('close'))
        self._view.MEDButton.clicked.connect(lambda: self._setImage('MED'))
        self._view.MGButton.clicked.connect(lambda: self._setImage('MG'))
        
