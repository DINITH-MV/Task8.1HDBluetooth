import asyncio
from bleak import BleakClient, BleakScanner
import RPi.GPIO as GPIO

RED_LED = 18
GREEN_LED = 23
DEVICE_NAME = "distanceMonitor"
CHAR_UUID = "5678"

GPIO.setmode(GPIO.BCM)
GPIO.setup(RED_LED, GPIO.OUT)
GPIO.setup(GREEN_LED, GPIO.OUT)
GPIO.output(RED_LED, GPIO.LOW)
GPIO.output(GREEN_LED, GPIO.LOW)

def notification_handler(_, data):
    level = int.from_bytes(data, byteorder="little")
    print(f"Level={level}")
    if level <= 30 and level > 10:
        GPIO.output(GREEN_LED, GPIO.HIGH)
        GPIO.output(RED_LED, GPIO.LOW)
    elif level <= 10:
        GPIO.output(RED_LED, GPIO.HIGH)
        GPIO.output(GREEN_LED, GPIO.LOW)
    else:
        GPIO.output(RED_LED, GPIO.LOW)
        GPIO.output(GREEN_LED, GPIO.LOW)

async def run():
    devices = await BleakScanner.discover(timeout=2.0)
    target = next((d for d in devices if d.name == DEVICE_NAME), None)
    if not target:
        print("Sensor not found")
        return
    async with BleakClient(target.address) as client:
        await client.start_notify(CHAR_UUID, notification_handler)
        while True:
            await asyncio.sleep(1)

if __name__ == "__main__":
    try:
        asyncio.run(run())
    except KeyboardInterrupt:
        pass
    finally:
        GPIO.cleanup()
