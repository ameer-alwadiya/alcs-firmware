#!/usr/bin/env python3
"""
Simple PWM Control Script
Usage: python simple_pwm_test.py [duty_cycle]
Example: python simple_pwm_test.py 500
"""

import paho.mqtt.client as mqtt
import sys
import time
import json

BROKER = "broker.hivemq.com"
PORT = 1883
TOPIC_CONTROL = "devices/stm32/control"
TOPIC_PWM = "sensors/stm32/pwm"

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print(f"✓ Connected to {BROKER}")
        client.subscribe(TOPIC_PWM)
    else:
        print(f"✗ Connection failed with code {rc}")

def on_message(client, userdata, msg):
    try:
        data = json.loads(msg.payload.decode())
        if 'duty' in data:
            percentage = (data['duty'] * 100) / 1000
            print(f"✓ STM32 Response: Duty={data['duty']}/1000 ({percentage:.1f}%)")
            if 'status' in data:
                print(f"  Status: {data['status']}")
        if 'error' in data:
            print(f"✗ Error from STM32: {data['error']}")
    except:
        print(f"  Response: {msg.payload.decode()}")

def send_pwm(duty):
    """Send single PWM command and exit"""
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    
    print(f"\nSending PWM command: {duty}/1000 ({(duty*100)/1000:.1f}%)")
    print("-" * 50)
    
    client.connect(BROKER, PORT, 60)
    client.loop_start()
    
    time.sleep(1)  # Wait for connection
    
    message = json.dumps({"duty": duty})
    result = client.publish(TOPIC_CONTROL, message)
    
    if result.rc == mqtt.MQTT_ERR_SUCCESS:
        print("✓ Command sent, waiting for response...")
        time.sleep(2)  # Wait for response
    else:
        print(f"✗ Failed to send command")
    
    client.loop_stop()
    client.disconnect()

def monitor_mode():
    """Continuous monitoring mode"""
    def on_connect_monitor(client, userdata, flags, rc):
        print(f"✓ Connected to {BROKER}")
        print("\nSubscribed to:")
        client.subscribe("sensors/stm32/mq6")
        print("  - sensors/stm32/mq6")
        client.subscribe("sensors/stm32/bmp280")
        print("  - sensors/stm32/bmp280")
        client.subscribe("sensors/stm32/pwm")
        print("  - sensors/stm32/pwm")
        client.subscribe("devices/stm32/status")
        print("  - devices/stm32/status")
        print("\nListening for messages (Ctrl+C to stop)...\n")
    
    def on_message_monitor(client, userdata, msg):
        timestamp = time.strftime("%H:%M:%S")
        try:
            data = json.loads(msg.payload.decode())
            print(f"[{timestamp}] {msg.topic}")
            print(f"  {json.dumps(data, indent=2)}")
        except:
            print(f"[{timestamp}] {msg.topic}: {msg.payload.decode()}")
        print()
    
    client = mqtt.Client()
    client.on_connect = on_connect_monitor
    client.on_message = on_message_monitor
    
    try:
        client.connect(BROKER, PORT, 60)
        client.loop_forever()
    except KeyboardInterrupt:
        print("\n\nStopping monitor...")
        client.disconnect()

def print_usage():
    print("""
STM32 PWM Control Script
========================

Usage:
  python simple_pwm_test.py [duty_cycle]    Send PWM command
  python simple_pwm_test.py monitor         Monitor all topics
  python simple_pwm_test.py --help          Show this help

Examples:
  python simple_pwm_test.py 0      # Turn OFF (0%)
  python simple_pwm_test.py 250    # Set 25%
  python simple_pwm_test.py 500    # Set 50%
  python simple_pwm_test.py 750    # Set 75%
  python simple_pwm_test.py 1000   # Set 100%
  python simple_pwm_test.py monitor # Monitor all messages

Valid duty cycle range: 0 - 1000
    """)

def main():
    if len(sys.argv) < 2:
        print_usage()
        sys.exit(1)
    
    arg = sys.argv[1]
    
    if arg in ['--help', '-h', 'help']:
        print_usage()
        sys.exit(0)
    
    if arg.lower() == 'monitor':
        monitor_mode()
        sys.exit(0)
    
    try:
        duty = int(arg)
        if duty < 0 or duty > 1000:
            print(f"✗ Error: Duty cycle must be between 0 and 1000")
            print(f"  You entered: {duty}")
            sys.exit(1)
        
        send_pwm(duty)
        
    except ValueError:
        print(f"✗ Error: Invalid duty cycle value: {arg}")
        print(f"  Must be a number between 0 and 1000")
        sys.exit(1)

if __name__ == "__main__":
    main()