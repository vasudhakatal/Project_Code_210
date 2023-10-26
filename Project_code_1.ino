import tkinter as tk
from tkinter import ttk
import socket
import threading
import RPi.GPIO as GPIO 

GPIO.setwarnings(False)
# Create a socket object for data communication
host = '192.168.1.12'  # Listen on all available network interfaces
port = 12345  # Use the same port as on the Arduino
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((host, port))
server_socket.listen(5)  # Listen for incoming connections

# Create a tkinter window
window = tk.Tk()
window.title("Sensor Data and Chemical Dispenser")

# Variables to store data for updating the GUI
SERVO_PIN_1 = 17  # GPIO pin for servo 1
temperature_var = tk.StringVar()
turbidity_var = tk.StringVar()
action_var = tk.StringVar()


GPIO.setmode(GPIO.BCM)
GPIO.setup(SERVO_PIN_1, GPIO.OUT)


# Function to control servo 1
def control_servo_1():
    action_label.config(text="Dispensing reagent using servo 1")
    for angle in range(0, 181, 1):
        GPIO.output(SERVO_PIN_1, GPIO.HIGH)
    for angle in range(180, -1, -1):
        GPIO.output(SERVO_PIN_1, GPIO.HIGH)
# Function to accept connections and receive data from the Arduino
def receive_data():
    while True:
        client_socket, client_address = server_socket.accept()
        data = client_socket.recv(1024).decode()
        print(f"Received data: {data}")
        if data == "ADD_CHEMICAL":
            add_chemical()
        else:
            update_sensor_values(data)
        client_socket.close()

# Function to update the temperature label
def update_sensor_values(data):
    temperature, turbidity = data.split(",")  # Assuming data is in the format "temperature, turbidity"
    temperature_var.set(f"TURBIDITY: {temperature} ")
    turbidity_var.set(f"TEMPERATURE: {turbidity} °C")

# Function to control the servo motor for chemical addition
def add_chemical():
    action_var.set("Dispensing chemical using servo 1")
    control_servo_1()
    # Add your code to control the servo motor here
    # For example, if you're using RPi.GPIO:
    # GPIO.output(SERVO_PIN_1, GPIO.HIGH)

# Function to stop chemical addition
def stop_chemical_addition():
    action_var.set("Chemical dispensing stopped")
    # Add your code to stop the servo motor here
    # For example, if you're using RPi.GPIO:
    # GPIO.output(SERVO_PIN_1, GPIO.LOW)

# Create a frame for the sensor values
sensor_frame = tk.Frame(window)
sensor_frame.pack(pady=10)

# Temperature label
temperature_label = ttk.Label(sensor_frame, textvariable=temperature_var)
temperature_label.grid(row=0, column=0, padx=10, pady=10)

# Turbidity label
turbidity_label = ttk.Label(sensor_frame, textvariable=turbidity_var)
turbidity_label.grid(row=1, column=0, padx=10, pady=10)

# Action label
action_label = ttk.Label(sensor_frame, textvariable=action_var)
action_label.grid(row=2, column=0, padx=10, pady=10)

# Add a frame for better layout
frame = ttk.Frame(window)
frame.pack(pady=20)

# Dispense button for chemical
chemical_button = ttk.Button(frame, text="Dispense Chemical", command=add_chemical)
chemical_button.grid(row=0, column=0, padx=10, pady=10)

# Stop button for chemical addition
stop_button = ttk.Button(frame, text="Stop Chemical", command=stop_chemical_addition)
stop_button.grid(row=0, column=1, padx=10, pady=10)

# Start a thread to receive data from the Arduino
data_thread = threading.Thread(target=receive_data)
data_thread.daemon = True
data_thread.start()

# Start the GUI main loop
window.mainloop()