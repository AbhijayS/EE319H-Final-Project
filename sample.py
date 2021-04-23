import serial
from matplotlib import pyplot as plt
from matplotlib.animation import FuncAnimation

s = serial.Serial('COM3', baudrate=115200, timeout=.01)

while True:
    print(s.readline())

anim = FuncAnimation(fig, animate, init_func=init,
                               frames=200, interval=20, blit=True)