import asyncio
from bleak import BleakClient, BleakScanner
import RPi.GPIO as GPIO

LED_PIN = 18
SERVICE_UUID = "180C"
CHAR_UUID    = "1A79"

GPIO.setmode(GPIO.BCM)
GPIO.setup(LED_PIN, GPIO.OUT)
pwm = GPIO.PWM(LED_PIN, 1000)  # 1 kHz
pwm.start(0)

def notification_handler(_, data):
    level = int.from_bytes(data, byteorder="little")
    # Map 0?1023 to 0?100% PWM
    duty = max(0, min(100, level * 100 // 1023))
    pwm.ChangeDutyCycle(duty)
    print(f"Light={level}, PWM={duty}%")

async def run():
    devices = await BleakScanner.discover()
    target = next((d for d in devices if d.name == "distanceMonitor"), None)
    if not target:
        print("Sensor not found")
        return

    async with BleakClient(target.address) as client:
        await client.start_notify(CHAR_UUID, notification_handler)
        print("Receiving sensor updates?press Ctrl+C to stop")
        while True:
            await asyncio.sleep(1)

if __name__ == "__main__":
    try:
        asyncio.run(run())
    except KeyboardInterrupt:
        pass
    finally:
        pwm.stop()
        GPIO.cleanup()
