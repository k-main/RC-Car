#Version final: Real time data values plotted, but not wirelessly
#Esp32 number 2 has not been cooperating, and might have died

#https://discuss.python.org/t/how-to-send-and-receive-serial-in-python/10394
#https://matplotlib.org/stable/users/explain/animations/animations.html
##https://www.w3schools.com/python/ref_string_split.asp
#https://www.geeksforgeeks.org/dynamically-updating-plot-in-matplotlib/

import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np

SERIAL_PORT = 'COM12'
BAUD_RATE = 115200

# Initialize serial connection
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
angles1, distances1 = np.array([]), np.array([])
angles2, distances2 = np.array([]), np.array([])
MAX_POINTS = 360

def process_line(line):
    global angles1, distances1, angles2, distances2
    try:
        # Check for SCAN_COMPLETE to reset data
        if "SCAN_COMPLETE" in line:
            angles1, distances1 = np.array([]), np.array([])
            angles2, distances2 = np.array([]), np.array([])
        elif "Angle1" in line and "Distance1" in line:
            parts = line.split("|")
            if len(parts) == 4:  # Only 4 parts/values
                angle1_part = parts[0].split(":")
                distance1_part = parts[1].split(":")
                angle2_part = parts[2].split(":")
                distance2_part = parts[3].split(":")
                if len(angle1_part) > 1 and len(distance1_part) > 1 and len(angle2_part) > 1 and len(distance2_part) > 1:
                    angle1 = int(angle1_part[1].strip())
                    distance1 = int(distance1_part[1].strip())
                    angle2 = int(angle2_part[1].strip())
                    distance2 = int(distance2_part[1].strip())

                    # Filter out distances kind of, 1 sensor went bad
                    if 20 <= distance1 <= 2000:
                        angles1 = np.append(angles1, angle1)[-MAX_POINTS:]
                        distances1 = np.append(distances1, distance1)[-MAX_POINTS:]
                    if 20 <= distance2 <= 2000:
                        angles2 = np.append(angles2, angle2)[-MAX_POINTS:]
                        distances2 = np.append(distances2, distance2)[-MAX_POINTS:]
                else:
                    print(f"Invalid format : {line}")
            else:
                print(f"TOO MANY PARTS: {line}")
    except ValueError as e:
        print(f"ValueError: {e} for line: {line}")


def update_plot(frame):
    global angles1, distances1, angles2, distances2

    if ser.in_waiting > 0:
        raw_data = ser.read_all().decode('utf-8').splitlines()
        for line in raw_data:
            process_line(line.strip())

    ax.clear()
    ax.set_theta_zero_location('N')
    ax.set_theta_direction(-1)
    ax.set_rmax(1000)
    ax.set_rmin(200)

    if angles1.size > 0 and distances1.size > 0:
        ax.scatter(np.radians(angles1), distances1, c='blue', label='Sensor 1', s=10)
    if angles2.size > 0 and distances2.size > 0:
        ax.scatter(np.radians(angles2), distances2, c='red', label='Sensor 2', s=10)

    ax.set_title("Object Detection", va='bottom')
    ax.legend(loc='upper right')

fig = plt.figure(figsize=(8, 8))
ax = plt.subplot(111, polar=True)
ani = FuncAnimation(fig, update_plot, interval=300)  # Update every 300 ms

try:
    plt.show()
except KeyboardInterrupt:
    print("Exiting...")
finally:
    ser.close()
