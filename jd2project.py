import json
import time
import asyncio
from bleak import BleakClient
import math
from tkinter import Tk
from tkinter.filedialog import askopenfilename


#arm length in millimeters
arm1_length = 225
arm2_length = 188

SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"
MAX_CONNECT_ATTEMPTS = 10


"""def read_json(json_path):
    
    Reads from a json file and returns the gcode commands in order from top to bottom.

    :param json_path: A string which represents the json file path.

    :return: A list of strings that represent gcode commands and coordinates
    
    with open(json_path, 'r') as file:
        data = json.load(file)
        return data"""


def read_json():
    """
    Opens a file picker dialog and reads G-code JSON from the selected file.

    :return: A list of G-code command dicts or None if cancelled.
    """
    Tk().withdraw()  # Hide root window
    json_path = askopenfilename(
        title="Select G-code JSON file",
        filetypes=[("JSON files", "*.json"), ("All files", "*.*")]
    )
    if not json_path:
        print("No file selected.")
        return None

    with open(json_path, 'r') as file:
        data = json.load(file)
        return data


def angle2(x, y):
    """
    Computes the required angle of the second motor
    :param x: The desired x coordinate
    :param y: The desired y coordinate
    :return: A numerical angle in radians
    """
    top = (x**2) + (y**2) - (arm1_length**2) - (arm2_length**2)
    bottom = 2*arm1_length*arm2_length
    value = top / bottom
    #clamp the value to avoid floating point errors
    value = max(min(value, 1), -1)
    radians = math.acos(value)
    return radians


def angle1(x, y, phi):
    """
    Computes the required angle of the first motor using the angle of the second motor
    :param x: The desired x coordinate
    :param y: The desired y coordinate
    :param phi: The angle calculated for the second motor in radians
    :return: A numerical angle in degrees
    """
    term1 = math.atan2(y, x)
    term2 = math.atan2(arm2_length*math.sin(phi), arm1_length + (arm2_length*math.cos(phi)))
    return term1 - term2


def compute_angles(commands):
    """
    Computes the angles the motors need to rotate to for the arm to arrive at the endpoint
    :param commands: a list of dictionaries with the gcode, X, Y data.
    :return: a list of dictionaries with the gcode, angle1, angle2 data.
    """
    messages = []

    for item in commands:
        temp_dict = dict()
        temp_dict["gcode"] = item["gcode"]
        a2 = angle2(item['x'] + 50, item['y'] + 50)
        a1 = angle1(item["x"] + 50, item["y"] + 50, a2)
        temp_dict["a2"] = a2 * (180/math.pi)
        temp_dict["a1"] = a1 * (180/math.pi)
        messages.append(temp_dict)

    return messages


def generate_label(index):
    """
    Generates a label prior to sending a message.

    :param index: Index for the list of GCode commands.

    :return: A label based on the index and current time.
    """
    return f"msg_{index}_{int(time.time())}"


async def send_data(client, commands):
    """
    Sends a series of strings to the client and waits for a response.

    :param client: Object which acts as the bluetooth client.
    :param commands: List of commands that the esp32 should follow.
    """
    confirmation_event = asyncio.Event()
    last_confirmation = ""

    def notification_handler(sender, data):
        nonlocal last_confirmation
        last_confirmation = data.decode()
        print(f"Notification received: {last_confirmation}")
        confirmation_event.set()

    await client.start_notify(CHARACTERISTIC_UUID, notification_handler)

    for idx, command in enumerate(commands):
        #generate a label for message confirmation
        label = generate_label(idx)
        print(label)
        #combine the label with the command
        full_msg = f"{label}: {command}"

        for attempt in range(1, MAX_CONNECT_ATTEMPTS):
            print(f"Attempt {attempt}: Sending {full_msg}")
            confirmed = False
            try:
                confirmation_event.clear()
                await client.write_gatt_char(CHARACTERISTIC_UUID, full_msg.encode())
                try:
                    await asyncio.wait_for(confirmation_event.wait(), timeout=2)
                    if label in last_confirmation:
                        print(f"Message {label} confirmed")
                        confirmed = True
                        break
                    else:
                        print(f"Unexpected confirmation received: {last_confirmation}")
                except asyncio.TimeoutError:
                    print("Confirmation Timeout")
            except Exception as e:
                print(f"No Confirmation: {e}")

            if confirmed:
                break
        else:
            print("message failed to send. Exiting program!")
            return

    return


async def main():
    #address of the ESP32
    address = "E0F2723A-DDED-FBEB-D5D7-8908EF925A6D"

    #read the json file and store the commands as a list
    commands = read_json()
    message = compute_angles(commands)
    #print(message)

    #attempt a connection with the ESP32.
    async with BleakClient(address) as client:
        #print that the connection was successful
        print(f"Connected: {client.is_connected}")

        for service in client.services:
            print(f"[Service] {service.uuid}")
            for char in service.characteristics:
                print(f"  [Characteristic] {char.uuid} - {char.properties}")

        #send the data to the esp32 for processing
        await send_data(client, message)


if __name__ == '__main__':
    asyncio.run(main())
