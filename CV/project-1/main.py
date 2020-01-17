import sys
import view
import controller
import model

from PyQt5.QtWidgets import QApplication

def main():
    app = QApplication(sys.argv)

    imgPath = "./test2.png"

    appView = view.View(imgPath)
    appView.show()

    appModel = model.Model()

    appCtrl = controller.Controller(view=appView, model=appModel)

    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
