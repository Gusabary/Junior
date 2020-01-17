import cv2
import numpy as np
import math

class Model:

    def convolution(self, kernel, image, kernelFunc=None):
        kernelWidth, kernelHeight = kernel.shape
        paddedImage = np.pad(image, ((0, kernelHeight), (0, kernelWidth)), "constant", constant_values=128)  # todo: more precisely padding
        flattedKernel = kernel.flatten()

        result = np.zeros(image.shape, dtype=image.dtype)
        for i in range(result.shape[0]):
            for j in range(result.shape[1]):
                convImage = paddedImage[i:i + kernelHeight, j:j + kernelWidth].reshape(-1)
                if kernelFunc is None:
                    result[i, j] = flattedKernel.dot(convImage)
                else:
                    result[i, j] = kernelFunc(convImage)
        return result

    def operator(self, verOp, horOp, imgpath):
        img = np.array(cv2.imread(imgpath), dtype=np.int16)
        img = img[:,:, 0]  # select R channel as gray scale
        verRes = self.convolution(verOp, img)
        horRes = self.convolution(horOp, img)

        res = np.zeros(img.shape)
        for i in range(img.shape[0]):
            for j in range(img.shape[1]):
                res[i, j] = abs(verRes[i, j]) + abs(horRes[i, j])
        for i in range(img.shape[0]):
            for j in range(img.shape[1]):
                res[i, j] = (float(res[i, j]) - res.min()) / float(res.max() - res.min())

        return res
    
    def filter(self, kernel, imgpath, kernelFunc=None):
        img = np.array(cv2.imread(imgpath), dtype=np.int16)
        img = img[:,:, 0]  # select R channel as gray scale
        res = self.convolution(kernel, img, kernelFunc)
        return res

    def roberts(self, imgpath):
        verOp = np.array([[1, 0], [0, -1]])
        horOp = np.array([[0, 1], [-1, 0]])
        res = self.operator(verOp, horOp, imgpath)
        return res

    def prewitt(self, imgpath):
        verOp = np.array([[1, 0, -1], [1, 0, -1], [1, 0, -1]])
        horOp = np.array([[1, 1, 1], [0, 0, 0], [-1, -1, -1]])
        res = self.operator(verOp, horOp, imgpath)
        return res

    def sobel(self, imgpath):
        verOp = np.array([[1, 0, -1], [2, 0, -2], [1, 0, -1]])
        horOp = np.array([[1, 2, 1], [0, 0, 0], [-1, -2, -1]])
        res = self.operator(verOp, horOp, imgpath)
        return res

    def gaussian(self, imgpath, size, sigma):
        kernel = np.zeros((size, size))
        for i in range(size):
            for j in range(size):
                x = i - (size - 1) / 2
                y = j - (size - 1) / 2
                kernel[i, j] = self.gaussianFunc(x, y, sigma)
        kernel = kernel / kernel.sum()
        res = self.filter(kernel, imgpath)
        return res
    
    def mean(self, imgpath, size):
        kernel = np.ones((size, size))
        kernel = kernel / kernel.sum()
        res = self.filter(kernel, imgpath)
        return res

    def median(self, imgpath, size):
        kernel = np.zeros((size, size))

        def getMedian(pixs):
            return np.median(pixs)

        res = self.filter(kernel, imgpath, kernelFunc=getMedian)
        return res

    def gaussianFunc(self, x, y, sigma):
        return math.exp(-(x ** 2 + y ** 2) / (2 * (sigma ** 2)))

