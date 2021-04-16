import matplotlib.pyplot as plt                                                               
from matplotlib.backends.backend_pdf import PdfPages

def make_pyplot(Xdata,Ydata,Xlab,Ylab):
    plt.plot(Xdata,Ydata)  
    plt.xlabel(Xlab)
    plt.ylabel(Ylab)

def show_pyplot():    
    plt.show()                       

def pdf_save_pyplot(Xdata,Ydata,Xlab,Ylab,file):
    with PdfPages(file) as outfile:
        plt.plot(Xdata,Ydata)  
        plt.xlabel(Xlab)
        plt.ylabel(Ylab)
        outfile.savefig()
