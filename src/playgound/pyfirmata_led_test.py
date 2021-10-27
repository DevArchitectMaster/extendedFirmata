from pyfirmata import ArduinoMega, util
import time

board = ArduinoMega('COM5') # Change to your port

led_on = False

def toggleLED():
  global led_on
  if led_on == True:
    print('Led OFF')
    led_13.write(0)
    led_on = False
  else:
    print('Led ON')
    led_13.write(1)
    led_on= True

it = util.Iterator(board)
it.start()

led_13 = board.get_pin('d:13:o') # board.digital[13] #p pulse modulated signal
button_pin = board.get_pin('d:10:i')
b_time = 0.2

while True:
    result = button_pin.read()
    if result == True:
      toggleLED()
    #print("\n")
    time.sleep(b_time)


# https://realpython.com/arduino-python/


###################################################################

'''
print("Start blinking D13")

while True:
    board.digital[13].write(1)
    time.sleep(1)
    board.digital[13].write(0)
    time.sleep(1)
'''
'''
while True:
    led_13.write(1)
    print('Led ON')
    time.sleep(b_time)
    led_13.write(0)
    print('Led OFF')
    time.sleep(b_time)
'''