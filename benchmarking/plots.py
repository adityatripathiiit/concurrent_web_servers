# Python script to generate plots from the CSV files
import matplotlib.pyplot as plt
import csv
from scipy.interpolate import make_interp_spline,BSpline
import numpy as np

def plot_graph():

    x=[]
    y=[]
    file_name1 = "./data/concurr_test_fifo.csv"
    
    with open(file_name1, 'r') as csvfile:
        plots= csv.reader(csvfile, delimiter=',')
        for row in plots:
            x.append(float(row[0]))
            y.append(float(row[1]))

    x_new = np.linspace(min(x), max(x), 2000) 
    spl = make_interp_spline(x, y, k=3)
    y_smooth = spl(x_new)

    plt.plot(x_new,y_smooth, color="orange", label="FIFO", linewidth=1.5)


    x=[]
    y=[]
    file_name1 = "./data/concurr_test_sff.csv"
    
    with open(file_name1, 'r') as csvfile:
        plots= csv.reader(csvfile, delimiter=',')
        for row in plots:
            x.append(float(row[0]))
            y.append(float(row[1]))

    x_new = np.linspace(min(x), max(x), 2000) 
    spl = make_interp_spline(x, y, k=3)
    y_smooth = spl(x_new)

    plt.plot(x_new,y_smooth, color="blue", label="SFF", linewidth=1.5)


    
    plt.title("Work Load Comparison")

    plt.xlabel('Number of Concurrent Users')
    plt.ylabel('Time (in sec)')
    plt.legend(loc='best')

    plt.savefig("./plots/cocurr_test_compare",dpi = 600)
    plt.close()


plot_graph()
