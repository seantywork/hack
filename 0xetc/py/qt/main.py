# Import libraries
import numpy as np
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *

import pyqtgraph as pg
import random


class Worker(QRunnable):

    def __init__(self, *args, **kwargs):
        super(Worker, self).__init__()

        self.app = kwargs["app"]
        self.Xm = kwargs["Xm"]
        self.ptr = kwargs["ptr"]
        self.curve = kwargs["curve"]

    @pyqtSlot()
    def run(self):
        while True:
            self.update()


    def update(self):
            
        self.Xm[:-1] = self.Xm[1:]                      
        value =  random.randrange(256)              
        self.Xm[-1] = float(value)                 
        self.ptr += 1                             
        self.curve.setData(self.Xm)                     
        self.curve.setPos(self.ptr,0)                  
        self.app.processEvents()   


class MainWindow(QMainWindow):
    def __init__(self, app):
        super().__init__()

        # Temperature vs time plot
        self.app = app


        self.win = pg.PlotWidget() # creates a window
        self.setCentralWidget(self.win)
    
        self.curve = self.win.plot()                      

        self.windowWidth = 500                      
        self.Xm = np.linspace(0,0,self.windowWidth)           
        self.ptr = self.windowWidth * -1       


        self.threadpool = QThreadPool()              

        self.worker = Worker(

            app=self.app, 
            Xm=self.Xm, 
            ptr=self.ptr, 
            curve=self.curve
            
            )


        self.threadpool.start(self.worker)



app = QApplication([])

main = MainWindow(app)
main.show()

app.exec_()