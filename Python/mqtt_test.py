#!/usr/bin/env python3
"""
Improved PWM Control Script with Better Synchronization
Usage: python improved_mqtt_pwm_test.py [duty_cycle]
Example: python improved_mqtt_pwm_test.py 500
"""

import paho.mqtt.client as mqtt
import sys
import time
import json
import threading

BROKER = "broker.hivemq.com"
PORT = 1883
TOPIC_CONTROL = "devices/stm32/control"
TOPIC_PWM = "sensors/stm32/pwm"

class PWMController:
    def __init__(self):
        self.connected = threading.Event()
        self.subscribed = threading.Event()
        self.response_received = threading.Event()
        self.client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message
        self.client.on_subscribe = self.on_subscribe
        
    def on_connect(self, client, userdata, flags, reason_code, properties):
        if reason_code == 0:
            print(f"✓ Connected to {BROKER}")
            client.subscribe(TOPIC_PWM)
        else:
            print(f"✗ Connection failed with code {reason_code}")
        self.connected.set()
    
    def on_subscribe(self, client, userdata, mid, reason_codes, properties):
        print(f"✓ Subscribed to {TOPIC_PWM}")
        self.subscribed.set()
    
    def on_message(self, client, userdata, msg):
        try:
            data = json.loads(msg.payload.decode())
            if 'duty' in data:
                percentage = (data['duty'] * 100) / 1000
                print(f"✓ STM32 Response: Duty={data['duty']}/1000 ({percentage:.1f}%)")
                if 'status' in data:
                    print(f"  Status: {data['status']}")
            if 'error' in data:
                print(f"✗ Error from STM32: {data['error']}")
            self.response_received.set()
        except Exception as e:
            print(f"  Response: {msg.payload.decode()}")
            self.response_received.set()
    
    def send_pwm(self, duty, timeout=5):
        """Send PWM command with proper synchronization"""
        print(f"\nSending PWM command: {duty}/1000 ({(duty*100)/1000:.1f}%)")
        print("-" * 50)
        
        # Connect to broker
        self.client.connect(BROKER, PORT, 100)
        self.client.loop_start()
        
        # Wait for connection
        if not self.connected.wait(timeout=6):
            print("✗ Connection timeout")
            self.cleanup()
            return False
        
        # Wait for subscription to complete
        if not self.subscribed.wait(timeout=6):
            print("✗ Subscription timeout")
            self.cleanup()
            return False
        
        # Now send the command
        message = json.dumps({"duty": duty})
        result = self.client.publish(TOPIC_CONTROL, message)
        
        if result.rc == mqtt.MQTT_ERR_SUCCESS:
            print("✓ Command sent, waiting for response...")
            
            # Wait for response with timeout
            if self.response_received.wait(timeout=timeout):
                print("✓ Response received successfully")
                success = True
            else:
                print(f"✗ No response received within {timeout} seconds")
                success = False
        else:
            print(f"✗ Failed to send command")
            success = False
        
        self.cleanup()
        return success
    
    def cleanup(self):
        """Clean shutdown"""
        time.sleep(0.5)  # Brief delay to ensure message processing
        self.client.loop_stop()
        self.client.disconnect()

def monitor_mode():
    """Continuous monitoring mode"""
    def on_connect_monitor(client, userdata, flags, reason_code, properties):
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
        client.subscribe("sensors/stm32/combined")
        print("  - sensors/stm32/combined")
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
    
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    client.on_connect = on_connect_monitor
    client.on_message = on_message_monitor
    
    try:
        client.connect(BROKER, PORT, 100)
        client.loop_forever()
    except KeyboardInterrupt:
        print("\n\nStopping monitor...")
        client.disconnect()

def print_usage():
    print("""
STM32 PWM Control Script (Improved)
===================================

Usage:
  python improved_mqtt_pwm_test.py [duty_cycle]    Send PWM command
  python improved_mqtt_pwm_test.py monitor         Monitor all topics
  python improved_mqtt_pwm_test.py --help          Show this help

Examples:
  python improved_mqtt_pwm_test.py 0      # Turn OFF (0%)
  python improved_mqtt_pwm_test.py 250    # Set 25%
  python improved_mqtt_pwm_test.py 500    # Set 50%
  python improved_mqtt_pwm_test.py 750    # Set 75%
  python improved_mqtt_pwm_test.py 1000   # Set 100%
  python improved_mqtt_pwm_test.py monitor # Monitor all messages

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
        
        controller = PWMController()
        success = controller.send_pwm(duty, timeout=5)
        sys.exit(0 if success else 1)
        
    except ValueError:
        print(f"✗ Error: Invalid duty cycle value: {arg}")
        print(f"  Must be a number between 0 and 1000")
        sys.exit(1)

if __name__ == "__main__":
    main()