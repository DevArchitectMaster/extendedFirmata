try:
  import pyfirmata
except:
  import pip
  pip.main(['install', 'pyfirmata'])
  import pyfirmata
finally:
  import time

port = 'COM6' # Change to your port
board = pyfirmata.ArduinoMega(port)

pattern = "###"
responseLength = 32
asyncResponse = {}

def sendAsyncStringRequest(requestString):
  if responseLength != 0:
    requestString = requestString[:responseLength]

  board.send_sysex(pyfirmata.STRING_DATA, pyfirmata.util.str_to_two_byte_iter(requestString))

def getAsyncSensorValue(operation, callnumber, result):
  requestString = pattern + str(operation) + pattern + str(callnumber) + pattern + str(result) + pattern
  sendAsyncStringRequest(requestString)

def _messageHandler(*args, **kwargs):
  if responseLength != 0:
    args = args[:responseLength*2]
  
  response_string = pyfirmata.util.two_byte_iter_to_str(args)

  if response_string.count(pattern)  == 4:
    response_list = response_string.split(pattern)
    response_list = response_list[1:-1]
    if response_list[2] == "":
      print("### ERROR 404: no sensor value get ###")
      response_list[2] = 0.0
    asyncResponse[response_list[1]] = float(response_list[2])

board.add_cmd_handler(pyfirmata.STRING_DATA, _messageHandler)

it = pyfirmata.util.Iterator(board)
it.start()

######################### START YOUR CODE HERE #########################

# example
while True:
  """
  # show clock on display
  clock = time.strftime('%H:%M:%S %p')
  sendAsyncStringRequest(clock)
  time.sleep(1)
  """

  """
  # read sensors
  sendAsyncStringRequest("###3###1###11###")
  time.sleep(1)
  print("sensor temp: ", asyncResponse["1"])
  sendAsyncStringRequest("###2###2###11###")
  time.sleep(1)
  print("sensor hum: ", asyncResponse["2"])
  sendAsyncStringRequest("###4###3###11###")
  time.sleep(1)
  print("sensor heat index: ", asyncResponse["3"])
  sendAsyncStringRequest("###1###4###11###")
  time.sleep(1)
  print("sensor rand: ", asyncResponse["4"])
  """
  
  print("--------------------------------------------------------")
  break

######################### END YOUR CODE HERE #########################

asyncResponse = {}
board.exit()